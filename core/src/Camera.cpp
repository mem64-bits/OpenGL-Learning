#include <Camera.h>
#include <glm/gtc/matrix_transform.hpp>


namespace core
{
    void Camera::updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        front.y = sin(glm::radians(m_Pitch));
        front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        m_Front = glm::normalize(front);
        m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));
    }

    Camera::Camera(const CameraOptions& options)
        : m_Pos(options.Pos), m_WorldUp(options.WorldUp), m_Yaw(options.Yaw), m_Pitch(options.Pitch),
          m_Speed(options.Speed), m_FOV(options.FOV), m_MouseSens(options.MouseSens)
    {
        updateCameraVectors();
    }

    Camera::Camera(const glm::vec3 pos, const glm::vec3 worldUp, const float yaw, const float pitch, const float speed,
                   const float fov, const float mouseSens)
        : m_Pos(pos), m_WorldUp(worldUp), m_Yaw(yaw), m_Pitch(pitch),
          m_Speed(speed), m_FOV(fov), m_MouseSens(mouseSens)
    {
        updateCameraVectors();
    }

    glm::mat4 Camera::getViewMatrix() const
    {
        return glm::lookAt(m_Pos, m_Pos + m_Front, m_Up);
    }

    glm::mat4 Camera::getProjectionMatrix(const int width, const int height, const float zNear, const float zFar) const
    {
        return glm::perspective(glm::radians(m_FOV), static_cast<float>( width ) / height, zNear, zFar);
    }

    void Camera::processKeyboard(const CameraMovement direction, const float deltaTime)
    {
        const float velocity = m_Speed * deltaTime;
        if(direction == FORWARD) m_Pos += m_Front * velocity;
        if(direction == BACKWARD) m_Pos -= m_Front * velocity;
        if(direction == LEFT) m_Pos -= m_Right * velocity;
        if(direction == RIGHT) m_Pos += m_Right * velocity;
    }

    void Camera::ProcessMouseMovement(float xOffset, float yOffset, const bool constrainPitch)
    {
        xOffset *= m_MouseSens;
        yOffset *= m_MouseSens;
        m_Yaw += xOffset;
        m_Pitch += yOffset;

        if(constrainPitch)
        {
            if(m_Pitch > 89.0f) m_Pitch = 89.0f;
            if(m_Pitch < - 89.0f) m_Pitch = - 89.0f;
        }
        updateCameraVectors();
    }

    void Camera::ProcessMouseScroll(const float yOffset)
    {
        m_FOV -= yOffset;
        if(m_FOV < 1.0f) m_FOV = 1.0f;
        if(m_FOV > 45.0f) m_FOV = 45.0f;
    }

    // Getters & Setters Implementation
    glm::vec3 Camera::getCamPos() const { return m_Pos; }
    void Camera::setCamPos(const glm::vec3& camPos) { m_Pos = camPos; }

    float Camera::getYaw() const { return m_Yaw; }
    void Camera::setYaw(const float yaw) { m_Yaw = yaw; }

    float Camera::getPitch() const { return m_Pitch; }
    void Camera::setPitch(const float pitch) { m_Pitch = pitch; }

    float Camera::getSpeed() const { return m_Speed; }
    void Camera::setSpeed(const float speed) { m_Speed = speed; }

    float Camera::getFOV() const { return m_FOV; }
    void Camera::setFOV(const float fov) { m_FOV = fov; }

    float Camera::getMouseSens() const { return m_MouseSens; }
    void Camera::setMouseSens(const float sens) { m_MouseSens = sens; }

    glm::vec3 Camera::getWorldUp() const { return m_WorldUp; }
    void Camera::setWorldUp(const glm::vec3& worldUp) { m_WorldUp = worldUp; }
}
