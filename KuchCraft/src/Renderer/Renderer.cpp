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

	RendererData Renderer::s_RendererData;

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

		s_RendererData.CurrentCamera = camera ? camera : &(*s_RendererData.SpareCamera);

		// Set camera uniform buffer
		{
			UniformBufferCameraData buffer{
				s_RendererData.CurrentCamera->GetViewProjection(),
				s_RendererData.CurrentCamera->GetAbsoluteViewProjection(),
				s_RendererData.CurrentCamera->GetOrthoProjection()
			};
			s_RendererData.WorldDataUniformBuffer->SetData(&buffer, sizeof(buffer));
		}

		Renderer3D::Render();

		DefaultFrameBuffer::BindAndClear();
		Renderer2D::RenderFullScreenQuad(Renderer3D::GetRendererdColorAttachmentRendererID());

		TextRenderer::Render();
	}

	void Renderer::PrepareRenderer()
	{
		KC_PROFILE_FUNCTION();

		if (opengl_logs)
			RendererCommand::EnableLogMessages();
		
		s_RendererData.WorldDataUniformBuffer = UniformBuffer::Create(sizeof(UniformBufferCameraData), 0);

		s_RendererData.SpareCamera = CreateRef<Camera>();
	}

	void Renderer::LoadRendererInfo()
	{
		KC_PROFILE_FUNCTION();
	}

	void Renderer::OnViewportSizeChanged(uint32_t width, uint32_t height)
	{
		Renderer3D::OnViewportSizeChanged(width, height);

		s_RendererData.SpareCamera->UpdateProjection(width, height);
	}

}