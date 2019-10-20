#include "seacomp_amulet_mode.h"
#include "../animation/animations.h"
#include "../communication/advertising.h"
#include "../communication/signal.h"
#include "../settings/settings.h"
#include "../leds/led.h"
#include "../misc/seacomp_game.h"

void SeacompAmuletMode::start()
{
	Serial.println("Starting seacomp mode");
	scoreboardAnim_ = {};
	scoreboardAnim_.anim_ = Anim::AnimScoreboard;
	scoreboardAnim_.speed_ = 4;

	victoryAnim_ = {};
	victoryAnim_.anim_ = Anim::AnimPrism;

	// Set the initial ambient animation
	auto &config = localSettings_.startupConfig_;
	start_animation(scoreboardAnim_);
	advertising_start(AdvertisementType::Amulet, config.ad,
					  (uint8_t *)&scoreboardAnim_, sizeof(anim_config_t));
}

// returns true if animation was changed
void SeacompAmuletMode::set_animation_from_signal(Signal *s)
{
	if (nullptr != s) {
		anim_config_t pattern;
		VERIFY_STATIC(sizeof(pattern) <= kMaxPayloadLen);
		memcpy(&pattern, s->data_.payload, sizeof(anim_config_t));
		if (pattern.anim_ == Anim::AnimSimon && ((gameState_ & kSeacompGameSimon) != 0))
		{
			s = nullptr;
		}

		if (pattern.anim_ == Anim::AnimPhotoKey && ((gameState_ & kSeacompGamePhotokey) != 0))
		{
			s = nullptr;
		}

		if (pattern.anim_ == Anim::AnimSafe && ((gameState_ & kSeacompGameSafe) != 0))
		{
			s = nullptr;
			;
		}
	}

	if (s != nullptr)
	{
		anim_config_t pattern;
		VERIFY_STATIC(sizeof(pattern) <= kMaxPayloadLen);
		memcpy(&pattern, s->data_.payload, sizeof(anim_config_t));



		if (!matches_current_animation(pattern))
		{
			if (pattern.anim_ == Anim::AnimVictory)
			{
				// change game state
				gameState_ |= pattern.extra0_;
				if (seacomp_game_state_is_victory(gameState_)) {
					resetTimeout_ = millis() + 1000 * 60 * 30;
				}
			}
			if (pattern.anim_ == Anim::AnimInNOut)
			{
				// since this is a new animation pattern we set the timestamp
				timeAtTree_ = millis();
			}
			else
			{
				// we are changing to a pattern that is NOT orbit which means we are away from the tree
				timeAtTree_ = 0;
			}
			start_animation_if_new(pattern);
		}
		else
		{
			if (pattern.anim_ == Anim::AnimInNOut && timeAtTree_ > 0)
			{
				int treeDuration = millis() - timeAtTree_;
				if (treeDuration > 1000 * 60)
				{
					// reset game state. Amulet has been under tree for over 5 minutes
					gameState_ = 0;
					resetTimeout_ = INT32_MAX;
				}
			}
		}
	}
	else
	{
		if (seacomp_game_state_is_victory(gameState_))
		{
			start_animation_if_new(victoryAnim_);
		}
		else
		{
			scoreboardAnim_.extra0_ = gameState_;
			start_animation_if_new(scoreboardAnim_);
		}
	}
}

void SeacompAmuletMode::loop()
{
	if (millis() > resetTimeout_)
	{
		// reset game state. Amulet has been under tree for over 5 minutes
		gameState_ = 0;
		resetTimeout_ = INT32_MAX;
	}

	EVERY_N_MILLISECONDS(globalSettings_.animationUpdateTimer_)
	{
		Signal *signal = signal_get_current_top();
		set_animation_from_signal(signal);
	}
}

/* button press for testing only
void SeacompAmuletMode::buttonPressMode()
{
	static int stateIdx = 0;
	stateIdx = (stateIdx + 1) % 8;
	Serial.printf("New state idx: %d\n", stateIdx);
	Serial.printf("New state int: %d\n", stateIdx);

	if (stateIdx == 0)
	{
		gameState_ = seacomp_game_state(false, false, false);
	}
	if (stateIdx == 1)
	{
		gameState_ = seacomp_game_state(true, false, false);
	}
	if (stateIdx == 2)
	{
		gameState_ = seacomp_game_state(false, true, false);
	}
	if (stateIdx == 3)
	{
		gameState_ = seacomp_game_state(false, false, true);
	}
	if (stateIdx == 4)
	{
		gameState_ = seacomp_game_state(true, true, false);
	}
	if (stateIdx == 5)
	{
		gameState_ = seacomp_game_state(true, false, true);
	}
	if (stateIdx == 6)
	{
		gameState_ = seacomp_game_state(false, true, true);
	}
	if (stateIdx == 7)
	{
		gameState_ = seacomp_game_state(true, true, true);
	}
	Serial.printf("New state int: %d\n", gameState_);
}

*/