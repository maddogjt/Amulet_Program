#include "simon_mode.h"
#include <JC_Button.h>
#include "../animation/animations.h"
#include "../communication/advertising.h"
#include "../communication/signal.h"
#include "../settings/settings.h"
#include "../leds/led.h"
#include "../misc/simon_sequence.h"
#include "../misc/victory_sound.h"

#include <nrf.h>
#include <nrf_pwm.h>  // for Clockless
#include <nrf_nvic.h> // for Clockless / anything else using interrupts

constexpr int led_pins[4] = {25, 27, 30, 26};   // R,G,B,Y
constexpr int button_pins[4] = {15, 16, 7, 11}; // R,G,B,Y
constexpr int freqs[5] = {310, 415, 209, 252, 150};
constexpr int freq_bad = 150;

constexpr int32_t kLevelCount = 7;
constexpr uint8_t kLevelColors[kLevelCount] = { 2, 3, 4, 5, 6, 7, 8};
constexpr uint8_t kLevelSpeeds[kLevelCount] = {420 / 4, 420 / 4, 420 / 4, 420 / 4, 420 / 4, 420 / 4, 420 / 4};

Button buttons[4] = {{15}, {16}, {7}, {11}};

static void playTone(uint16_t freq, uint8_t intensity)
{
	NRF_PWM_Type *pwm = NRF_PWM2;

	FastPin<A4>::setOutput();
	FastPin<A4>::lo();
	FastPin<A5>::setOutput();
	FastPin<A5>::lo();

	//    uint16_t gFreq = 1000;
	uint16_t counterLength = 1000000 / freq * 2;

	// Pins must be set before enabling the peripheral
	pwm->PSEL.OUT[0] = FastPin<A4>::nrf_pin();
	pwm->PSEL.OUT[1] = NRF_PWM_PIN_NOT_CONNECTED;
	pwm->PSEL.OUT[2] = FastPin<A5>::nrf_pin();
	pwm->PSEL.OUT[3] = NRF_PWM_PIN_NOT_CONNECTED;
	nrf_pwm_enable(pwm);
	nrf_pwm_configure(pwm, NRF_PWM_CLK_1MHz, NRF_PWM_MODE_UP, counterLength);
	nrf_pwm_decoder_set(pwm, NRF_PWM_LOAD_GROUPED, NRF_PWM_STEP_AUTO);

	// clear any prior shorts / interrupt enable bits
	nrf_pwm_shorts_set(pwm, 0);
	nrf_pwm_int_set(pwm, 0);
	// clear all prior events
	nrf_pwm_event_clear(pwm, NRF_PWM_EVENT_STOPPED);
	nrf_pwm_event_clear(pwm, NRF_PWM_EVENT_SEQSTARTED0);
	nrf_pwm_event_clear(pwm, NRF_PWM_EVENT_SEQSTARTED1);
	nrf_pwm_event_clear(pwm, NRF_PWM_EVENT_SEQEND0);
	nrf_pwm_event_clear(pwm, NRF_PWM_EVENT_SEQEND1);
	nrf_pwm_event_clear(pwm, NRF_PWM_EVENT_PWMPERIODEND);
	nrf_pwm_event_clear(pwm, NRF_PWM_EVENT_LOOPSDONE);

	static uint16_t sequenceBuffer[2];

	sequenceBuffer[0] = counterLength / 2;
	sequenceBuffer[1] = counterLength / 2 | 0x8000;

	// config is easy, using SEQ0, no loops...
	nrf_pwm_sequence_t sequenceConfig;
	sequenceConfig.values.p_common = &(sequenceBuffer[0]);
	sequenceConfig.length = 2;
	sequenceConfig.repeats = 0;   // send the data once, and only once
	sequenceConfig.end_delay = 0; // no extra delay at the end of SEQ[0] / SEQ[1]
	nrf_pwm_sequence_set(pwm, 0, &sequenceConfig);
	nrf_pwm_sequence_set(pwm, 1, &sequenceConfig);
	nrf_pwm_loop_set(pwm, 0);

	nrf_pwm_event_clear(pwm, NRF_PWM_EVENT_STOPPED);

	nrf_pwm_task_trigger(pwm, NRF_PWM_TASK_SEQSTART0);
}

void stopTone()
{
	NRF_PWM_Type *pwm = NRF_PWM2;
	nrf_pwm_disable(pwm);
	FastPin<A4>::lo();
	FastPin<A5>::lo();
}

void SimonMode::start()
{
	for (auto pin : led_pins)
	{
		pinMode(pin, OUTPUT);
		digitalWrite(pin, 1);
	}

	for (auto &button : buttons)
	{
		button.begin();
	}

	startNewSequence(0);
}

