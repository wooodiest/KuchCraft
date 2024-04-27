#pragma once

namespace KuchCraft {

	class IndexBuffer
	{
	public:
		IndexBuffer();
		~IndexBuffer();

		void Create(uint32_t count, uint32_t* data = nullptr, bool staticDraw = false);
		void SetData(uint32_t* data, uint32_t count);

		void Bind()   const;
		void Unbind() const;

		uint32_t GetCount() const { return m_Count; };

	private:
		uint32_t m_RendererID = 0;
		uint32_t m_Count      = 0;

	};

}