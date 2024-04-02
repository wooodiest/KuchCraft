#pragma once

namespace KuchCraft {

	class UniformBuffer
	{
	public:
		UniformBuffer();
		~UniformBuffer();

		void Create(uint32_t size, uint32_t binding);

		void SetData(const void* data, uint32_t size, uint32_t offset = 0);

	private:
		uint32_t m_RendererID = 0;

	};

}