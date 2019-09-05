#pragma once

#include "animation.h"

class AnimPsyco : public Animation
{
public:
	const static int k_cols = 2;
	const static int k_rows = 4;

	virtual void init()
	{
		LOG_LV1("LED", "Psyco::init");
		fill_solid(leds, RGB_LED_COUNT, CHSV(params_.color1_, 255, 0));
	}

	virtual void step(const int frame, const float deltaTime, const float sourceDistance) override
	{
		// eight colored dots, weaving in and out of sync with each other
		fadeToBlackBy(leds, RGB_LED_COUNT, 40);

		// Update velocity every 4 seconds
		if (frame % (ANIMATION_FRAMERATE * 4))
		{
			m_vx = random16() / 65535.f * .1f;
			m_vy = random16() / 65535.f * .1f;
		}

		// Updated poses wrapping at edges
		m_x += m_vx;
		if (m_x > 3000)
			m_x = 0;
		// if (m_x < 0)
		// 	m_x = k_cols + m_x;
		// else if (m_x > k_cols)
		// 	m_x = m_x - k_cols;

		m_y += m_vy;
		if (m_y > 3000)
			m_y = 0;
		// if (m_y < 0)
		// 	m_y = k_rows + m_y;
		// else if (m_y > k_rows)
		// 	m_y = m_y - k_rows;

		leds[positionToIndex(m_x, m_y)] = CHSV(params_.color1_, 255, 255);
	}

	int positionToIndex(float x, float y)
	{
		int col = round(x) % k_cols;
		int row = round(y) % k_rows;

		return col * k_rows + ((col % 2) == 0 ? row : (k_rows - row - 1));
	}

	float m_ax = 0;
	float m_ay = 0;
	float m_vx = 0;
	float m_vy = 0;
	float m_x = 0;
	float m_y = 0;
};
