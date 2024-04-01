#pragma once

namespace KuchCraft
{
	class RendererCommand
	{
	public:
		static void ShowPolygons(bool status);

	private:
		static void EnableBlending();
		static void DisableBlending();

		static void EnableFaceCulling();
		static void EnableFrontFaceCulling();
		static void DisableFaceCulling();

		static void EnableDepthTesting();
		static void EnableLessEqualDepthTesting();
		static void DisableDepthTesting();

		static void EnableLogMessages();

	public:
		friend class Renderer;
		friend class TextRenderer;
		friend class Renderer3D;
		friend class Renderer2D;

	private:
		RendererCommand() = default;
	};
}