void SimonMode::startNewSequence(int level)
{
	state_ = State::Playing;
	currentLevel_ = level;
	advertising_stop();

	srand(millis());

	if (level == 0) {
		simonSeed_ = rand();
	}

	anim_config_t anim{};
	anim.anim_ = Anim::AnimSimon;
	anim.extra0_ = simonSeed_;
	anim.speed_ = kLevelSpeeds[currentLevel_];
	anim.extra1_ = kLevelColors[currentLevel_];

	simonStepCount_ = generate_simon_sequence(anim.extra0_, anim.speed_, anim.extra1_, simonSteps_);

	auto &config = localSettings_.startupConfig_;
	advertising_start(AdvertisementType::Beacon, config.ad,
					  (uint8_t *)&anim, sizeof(anim_config_t));
	start_animation_if_new(anim);

	startStep(0);
}

void SimonMode::handlePressedState(int state)
{
}

#define MULTIPLE_BUTTONS 5

void SimonMode::loop()
{
	int now = millis();
	int pressed = 0;
	for (int i = 0; i < 4; i++)
	{
		if (buttons[i].read())
		{
			pressed = (pressed == 0) ? i + 1 : MULTIPLE_BUTTONS;
		}
	}

	if (state_ == State::Failure)
	{
		if (now > stepEnd_ && pressed == 0)
		{
			endFail();
		}
	}
	else if (state_ == State::Playing)
	{
		if (now > stepEnd_)
		{
			startFail();
		}
		else
		{
			SimonStep current = simonSteps_[currentSimonStep_];
			int nextStep = (currentSimonStep_ + 1) % simonStepCount_;
			SimonStep next = simonSteps_[nextStep];
			if ((int)current.color == pressed)
			{
				return;
			}
			else if ((int)next.color == pressed)
			{
				if (nextStep == 0)
				{
					if (kLevelCount == currentLevel_ + 1)
					{
						startFinalVictory();
					}
					else
					{
						startRoundVictory(current.color);
					}
				}
				else
				{
					startStep(nextStep);
				}
			}
			else
			{
				startFail();
			}
		}
	}
}

void SimonMode::startStep(int stepIndex)
{
	stopTone();

	currentSimonStep_ = stepIndex % simonStepCount_;
	stepEnd_ = stepIndex == 0 ? INT32_MAX : millis() + 3000;
	auto color = simonSteps_[currentSimonStep_].color;

	for (auto pin : led_pins)
	{
		digitalWrite(pin, 1);
	}

	if (color != SimonColor::None)
	{
		auto pinIdx = (int)color - 1;
		digitalWrite(led_pins[pinIdx], 0);
		playTone(freqs[pinIdx], 255);
	}
}

void SimonMode::startFail()
{
	state_ = State::Failure;
	stepEnd_ = millis() + 1500;
	playTone(freq_bad, 255);

	// turn on all lights
	for (auto p : led_pins)
	{
		digitalWrite(p, 0);
	}
}

void SimonMode::endFail()
{
	stopTone();

	// turn off all lights
	for (auto p : led_pins)
	{
		digitalWrite(p, 1);
	}

	// reset
	startNewSequence(0);
}

void SimonMode::startRoundVictory(SimonColor color)
{
	advertising_stop();
	startNewSequence(currentLevel_ + 1);
	delay(100);

	// for (int i = 0; i < 5; i++)
	// {
	// 	auto pinIdx = (int)color - 1;
	// 	digitalWrite(led_pins[pinIdx], 0);
	// 	playTone(freqs[pinIdx], 255);
	// 	delay(70);

	// 	digitalWrite(led_pins[pinIdx], 1);
	// 	stopTone();
	// 	delay(20);
	// }
	{
		int note = 0;
		long freq, length, delayNext;
		while (zelda_sound_get(note, freq, length, delayNext))
		{
			playTone(freq, 255);
			int led = rand() % 4;
			digitalWrite(led_pins[led], 0);

			delay(length);
			digitalWrite(led_pins[led], 1);
			//stopTone();
			//delay(delayNext);
			playTone(0, 255);
			delay(50);
			note++;
		}
		stopTone();
		delay(200);
	}
}

void SimonMode::startFinalVictory()
{
	advertising_stop();

	// Start sending victory
	anim_config_t anim{};
	anim.anim_ = Anim::AnimVictory;
	anim.extra0_ = 0;
	auto &config = localSettings_.startupConfig_;
	advertising_start(AdvertisementType::Beacon, config.ad, (uint8_t *)&anim, sizeof(anim_config_t));

	delay(100);

	for (int repeat = 0; repeat < 2; repeat++)
	{
		int note = 0;
		long freq, length, delayNext;
		while (victory_sound_get(note, freq, length, delayNext))
		{
			playTone(freq, 255);
			int led = rand() % 4;
			digitalWrite(led_pins[led], 0);

			delay(length);
			digitalWrite(led_pins[led], 1);
			//stopTone();
			//delay(delayNext);
			playTone(0, 255);
			delay(50);
			note++;
		}
		stopTone();
		delay(200);
	}

	delay(5000);

	startNewSequence(0);
}

void SimonMode::buttonPressMode()
{
	startNewSequence(currentLevel_);
}