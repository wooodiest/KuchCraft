#pragma once

#include "Core/Events/Event.h"
#include "Core/Events/ApplicationEvent.h"

namespace KuchCraft {

	class Camera
	{
	public:
		Camera();
		~Camera();

		void OnUpdate(const glm::vec3& position, const glm::vec2& rotation);
		void OnEvent(Event& event);

		bool OnWindowResized(WindowResizeEvent& e);

		void SetFarPlan(float farplan);

		const glm::mat4& GetProjection()             const { return m_Projection;             }
		const glm::mat4& GetOrthoProjection()        const { return m_OrthoProjection;        }
		const glm::mat4& GetView()                   const { return m_View;                   }
		const glm::mat4& GetViewProjection()         const { return m_ViewProjection;         }
		const glm::mat4& GetAbsoluteViewProjection() const { return m_AbsoluteViewProjection; }

		const glm::vec3& GetFront() const { return m_Front; }
		const glm::vec3& GetUp()    const { return m_Up;    }

		const glm::vec3& GetAbsoluteFront() const { return m_AbsoluteFront; }
		const glm::vec3& GetAbsoluteRight() const { return m_AbsoluteRight; }

		const glm::vec3& GetPosition() const { return m_Position; }

		void UpdateProjection(uint32_t width, uint32_t height);

	private:
		void UpdateView(const glm::vec3& position);
		void UpdateFront(const glm::vec2& rotation);

	private:
		glm::mat4 m_Projection             { 1.0f };
		glm::mat4 m_View                   { 1.0f };
		glm::mat4 m_ViewProjection         { 1.0f };
		glm::mat4 m_OrthoProjection        { 1.0f };
		glm::mat4 m_AbsoluteViewProjection { 1.0f };

		glm::vec3 m_Up { 0.0f, 1.0f, 0.0f };
		glm::vec3 m_Front          { 0.0f };
		glm::vec3 m_AbsoluteFront  { 0.0f };
		glm::vec3 m_AbsoluteRight  { 0.0f };
		glm::vec3 m_Position       { 0.0f };

		float m_Fov         = glm::radians(45.0f);
		float m_Near        = 0.01f;
		float m_Far         = 1000.0f;
		float m_AspectRatio = 16.0f / 9.0f;

	};


}