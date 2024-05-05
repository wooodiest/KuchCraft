#pragma once

namespace KuchCraft {

	inline constexpr glm::vec4 default_clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };

	enum class FrameBufferTextureFormat
	{
		None = 0,

		// Color
		RGBA8,
		RED_INTEGER,
		RGBA16F,

		// Depth/stencil
		DEPTH24STENCIL8
	};

	struct FrameBufferTextureSpecification
	{
		FrameBufferTextureSpecification() = default;
		FrameBufferTextureSpecification(FrameBufferTextureFormat format)
			: TextureFormat(format) {}

		FrameBufferTextureFormat TextureFormat = FrameBufferTextureFormat::RGBA8;
	};

	struct FrameBufferAttachmentSpecification
	{
		FrameBufferAttachmentSpecification() = default;
		FrameBufferAttachmentSpecification(std::initializer_list<FrameBufferTextureSpecification> attachments)
			: Attachments(attachments) {}

		std::vector<FrameBufferTextureSpecification> Attachments;
	};

	struct FrameBufferSpecification
	{
		uint32_t Width = 0, Height = 0;
		FrameBufferAttachmentSpecification Attachments;
		uint32_t Samples = 1;
	};

	class FrameBuffer
	{
	public:
		FrameBuffer();
		~FrameBuffer();

		void Create(const FrameBufferSpecification& specification);

		void Invalidate();

		void Bind() const;
		void Unbind() const;

		void BindAndClear() const;

		void Resize(uint32_t width, uint32_t height);
		int  ReadPixel(uint32_t attachmentIndex, int x, int y) const;

		void ClearColorAttachment(uint32_t attachmentIndex, const glm::vec4& color = default_clear_color) const;
		void ClearColorAttachments(const glm::vec4 & color = default_clear_color) const;
		void ClearDepthAttachment() const;
		void Clear(const glm::vec4& color = default_clear_color) const;

		uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const { return m_ColorAttachments[index]; }
		const FrameBufferSpecification& GetSpecification()        const { return m_Specification;           }

	private:
		uint32_t m_RendererID = 0;

		FrameBufferSpecification m_Specification;
		std::vector<FrameBufferTextureSpecification> m_ColorAttachmentSpecifications;
		FrameBufferTextureSpecification m_DepthAttachmentSpecification = FrameBufferTextureFormat::None;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};

	class DefaultFrameBuffer
	{
	public:
		static void Bind();
		static void Clear();

		static void BindAndClear();
	};

}