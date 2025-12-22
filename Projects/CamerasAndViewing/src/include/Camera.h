#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct CameraOptions
{
    glm::vec3 Pos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    float Yaw = -90.0f;
    float Pitch = 0.0f;
    float Speed = 2.5f;
    float FOV = 45.0f;
    float MouseSens = 0.1f;
};

enum CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera
{
private:
    glm::vec3 m_Pos;
    glm::vec3 m_Up;
    glm::vec3 m_WorldUp;
    glm::vec3 m_Front;
    glm::vec3 m_Right;

    // The main variables of Camera control (Roll isn't needed in simple FPS)
    float m_Yaw;
    float m_Pitch;

    float m_Speed;
    float m_FOV;
    float m_MouseSens;


    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        front.y = sin(glm::radians(m_Pitch));
        front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

        m_Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
        // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));
    }

public:
    explicit Camera(const CameraOptions &options = {}) // NOLINT(*-pro-type-member-init)
        : m_Pos(options.Pos),
          m_WorldUp(options.WorldUp),
          m_Yaw(options.Yaw),
          m_Pitch(options.Pitch),
          m_Speed(options.Speed),
          m_FOV(options.FOV),
          m_MouseSens(options.MouseSens)
    {
        updateCameraVectors();
    }


    Camera(const glm::vec3 pos, const glm::vec3 worldUp, const float yaw, const float pitch, const float speed, // NOLINT(*-pro-type-member-init)
           const float fov, const float mouseSens)
        : m_Pos(pos),
          m_WorldUp(worldUp),
          m_Yaw(yaw),
          m_Pitch(pitch),
          m_Speed(speed),
          m_FOV(fov),
          m_MouseSens(mouseSens) {}


    [[nodiscard]] glm::mat4 getViewMatrix() const
    {
        return glm::lookAt(m_Pos, m_Pos + m_Front, m_Up);
    }

    [[nodiscard]] glm::mat4 getProjectionMatrix(const float width, const float height, float zNear = 0.1f,
                                                float zFar = 100.f) const
    {
        return glm::perspective(glm::radians(m_FOV), (width / height), 0.1f, 100.0f);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void processKeyboard(const CameraMovement direction, const float deltaTime)
    {
        const float velocity = m_Speed * deltaTime;
        if (direction == FORWARD)
            m_Pos += m_Front * velocity;
        if (direction == BACKWARD)
            m_Pos -= m_Front * velocity;
        if (direction == LEFT)
            m_Pos -= m_Right * velocity;
        if (direction == RIGHT)
            m_Pos += m_Right * velocity;
        // Fixes camera on ground to create true fps
        // m_Pos.y = 0;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xOffset, float yOffset, const bool constrainPitch = true)
    {
        xOffset *= m_MouseSens;
        yOffset *= m_MouseSens;

        m_Yaw += xOffset;
        m_Pitch += yOffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (m_Pitch > 89.0f)
                m_Pitch = 89.0f;
            if (m_Pitch < -89.0f)
                m_Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(const float yOffset)
    {
        m_FOV -= yOffset;
        if (m_FOV < 1.0f)
            m_FOV = 1.0f;
        if (m_FOV > 45.0f)
            m_FOV = 45.0f;
    }

    // Getters & Setters
    [[nodiscard]] glm::vec3 getCamPos() const { return m_Pos; }
    void setCamPos(const glm::vec3 &camPos) { m_Pos = camPos; }

    [[nodiscard]] glm::vec3 getWorldUp() const { return m_WorldUp; }
    void setWorldUp(const glm::vec3 &worldUp) { m_WorldUp = worldUp; }

    [[nodiscard]] float getYaw() const { return m_Yaw; }
    void setYaw(const float yaw) { m_Yaw = yaw; }

    [[nodiscard]] float getPitch() const { return m_Pitch; }
    void setPitch(const float pitch) { m_Pitch = pitch; }
    [[nodiscard]] float getSpeed() const { return m_Speed; }
    void setSpeed(const float speed) { m_Speed = speed; }

    [[nodiscard]] float getFOV() const { return m_FOV; }
    void setFOV(const float fov) { m_FOV = fov; }

    [[nodiscard]] float getMouseSens() const { return m_MouseSens; }
    void setMouseSens(const float sens) { m_MouseSens = sens; }
};
