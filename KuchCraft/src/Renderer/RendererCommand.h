#pragma once

namespace KuchCraft
{
	class RendererCommand
	{
	public:
		static void ShowPolygons(bool status);

	private:
		static void DrawArrays(uint32_t count, uint32_t offset = 0);
		static void DrawElements(uint32_t count);
		static void DrawStripArraysInstanced(uint32_t count, uint32_t instanceCount, uint32_t offset = 0);

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