#pragma once

#include <glm/glm.hpp>

namespace core
{
    struct CameraOptions
    {
        glm::vec3 Pos = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        float Yaw = - 90.0f;
        float Pitch = 0.0f;
        float Speed = 2.5f;
        float FOV = 45.0f;
        float MouseSens = 0.1f;
    };

    enum CameraMovement { FORWARD, BACKWARD, LEFT, RIGHT };

    class Camera
    {
    private:
        glm::vec3 m_Pos, m_Up, m_WorldUp, m_Front, m_Right;
        float m_Yaw, m_Pitch, m_Speed, m_FOV, m_MouseSens;

        void updateCameraVectors();

    public:
        // Default arguments MUST stay in the header
        explicit Camera(const CameraOptions& options = {});

        Camera(glm::vec3 pos, glm::vec3 worldUp, float yaw, float pitch, float speed, float fov, float mouseSens);

        [[nodiscard]] glm::mat4 getViewMatrix() const;

        [[nodiscard]] glm::mat4 getProjectionMatrix(int width, int height,
                                                    float zNear = 0.1f, float zFar = 100.f) const;

        void processKeyboard(CameraMovement direction, float deltaTime);

        void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);

        void ProcessMouseScroll(float yOffset);

        // Getters & Setters (Declarations)
        [[nodiscard]] glm::vec3 getCamPos() const;

        void setCamPos(const glm::vec3& camPos);

        [[nodiscard]] float getYaw() const;

        void setYaw(float yaw);

        [[nodiscard]] float getPitch() const;

        void setPitch(float pitch);

        [[nodiscard]] float getSpeed() const;

        void setSpeed(float speed);

        [[nodiscard]] float getFOV() const;

        void setFOV(float fov);

        [[nodiscard]] float getMouseSens() const;

        void setMouseSens(float sens);

        [[nodiscard]] glm::vec3 getWorldUp() const;

        void setWorldUp(const glm::vec3& worldUp);
    };
}
