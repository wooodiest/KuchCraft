#include "kcpch.h"
#include "Renderer/Renderer.h"

#include "Renderer/Text/TextRenderer.h"
#include "Renderer/Renderer3D/Renderer3D.h"
#include "Renderer/Renderer2D/Renderer2D.h"
#include "Renderer/AssetManager.h"
#include "Renderer/RendererCommand.h"

#include "Renderer/Data/Shader.h"

#include "Core/Application.h"
#include "Core/Utils.h"

#include "World/World.h"

namespace KuchCraft {

	RendererData Renderer::s_Data;
	RendererInfo Renderer::s_Info;

	RendererStatistics Renderer::s_Stats;

	void Renderer::Init()
	{
		KC_PROFILE_FUNCTION();

		LoadRendererInfo();
		PrepareRenderer();
		
		AssetManager::Init();
		TextRenderer::Init();
		Renderer2D  ::Init();
		Renderer3D  ::Init();
	}

	void Renderer::ShutDown()
	{
		KC_PROFILE_FUNCTION();

		Renderer3D  ::ShutDown();
		Renderer2D  ::ShutDown();
		TextRenderer::ShutDown();
		AssetManager::ShutDown();
	}

	void Renderer::BeginFrame()
	{
		KC_PROFILE_FUNCTION();

		Renderer3D  ::Clear();
		Renderer2D  ::Clear();
		TextRenderer::Clear();
	}

	void Renderer::EndFrame(Camera* camera)
	{
		KC_PROFILE_FUNCTION();

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

		DefaultFrameBuffer::BindAndClear();
		Renderer2D::RenderFullScreenQuad(Renderer3D::GetRendererdColorAttachmentRendererID());

		TextRenderer::Render();
	}

	const std::string& Renderer::GetDubugText()
	{
		s_Stats.DebugText =
			"\nRenderer:"
			"\n    Draw cals: " + std::to_string(s_Stats.DrawCalls) +
			"\n    Quads: "     + std::to_string(s_Stats.Quads);

		return s_Stats.DebugText;
	}

	void Renderer::PrepareRenderer()
	{
		KC_PROFILE_FUNCTION();

		if (s_Info.OpenGlLogs)
			RendererCommand::EnableLogMessages();
		
		s_Data.WorldDataUniformBuffer.Create(sizeof(UniformBufferCameraData), 0);
		s_Data.SpareCamera = std::make_shared<Camera>();
	}

	void Renderer::LoadRendererInfo()
	{
		KC_PROFILE_FUNCTION();

		s_Info = RendererInfo();
	}

	void Renderer::OnViewportSizeChanged(uint32_t width, uint32_t height)
	{
		Renderer3D::OnViewportSizeChanged(width, height);

		s_Data.SpareCamera->UpdateProjection(width, height);
	}

}