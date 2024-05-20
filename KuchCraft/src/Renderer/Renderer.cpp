#include "kcpch.h"
#include "Renderer/Renderer.h"

#include "Renderer/Renderer3D/Renderer3D.h"
#include "Renderer/Renderer2D/Renderer2D.h"
#include "Renderer/AssetManager.h"
#include "Renderer/RendererCommand.h"

#include "Renderer/Data/Shader.h"

#include "Core/Application.h"

#include "World/World.h"

namespace KuchCraft {

	RendererData Renderer::s_Data;
	RendererInfo Renderer::s_Info;

	RendererStatistics Renderer::s_Stats;

	void Renderer::Init()
	{
		LoadRendererInfo();
		PrepareRenderer();
		
		AssetManager::Init();
		Renderer2D  ::Init();
		Renderer3D  ::Init();
	}

	void Renderer::ShutDown()
	{
		Renderer3D  ::ShutDown();
		Renderer2D  ::ShutDown();
		AssetManager::ShutDown();
	}

	void Renderer::BeginFrame()
	{
		Renderer3D  ::Clear();
		Renderer2D  ::Clear();
	}

	void Renderer::EndFrame(Camera* camera)
	{
		s_Stats.RenderTimer.Start();
		s_Stats.Clear();

		s_Data.CurrentCamera = camera ? camera : &(*s_Data.SpareCamera);

		// Set camera uniform buffer
		{
			UniformBufferCameraData buffer{
				s_Data.CurrentCamera->GetViewProjection(),
				s_Data.CurrentCamera->GetAbsoluteViewProjection(),
				s_Data.CurrentCamera->GetOrthoProjection()
			};
			s_Data.WorldDataUniformBuffer.SetData(&buffer, sizeof(buffer));
		}

		Renderer3D::Render();
		Renderer2D::Render();

		DefaultFrameBuffer::BindAndClear();
		// todo: combine it into one shader width draw list
		Renderer2D::RenderFullScreenQuad(Renderer3D::GetRendererdColorAttachmentRendererID());
		Renderer2D::RenderFullScreenQuad(Renderer2D::GetRendererdColorAttachmentRendererID());

		s_Stats.RenderTimer.Finish();
	}

	const std::string& Renderer::GetDubugText()
	{
		s_Stats.DebugText =
			"\nRenderer:"
			"\n    Draw cals: " + std::to_string(s_Stats.DrawCalls) +
			"\n    Quads: "     + std::to_string(s_Stats.Quads) +
			"\n    Render time: "          + Utils::FloatToString(s_Stats.RenderTimer.          GetElapsedMillis(), 3) + "ms" +
			"\n      - 2D:              "  + Utils::FloatToString(s_Stats.Renderer2DTimer.      GetElapsedMillis(), 3) + "ms" +
			"\n      - 3D:              "  + Utils::FloatToString(s_Stats.Renderer3DTimer.      GetElapsedMillis(), 3) + "ms" +
			"\n      -> chunks:         "      + Utils::FloatToString(s_Stats.Renderer3DChunkTimer. GetElapsedMillis(), 3) + "ms" + 
			"\n      -> skybox:         "      + Utils::FloatToString(s_Stats.Renderer3DSkyboxTimer.GetElapsedMillis(), 3) + "ms" +
			"\n      -> transparent: "         + Utils::FloatToString(s_Stats.Renderer3DTransparentQuadsTimer.GetElapsedMillis(), 3) + "ms" +
			"\n      -> quads:           "     + Utils::FloatToString(s_Stats.Renderer3DQuadsTimer. GetElapsedMillis(), 3) + "ms";

		return s_Stats.DebugText;
	}

	void Renderer::PrepareRenderer()
	{
		if (s_Info.OpenGlLogs)
			RendererCommand::EnableLogMessages();
		
		s_Data.WorldDataUniformBuffer.Create(sizeof(UniformBufferCameraData), 0);
		s_Data.SpareCamera = std::make_shared<Camera>();
	}

	void Renderer::LoadRendererInfo()
	{
		s_Info = RendererInfo();
	}

	void Renderer::OnViewportSizeChanged(uint32_t width, uint32_t height)
	{
		Renderer3D::OnViewportSizeChanged(width, height);

		s_Data.SpareCamera->UpdateProjection(width, height);
	}

}