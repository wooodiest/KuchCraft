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

		// Set camera uniform buffer
		{
			glm::mat4 viewProjection         = camera ? camera->GetViewProjection()         : glm::mat4(1.0f);
			glm::mat4 absoluteViewProjection = camera ? camera->GetAbsoluteViewProjection() : glm::mat4(1.0f);
			glm::mat4 orthoProjection        = camera ? camera->GetOrthoProjection()        : glm::ortho(0.0f, (float)Application::Get().GetWindow().GetWidth(), 0.0f, (float)Application::Get().GetWindow().GetHeight());

			UniformBufferCameraData buffer{
				viewProjection,
				absoluteViewProjection,
				orthoProjection
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
	}

	void Renderer::LoadRendererInfo()
	{
		KC_PROFILE_FUNCTION();
	}

	void Renderer::OnViewportSizeChanged(uint32_t width, uint32_t height)
	{
		Renderer3D::OnViewportSizeChanged(width, height);
	}

}