#pragma once
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <string>
#include <glm/glm.hpp>


namespace core
{
    enum class GLProfile
    {
        Core          = GLFW_OPENGL_CORE_PROFILE,
        Compatibility = GLFW_OPENGL_COMPAT_PROFILE,
        ANY           = GLFW_OPENGL_ANY_PROFILE
    };

    struct WindowOptions
    {
        //OpenGL Context Settings
        std::string name = "GLFW Window";
        int profileMajor = 4;
        int profileMinor = 6;
        int width = 800;
        int height = 600;
        bool vSync = true;
        GLProfile profile = GLProfile::Core;
    };

    class Window
    {
    private:
        GLFWwindow* m_Window = nullptr;
        WindowOptions m_Options;
        // Variables for tracking window timings essential for physics
        double m_LastFrameTime = 0.0;// Double for precision
        float m_DeltaTime = 0.0f;    // Float for game logic

        static void initGLFW();
        void initGLAD() const;

        void initImGui() const;
        void createWindow();

    public:
        explicit Window(WindowOptions options = {});

        ~Window();

        Window(const Window&) = delete;

        Window& operator=(const Window&) = delete;

        [[nodiscard]] double getWindowTime() const;

        void updateTime();

        [[nodiscard]] float getDeltaTime() const;


        void beginImgui() const;

        void endImgui() const;

        void clear() const;

        void setClearColour(const glm::vec4& colour) const;

        void swapBuffers() const;

        static void pollEvents();

        [[nodiscard]] bool shouldClose() const;

        // Handy for glfw callbacks
        [[nodiscard]] GLFWwindow* getGLFWWindow() const;

        WindowOptions& getOptions();

        [[nodiscard]] int getWidth() const;

        [[nodiscard]] int getHeight() const;

        [[nodiscard]] bool isVSync() const;

        void setVSync(const bool enabled) const;

        [[nodiscard]] int getFramebufferWidth() const;

        [[nodiscard]] int getFramebufferHeight() const;
    };
}
