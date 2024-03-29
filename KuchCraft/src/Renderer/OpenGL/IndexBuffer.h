#pragma once

namespace KuchCraft {

	class IndexBuffer
	{
	public:
		IndexBuffer();
		IndexBuffer(uint32_t* data, uint32_t count);
		~IndexBuffer();

		void Create(uint32_t* data, uint32_t count);

		void Bind()   const;
		void UnBind() const;

		uint32_t GetCount() const { return m_Count; };

	private:
		uint32_t m_RendererID = 0;
		uint32_t m_Count      = 0;

	};

}