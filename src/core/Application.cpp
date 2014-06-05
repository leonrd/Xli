#include <Xli/Application.h>

namespace Xli
{
    void Application::Run(Application* app, int flags)
    {
        Window::Init();

        Managed<Window> wnd = Window::Create(app->GetInitSize(), app->GetInitTitle(), flags);

        app->OnInit(wnd);
        wnd->SetEventHandler(app);
        app->OnLoad(wnd);

        Application::PreMainLoop(app, wnd);
        while (app->GetIsRunning(wnd))
        {
            app->OnDraw(wnd);
            Window::ProcessMessages();
        }
        Application::PostMainLoop(app, wnd);

        Window::Done();
    }
    
    void Application::OnInit(Window* wnd)
    {
    }
    
    void Application::OnLoad(Window* wnd)
    {
    }

    void Application::OnDraw(Window* wnd)
    {
    }
    bool Application::GetIsRunning(Window* wnd)
    {
        return !wnd->IsClosed();
    }

    String Application::GetInitTitle()
    {
        return "Xli Application";
    }

    Vector2i Application::GetInitSize()
    {
#if defined(XLI_PLATFORM_IOS) || defined(XLI_PLATFORM_ANDROID)
        return Window::GetScreenSize();
#else
        return Vector2i(1280, 720);
#endif
    }

    void Application::OnSizeChanged(Window* wnd, Vector2i size)
    {
#if defined(XLI_PLATFORM_WIN32) || defined(XLI_PLATFORM_OSX)
        if (wnd->GetMouseButtonState(MouseButtonLeft))
            OnDraw(wnd);
#endif
    }

    void Application::PreMainLoop(Application* app,Window* wnd)
    {
#if defined(XLI_PLATFORM_IOS) || defined(XLI_PLATFORM_OSX)
        app->OnAppDidEnterForeground();
#endif

#if defined(XLI_PLATFORM_ANDROID)       
        wnd->BeginTextInput(TextInputHintDefault);
        wnd->EndTextInput();
#endif
    }

    void Application::PostMainLoop(Application* app,Window* wnd)
    {
#if defined(XLI_PLATFORM_OSX)
        app->OnAppTerminating();
#endif
    }
}
