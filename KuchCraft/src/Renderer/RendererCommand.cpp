#include "kcpch.h"
#include "Renderer/RendererCommand.h"

#include <glad/glad.h>

namespace KuchCraft {

	void RendererCommand::ShowPolygons(bool status)
	{
		if (status)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void RendererCommand::DrawArrays(uint32_t count, uint32_t offset)
	{
		glDrawArrays(GL_TRIANGLES, offset, count);
	}

	void RendererCommand::DrawElements(uint32_t count)
	{
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

	void RendererCommand::DrawStripArraysInstanced(uint32_t count, uint32_t instanceCount, uint32_t offset)
	{
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, offset, count, instanceCount);
	}

	void RendererCommand::EnableBlending()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void RendererCommand::DisableBlending()
	{
		glDisable(GL_BLEND);
	}

	void RendererCommand::EnableFaceCulling()
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	void RendererCommand::EnableFrontFaceCulling()
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
	}

	void RendererCommand::DisableFaceCulling()
	{
		glDisable(GL_CULL_FACE);
	}

	void RendererCommand::EnableDepthTesting()
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
	}

	void RendererCommand::EnableLessEqualDepthTesting()
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	}

	void RendererCommand::DisableDepthTesting()
	{
		glDisable(GL_DEPTH_TEST);
	}

	void RendererCommand::EnableDepthMask()
	{
		glDepthMask(GL_TRUE);
	}

	void RendererCommand::DisableDepthMask()
	{
		glDepthMask(GL_FALSE);
	}

	static void OpenGLLogMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			KC_ERROR("[OpenGL] : {0}", (char*)message);
			break;

		case GL_DEBUG_SEVERITY_MEDIUM:
		case GL_DEBUG_SEVERITY_LOW:
			KC_WARN("[OpenGL] : {0}", (char*)message);
			break;

		case GL_DEBUG_SEVERITY_NOTIFICATION:
			KC_INFO("[OpenGL] : {0}", (char*)message);
			break;
		}
	}

	void RendererCommand::EnableLogMessages()
	{
		glDebugMessageCallback(OpenGLLogMessage, nullptr);
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}
}



