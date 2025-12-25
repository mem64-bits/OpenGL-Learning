#pragma once

#include <GLFW/glfw3.h>
#include <imgui.h>

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
    // --- FIX: Constructor handles the initial time ---
    FPSCounter() = default;

    void update(const float deltaTime)
    {
        m_DeltaTime = deltaTime;
        m_Timer += m_DeltaTime;
        m_Frames++;

        if (m_Timer >= m_UpdateInterval)
        {
            m_CurrentFPS = static_cast<float>(m_Frames) / m_Timer;
            m_FrameTime = m_DeltaTime * 1000.0f;
            m_Frames = 0;
            m_Timer = 0.0f;
            m_HasData = true;
        }
    }

    void drawUI() const
    {
        if (!m_HasData) return;

        // Use ImGuiCond_Always to ensure it stays in the corner
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);

        // --- POLISH: Added NoInputs and NoNav so the overlay is "ghost-like" ---
        ImGui::Begin("##FPSOverlay", nullptr,
                     ImGuiWindowFlags_NoDecoration |
                     ImGuiWindowFlags_AlwaysAutoResize |
                     ImGuiWindowFlags_NoBackground |
                     ImGuiWindowFlags_NoInputs |
                     ImGuiWindowFlags_NoNav);

        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "FPS: %.1f", m_CurrentFPS);
        // Pro addition: show the raw frame time in ms
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Frametime %.2f ms", m_FrameTime);

        ImGui::End();
    }

    [[nodiscard]] float getCurrentFPS() const { return m_CurrentFPS; }
    // returns frametime in (ms)
    float getFrameTime() const { return m_DeltaTime * 1000.0f; }
};
