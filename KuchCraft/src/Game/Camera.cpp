#include "kcpch.h"
#include "Game/Camera.h"

#include "Core/Input.h"
#include "Core/Application.h"
#include "Core/Core.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace KuchCraft {

	Camera::Camera()
	{
		auto& [width, height] = Application::Get().GetWindow().GetWindowSize();
		UpdateProjection(width, height);
	}

	Camera::~Camera()
	{
	}

	void Camera::OnUpdate(const glm::vec3& position, const glm::vec2& rotation)
	{
		KC_PROFILE_FUNCTION();

		UpdateFront(rotation);
		UpdateView(position);
	}

	void Camera::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowResizeEvent>(KC_BIND_EVENT_FN(Camera::OnWindowResized));
	}

	void Camera::SetFarPlan(float farplan)
	{
		m_Far = farplan;
		auto& [width, height] = Application::Get().GetWindow().GetWindowSize();
		UpdateProjection(width, height);
	}

	bool Camera::OnWindowResized(WindowResizeEvent& e)
	{
		UpdateProjection(e.GetWidth(), e.GetHeight());

		return false;
	}

	void Camera::UpdateProjection(uint32_t width, uint32_t height)
	{
		KC_PROFILE_FUNCTION();

		m_Projection      = glm::perspective(m_Fov, m_AspectRatio, m_Near, m_Far);
		m_OrthoProjection = glm::ortho(0.0f, (float)width, 0.0f, (float)height);
	}

	void Camera::UpdateView(const glm::vec3& position)
	{
		KC_PROFILE_FUNCTION();

		m_View           = glm::lookAt(position, position + m_Front, m_Up);
		m_ViewProjection = m_Projection * m_View;
	}

	void Camera::UpdateFront(const glm::vec2& rotation)
	{
		KC_PROFILE_FUNCTION();

		glm::vec3 front;
		front.x = glm::cos(rotation.x) * glm::cos(rotation.y);
		front.y = glm::sin(rotation.y);
		front.z = glm::sin(rotation.x) * glm::cos(rotation.y);
		m_Front = glm::normalize(front);

		m_AbsoluteViewProjection = m_Projection * glm::lookAt(glm::vec3(0.0f), m_Front, m_Up);

		m_AbsoluteFront = glm::normalize(glm::cross(m_Up, glm::cross(m_Front, m_Up)));
		m_AbsoluteRight = glm::normalize(glm::cross(m_Front, m_Up));
	}

}