#include "kcpch.h"
#include "Renderer/RendererCommand.h"

#include <glad/glad.h>

namespace KuchCraft {

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
}


