#pragma once

#include <chrono>

namespace KuchCraft {

	class Timer
	{
	public:
		Timer()
		{
			Begin();
		}

		static void OnUpdate(float dt);

		void Begin();
		void End();

		float Elapsed()
		{
			return m_LastTime;
		}

		float ElapsedMillis()
		{
			return m_LastTime * 1000.0f;
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
		float m_LastTime = 0.0f;

		static float s_CurrentTimeElapsed;
	};
}