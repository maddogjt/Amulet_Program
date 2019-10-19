#pragma once

#include <Arduino.h>

constexpr uint8_t kSeacompGameSimon = 1 << 0;
constexpr uint8_t kSeacompGameSafe = 1 << 1;
constexpr uint8_t kSeacompGamePhotokey = 1 << 2;

inline bool seacomp_game_state_is_victory(uint8_t gameState)
{
	return gameState >= (kSeacompGameSimon | kSeacompGameSafe | kSeacompGamePhotokey);
}

inline uint8_t seacomp_game_state(bool solvedSimon, bool solvedSafe, bool solvedPhotokey)
{
	uint8_t ret = 0;
	if (solvedSimon)
	{
		ret |= kSeacompGameSimon;
	}
	if (solvedSafe)
	{
		ret |= kSeacompGameSafe;
	}
	if (solvedPhotokey)
	{
		ret |= kSeacompGamePhotokey;
	}
	return ret;
}
