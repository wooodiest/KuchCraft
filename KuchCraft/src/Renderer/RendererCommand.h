#pragma once

namespace KuchCraft
{
	class RendererCommand
	{
	private:
		static void EnableBlending();
		static void DisableBlending();

		static void EnableFaceCulling();
		static void EnableFrontFaceCulling();
		static void DisableFaceCulling();

		static void EnableDepthTesting();
		static void EnableLessEqualDepthTesting();
		static void DisableDepthTesting();

	public:
		friend class Renderer;
		friend class TextRenderer;
		friend class Renderer3D;

	private:
		RendererCommand() = default;
	};
}