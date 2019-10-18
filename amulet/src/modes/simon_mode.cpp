#include "simon_mode.h"
#include "../animation/animations.h"
#include "../communication/advertising.h"
#include "../communication/signal.h"
#include "../settings/settings.h"
#include "../leds/led.h"
#include "../misc/simon_sequence.h"

constexpr int led_pins[4] = {25, 27, 30, 26};   // R,G,B,Y
constexpr int button_pins[4] = {15, 16, 7, 11}; // R,G,B,Y

void SimonMode::start()
{
	startNewSequence();
	for (auto pin : led_pins) {
		pinMode(pin, OUTPUT);
		digitalWrite(pin, 1);
	}
}

void SimonMode::startNewSequence()
{
	advertising_stop();

	srand(millis());
	simonSeed_ = rand();
	simonLevel_ = 1;

	anim_config_t anim {};
	anim.anim_ = Anim::AnimSimon;
	anim.extra0_ = simonSeed_;
	anim.speed_ = 420/4;
	anim.extra1_ = 4;

	simonStepCount_ = generate_simon_sequence(anim.extra0_, anim.speed_*4, anim.extra1_, simonSteps_);

	auto &config = localSettings_.startupConfig_;
	advertising_start(AdvertisementType::Beacon, config.ad,
					  (uint8_t *)&anim, sizeof(anim_config_t));
	start_animation_if_new(anim);

	startStep(0);
}

// constexpr int freqs[4] = {310, 415, 209, 252};
// constexpr int freq_bad = 42;
// constexpr int duration_bad = 1500;

// constexpr int duration_level[3] = {420, 320, 220};
// constexpr int duration_between = 50;

// constexpr int seq[] = {0, 2, 3, 2, 2, 2, 1};

void SimonMode::startStep(int stepIndex)
{
	currentSimonStep_ = stepIndex % simonStepCount_;
	stepEnd_ = millis() + simonSteps_[currentSimonStep_].duration;
	auto color = simonSteps_[currentSimonStep_].color;

	for (auto pin : led_pins)
	{
		digitalWrite(pin, 1);
	}

	if (currentSimonStep_ == 0)
	{
		Serial.println("Reset");
	}

	if (color != SimonColor::None) {
		Serial.println((int)color); 

		auto pinIdx = (int)color -1;
		digitalWrite(led_pins[pinIdx], 0);
	}
}

void SimonMode::loop()
{
	if (millis() >= stepEnd_)
	{
		startStep(currentSimonStep_ + 1);
	}
}

void SimonMode::buttonPressMode()
{
	startNewSequence();
}