#pragma once
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <stdexcept>
#include <utility>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glm/glm.hpp>

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
    GLFWwindow *m_Window = nullptr;
    WindowOptions m_Options;
    // Variables for tracking window timings essential for physics
    double m_LastFrameTime = 0.0; // Double for precision
    float m_DeltaTime = 0.0f;     // Float for game logic

    static void initGLFW()
    {
        static bool glfwInitialized = false;
        if (!glfwInitialized)
        {
            if (!glfwInit())
            {
                throw std::runtime_error("Failed to initialize GLFW\n");
            }
        }

        std::cout << "GLFW Initialized" << '\n';
        glfwInitialized = true;
    }

    void initGLAD() const
    {
        static bool gladInitialized = false;
        if (!gladInitialized)
        {
            if (!gladLoadGL(glfwGetProcAddress))
            {
                throw std::runtime_error("[Window]: Failed to initialize GLAD\n");
            }
            glViewport(0, 0, m_Options.width, m_Options.height);
            gladInitialized = true;
            std::cout << "[Window]: GLAD Initialized" << '\n';

            // Scaling fix to account for DPI on different displays
            int fbWidth, fbHeight;
            glfwGetFramebufferSize(m_Window, &fbWidth, &fbHeight);
            glViewport(0, 0, fbWidth, fbHeight);
        }
    }

    void createWindow()
    {
        // Applies different scaling for linux
#ifdef __linux__
        glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
#endif
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_Options.profileMajor);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_Options.profileMinor);
        glfwWindowHint(GLFW_OPENGL_PROFILE, static_cast<int>(m_Options.profile));

        m_Window = glfwCreateWindow(m_Options.width, m_Options.height, m_Options.name.c_str(), nullptr, nullptr);
        if (m_Window == nullptr)
        {
            throw std::runtime_error("[Window]: Failed to create GLFW window\n");
        }

        std::cout << "[Window]: Window Created" << '\n';
    }

public:
    explicit Window(WindowOptions options = {})
        : m_Options(std::move(options))
    {
        initGLFW();
        createWindow();
        glfwMakeContextCurrent(m_Window);
        setVSync(m_Options.vSync);
        initGLAD();
        glfwSetWindowUserPointer(m_Window, this);
        initImGui();
        m_LastFrameTime = glfwGetTime();
    }

    // Stops window class from being copied (no deleting the same pointers)
    Window(const Window &) = delete;

    Window &operator=(const Window &) = delete;

    ~Window()
    {
        if (m_Window != nullptr)
            glfwDestroyWindow(m_Window);
    }


    [[nodiscard]] double getWindowTime() const
    {
        return glfwGetTime();
    }

    void updateTime()
    {
        const double currentFrameTime = glfwGetTime();
        m_DeltaTime = static_cast<float>(currentFrameTime - m_LastFrameTime);
        m_LastFrameTime = currentFrameTime;
    }

    [[nodiscard]] float getDeltaTime() const { return m_DeltaTime; }

    void initImGui() const
    {
        // we only want to initialize imgui once for every class object
        static bool imguiInitialized = false;
        if (imguiInitialized) return;

        if (!imguiInitialized)
        {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            [[maybe_unused]] auto &io = ImGui::GetIO();
            ImGui::StyleColorsDark();
            imguiInitialized = true;
        }

        std::cout << "[Window]: ImGui Initialized" << '\n';

        // Initializes glfw and opengl imgui implementation
        ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
        ImGui_ImplOpenGL3_Init("#version 330 core");
    }


    void beginImgui() const
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void endImgui() const
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }


    void clear() const
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void setClearColour(const glm::vec4 &colour) const
    {
        glClearColor(colour.r, colour.g, colour.b, colour.a);
    }

    void swapBuffers() const { glfwSwapBuffers(m_Window); }
    void pollEvents() const { glfwPollEvents(); }
    [[nodiscard]] bool shouldClose() const { return glfwWindowShouldClose(m_Window); }

    // Handy for glfw callbacks
    [[nodiscard]] GLFWwindow *getGLFWWindow() const { return m_Window; }
    WindowOptions &getOptions() { return m_Options; }

    [[nodiscard]] int getWidth() const { return m_Options.width; }
    [[nodiscard]] int getHeight() const { return m_Options.height; }

    [[nodiscard]] bool isVSync() const { return m_Options.vSync; }
    static void setVSync(const bool enabled) { glfwSwapInterval(enabled ? 1 : 0); }

    [[nodiscard]] int getFramebufferWidth() const
    {
        int w, h;
        glfwGetFramebufferSize(m_Window, &w, &h);
        return w;
    }

    [[nodiscard]] int getFramebufferHeight() const
    {
        int w, h;
        glfwGetFramebufferSize(m_Window, &w, &h);
        return h;
    }
};


