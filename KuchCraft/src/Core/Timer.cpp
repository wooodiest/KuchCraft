#include "Timer.h"

namespace KuchCraft {

	float Timer::s_CurrentTimeElapsed = 0.0f;

	constexpr float timer_update_time = 0.1f;

	void Timer::OnUpdate(float dt)
	{
		s_CurrentTimeElapsed += dt;
		if (s_CurrentTimeElapsed >= timer_update_time)
			s_CurrentTimeElapsed = 0.0f;
	}

	void Timer::Begin()
	{
		m_Start = std::chrono::high_resolution_clock::now();
	}

	void Timer::End()
	{
		if (s_CurrentTimeElapsed == 0.0f)
			m_LastTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_Start).count() * 0.001f * 0.001f * 0.001f;
		
	}
}