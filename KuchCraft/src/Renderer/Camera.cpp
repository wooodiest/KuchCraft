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

	void Camera::OnUpdate(float dt)
	{
		float speed = Input::IsKeyPressed(KeyCode::LeftShift) ? m_Speed * 2.0f : m_Speed;

		if (Input::IsKeyPressed(KeyCode::W))
			m_Position += speed * dt * m_Front;
		if (Input::IsKeyPressed(KeyCode::S))
			m_Position -= speed * dt * m_Front;

		if (Input::IsKeyPressed(KeyCode::A))
			m_Position -= glm::normalize(glm::cross(m_Front, m_Up)) * speed * dt;
		if (Input::IsKeyPressed(KeyCode::D))
			m_Position += glm::normalize(glm::cross(m_Front, m_Up)) * speed * dt;

		if (Input::IsKeyPressed(KeyCode::Space))
			m_Position.y += m_Speed * dt;
		if (Input::IsKeyPressed(KeyCode::LeftControl))
			m_Position.y -= m_Speed * dt;


		glm::vec2 position = Input::GetMousePosition();
		glm::vec2 deltaPosition = position - m_PrevMousePosition;
		m_PrevMousePosition = position;

		m_Yaw   += m_Sensitivity * dt * deltaPosition.x;
		m_Pitch -= m_Sensitivity * dt * deltaPosition.y;

		if (m_Pitch > glm::radians(90.0f))
			m_Pitch = glm::radians(90.0f);

		if (m_Pitch < glm::radians(-90.0f))
			m_Pitch = glm::radians(-90.0f);

		UpdateFront();
		UpdateView(); 
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