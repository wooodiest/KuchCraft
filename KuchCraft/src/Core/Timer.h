#pragma once

#include <chrono>

namespace KuchCraft {

	class Timer
	{
	public:
		Timer() {}
		~Timer() {}

		void Start()
		{
			m_Start = std::chrono::high_resolution_clock::now();
		}

		void Finish()
		{
			m_Millis = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_Start).count() * 0.001f * 0.001f;
		}

		float GetElapsedMillis() const { return m_Millis; }

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;

		float m_Millis = 0.0f;
	};

}