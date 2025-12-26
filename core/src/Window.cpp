#include <Window.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>


namespace core
{
    void Window::initGLFW()
    {
        static bool glfwInitialized = false;
        if(!glfwInitialized)
        {
            if(!glfwInit())
            {
                throw std::runtime_error("Failed to initialize GLFW\n");
            }
        }

        std::cout << "[Window]: GLFW Initialized" << '\n';
        glfwInitialized = true;
    }

    void Window::initGLAD() const
    {
        static bool gladInitialized = false;
        if(!gladInitialized)
        {
            if(!gladLoadGL(glfwGetProcAddress))
            {
                throw std::runtime_error("[Window]: Failed to initialize GLAD\n");
            }
            glViewport(0, 0, m_Options.width, m_Options.height);
            gladInitialized = true;
            std::cout << "[Window]: GLAD Initialized" << '\n';

            // Scaling fix to account for DPI on different displays
            int fbWidth, fbHeight;
            glfwGetFramebufferSize(m_Window, & fbWidth, & fbHeight);
            glViewport(0, 0, fbWidth, fbHeight);
        }
    }

    void Window::initImGui() const
    {
        // we only want to initialize imgui once for every class object
        static bool imguiInitialized = false;
        if(imguiInitialized) return;

        if(!imguiInitialized)
        {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            [[maybe_unused]] auto& io = ImGui::GetIO();
            ImGui::StyleColorsDark();
            imguiInitialized = true;
        }

        std::cout << "[Window]: ImGui Initialized" << '\n';

        // Initializes glfw and opengl imgui implementation
        ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
        ImGui_ImplOpenGL3_Init("#version 330 core");
    }


    void Window::createWindow()
    {
        // Applies different scaling for linux
#ifdef __linux__
        glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
#endif
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_Options.profileMajor);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_Options.profileMinor);
        glfwWindowHint(GLFW_OPENGL_PROFILE, static_cast<int>( m_Options.profile ));

        m_Window = glfwCreateWindow(m_Options.width, m_Options.height, m_Options.name.c_str(), nullptr, nullptr);
        if(m_Window == nullptr)
        {
            throw std::runtime_error("[Window]: Failed to create GLFW window\n");
        }

        std::cout << "[Window]: Window Created" << '\n';
    }


    Window::Window(WindowOptions options)
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


    Window::~Window()
    {
        if(m_Window != nullptr)
            glfwDestroyWindow(m_Window);
    }

    double Window::getWindowTime() const
    {
        return glfwGetTime();
    }

    void Window::updateTime()
    {
        const double currentFrameTime = glfwGetTime();
        m_DeltaTime = static_cast<float>( currentFrameTime - m_LastFrameTime );
        m_LastFrameTime = currentFrameTime;
    }

    [[nodiscard]] float Window::getDeltaTime() const { return m_DeltaTime; }


    void Window::beginImgui() const
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void Window::endImgui() const
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }


    void Window::clear() const
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Window::setClearColour(const glm::vec4& colour) const
    {
        glClearColor(colour.r, colour.g, colour.b, colour.a);
    }

    void Window::swapBuffers() const { glfwSwapBuffers(m_Window); }
    void Window::pollEvents() { glfwPollEvents(); }
    [[nodiscard]] bool Window::shouldClose() const { return glfwWindowShouldClose(m_Window); }

    // Handy for glfw callbacks
    [[nodiscard]] GLFWwindow* Window::getGLFWWindow() const { return m_Window; }
    WindowOptions& Window::getOptions() { return m_Options; }

    [[nodiscard]] int Window::getWidth() const { return m_Options.width; }
    [[nodiscard]] int Window::getHeight() const { return m_Options.height; }

    [[nodiscard]] bool Window::isVSync() const { return m_Options.vSync; }
    void Window::setVSync(const bool enabled) const { glfwSwapInterval(enabled ? 1 : 0); }

    [[nodiscard]] int Window::getFramebufferWidth() const
    {
        int w, h;
        glfwGetFramebufferSize(m_Window, & w, & h);
        return w;
    }

    [[nodiscard]] int Window::getFramebufferHeight() const
    {
        int w, h;
        glfwGetFramebufferSize(m_Window, & w, & h);
        return h;
    }
}

