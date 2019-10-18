#pragma once

enum class SimonColor
{
	None,
	Red,
	Green,
	Blue,
	Yellow
};

inline CRGB simon_color_to_rgb(SimonColor c)
{
	switch (c)
	{
	case SimonColor::Red:
		return CRGB::Red;
	case SimonColor::Green:
		return CRGB::Green;
	case SimonColor::Blue:
		return CRGB::Blue;
	case SimonColor::Yellow:
		return CRGB::Yellow;
	case SimonColor::None:
	default:
		return CRGB::Black;
	}
}

struct SimonStep
{
	SimonColor color;
	int32_t duration;
};

constexpr size_t kMaxSimonSteps = 32;

inline size_t generate_simon_sequence(uint8_t seed, uint8_t speed, uint8_t count, SimonStep out[kMaxSimonSteps])
{
	srand(seed);

	int steps = min(count, kMaxSimonSteps / 2);
	for (int i = 0; i < steps; i++)
	{
		out[i * 2].color = (SimonColor)(rand() % 4 + 1);
		out[i * 2].duration = speed * 4;

		out[i * 2 + 1].color = SimonColor::None;
		out[i * 2 + 1].duration = 50;
	}

	out[steps * 2 - 1].duration = 1500;

	return steps * 2;
}
