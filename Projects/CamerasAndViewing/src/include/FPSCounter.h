#pragma once

class FPSCounter
{
private:
    float m_CurrentFPS = 0.0f;
    float m_FrameTime = 0.0f;
    float m_DeltaTime = 0.0f;

    int m_Frames = 0;
    float m_Timer = 0.0f;
    float m_UpdateInterval = 0.25f;
    bool m_HasData = false;

public:
    FPSCounter() = default;

    void update(float deltaTime);

    void drawUI() const;

    [[nodiscard]] float getCurrentFPS() const;

    // returns frame-time in (ms)
    [[nodiscard]] float getFrameTime() const;
};
