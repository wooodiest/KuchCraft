#pragma once

#include <glm/glm.hpp>

namespace KuchCraft {

	class Camera
	{
	public:
		Camera();
		~Camera();

		void OnUpdate(float dt);

		void UpdateProjection();
		void UpdateView();
		void UpdateFront();

		const glm::mat4& GetProjection()     const { return m_Projection;          }
		const glm::mat4& GetView()           const { return m_View;                }
		const glm::mat4& GetViewProjection() const { return m_Projection * m_View; }

		const glm::vec3& GetPosition()       const { return m_Position; }

	private:
		glm::mat4 m_Projection{ 1.0f };
		glm::mat4 m_View{ 1.0f };

		glm::vec3 m_Position{ -3.0f, 0.0f, -5.0f };
		glm::vec3 m_Front   { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Up      { 0.0f, 1.0f, 0.0f };

		glm::vec2 m_PrevMousePosition{ 0.0f, 0.0f };

		float m_Pitch = glm::radians(0.0f);
		float m_Yaw   = glm::radians(-90.0f);
		float m_Fov   = glm::radians(45.0f);

		float m_Near        = 0.01f;
		float m_Far         = 1000.0f;
		float m_AspectRatio = 16.0f / 9.0f;

		float m_Speed       = 5.5f;
		float m_Sensitivity = 0.075f;

	};


}