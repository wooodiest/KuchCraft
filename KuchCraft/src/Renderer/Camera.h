#pragma once

#include <glm/glm.hpp>

namespace KuchCraft {

	enum class KeyboardMovement
	{
		Forward = 0,Backward,
		Left, Right,
		Up, Down
	};

	class Camera
	{
	public:
		Camera();
		~Camera();

		void OnUpdate();
		void OnKeyboardMovement(KeyboardMovement m, bool sprint = false);
		void OnMouseMovement();

		void SetData(const glm::vec3& position, const glm::vec2& rotation, float speed, float sprintSpeed, float dt) {
			m_Position = position; m_Pitch = rotation.y;
			m_Yaw = rotation.x; m_Speed = speed; m_SprintSpeed = sprintSpeed; m_DeltaTime = dt;
		}

		const glm::mat4& GetProjection()     const { return m_Projection;          }
		const glm::mat4& GetView()           const { return m_View;                }
		const glm::mat4& GetViewProjection() const { return m_Projection * m_View; }
		const glm::mat4 GetSkyboxProjection() const;

		const glm::vec3& GetPosition()       const { return m_Position;                }
		const glm::vec2& GetRotation()       const { return glm::vec2(m_Yaw, m_Pitch); }
		float            GetHorizontalFov()  const { return m_HFov;                    }

	private:
		void CalculateHorizontalFov();
		void UpdateProjection();
		void UpdateView();
		void UpdateFront();

	private:
		glm::mat4 m_Projection{ 1.0f };
		glm::mat4 m_View{ 1.0f };

		glm::vec3 m_Position{ 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Front   { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Up      { 0.0f, 1.0f, 0.0f };

		glm::vec2 m_PrevMousePosition{ 0.0f, 0.0f };

		float m_Pitch = glm::radians(0.0f);
		float m_Yaw   = glm::radians(0.0f);
		float m_Fov   = glm::radians(45.0f);
		float m_HFov  = 0.0f;

		float m_Near        = 0.01f;
		float m_Far         = 1000.0f;
		float m_AspectRatio = 16.0f / 9.0f;

		float m_Speed       = 4.0f;
		float m_SprintSpeed = 6.0f;
		float m_Sensitivity = 0.075f;
		float m_DeltaTime   = 0.0f;

	};


}