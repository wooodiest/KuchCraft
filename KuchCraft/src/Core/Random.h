#pragma once

#include <random>

namespace KuchCraft {

	class Random
	{
	public:
		static void Init()
		{
			s_RandomEngine.seed(std::random_device()());
		}

		static uint32_t UInt()
		{
			return s_Distribution(s_RandomEngine);
		}

		static uint32_t UInt(uint32_t min, uint32_t max)
		{
			return min + (s_Distribution(s_RandomEngine) % (max - min + 1));
		}

		static int32_t Int()
		{
			return static_cast<int>(UInt());
		}

		static int32_t Int(int32_t min, int32_t max)
		{
			uint32_t uintValue = UInt(static_cast<uint32_t>(min), static_cast<uint32_t>(max));
			return static_cast<int32_t>(uintValue);
		}

		static float Float()
		{
			return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
		}

		static float Float(float min, float max)
		{
			return min + (max - min) * Float();
		}

	private:
		static std::mt19937 s_RandomEngine;
		static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
	};

}