#include "Camera.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Core/Input.h"

namespace KuchCraft {

	Camera::Camera()
	{
		m_PrevMousePosition = Input::GetMousePosition();

		UpdateProjection();
		UpdateFront();
		UpdateView();
	}

	Camera::~Camera()
	{
	}

	void Camera::OnUpdate()
	{
		UpdateFront();
		UpdateView(); 
	}

	void Camera::OnKeyboardMovement(KeyboardMovement m, bool sprint)
	{
		float speed = sprint ? m_SprintSpeed : m_Speed;
		switch (m)
		{
			case KuchCraft::KeyboardMovement::Forward:  m_Position   += speed * m_DeltaTime * glm::normalize(glm::cross(m_Up, glm::cross(m_Front, m_Up))); break;
			case KuchCraft::KeyboardMovement::Backward: m_Position   -= speed * m_DeltaTime * glm::normalize(glm::cross(m_Up, glm::cross(m_Front, m_Up))); break;
			case KuchCraft::KeyboardMovement::Left:     m_Position   -= speed * m_DeltaTime * glm::normalize(glm::cross(m_Front, m_Up));                   break;
			case KuchCraft::KeyboardMovement::Right:    m_Position   += speed * m_DeltaTime * glm::normalize(glm::cross(m_Front, m_Up));                   break;
			case KuchCraft::KeyboardMovement::Up:       m_Position.y += speed * m_DeltaTime;                                                               break;
			case KuchCraft::KeyboardMovement::Down:     m_Position.y -= speed * m_DeltaTime;                                                               break;
			default: break;
		}
	}

	void Camera::OnMouseMovement()
	{
		auto position       = Input::GetMousePosition();
		auto deltaPosition  = position - m_PrevMousePosition;
		m_PrevMousePosition = position;

		m_Yaw   += (float)(m_Sensitivity * m_DeltaTime * deltaPosition.x);
		m_Pitch -= (float)(m_Sensitivity * m_DeltaTime * deltaPosition.y);

		if (m_Pitch > glm::radians(89.9f))
			m_Pitch = glm::radians(89.9f);

		if (m_Pitch < glm::radians(-89.9f))
			m_Pitch = glm::radians(-89.9f);
	}

	const glm::mat4 Camera::GetAbsoluteViewProjection() const
	{
		return m_Projection * glm::mat4(glm::mat3(glm::lookAt(glm::vec3(0.0f), m_Front, m_Up)));
	}

	void Camera::UpdateProjection()
	{
		m_Projection = glm::perspective(m_Fov, m_AspectRatio, m_Near, m_Far);
	}

	void Camera::UpdateView()
	{
		m_View = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
	}

	void Camera::UpdateFront()
	{
		glm::vec3 front;
		front.x = glm::cos(m_Yaw) * glm::cos(m_Pitch);
		front.y = glm::sin(m_Pitch);
		front.z = glm::sin(m_Yaw) * glm::cos(m_Pitch);
		m_Front = glm::normalize(front);
	}

}