#include "Application.hpp"

Application::Application() : state(ApplicationState::NO_CURRENT_GAME)
{
}

void Application::initializeSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        std::println(stderr, "Error: {0}", SDL_GetError());
        return;
    }

    SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "permonitorv2");
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

    window = SDL_CreateWindow("mooChess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);

    if (window == nullptr)
    {
        std::println(stderr, "Error: SDL_CreateWindow(): {0}", SDL_GetError());
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    if (renderer == nullptr)
    {
        SDL_Log("Error creating SDL_Renderer!");
        return;
    }
}

void Application::initializeImGUI()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    io = &ImGui::GetIO(); // (void)io;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
}

void Application::loadFonts()
{
    ImFontConfig config;
    config.FontDataOwnedByAtlas = false;
    config.RasterizerMultiply = 1.2f;
    config.GlyphOffset.y = 1.0f;
    config.PixelSnapH = true;

    // double font size, half the scaling = Multi-sample Anti-Aliasing
    // io->FontGlobalScale = 0.5f;

    // Only if using FreeType with ImGui
#ifdef IMGUI_ENABLE_FREETYPE
    config.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_ForceAutoHint;
#endif

    font_opensans_18px = io->Fonts->AddFontFromFileTTF("assets/OpenSans-Light.ttf", 18.0f, &config);

    if (font_opensans_18px == nullptr)
    {
        std::println(stderr, "Could not load font");
    }

    font_opensans_36px = io->Fonts->AddFontFromFileTTF("assets/OpenSans-Light.ttf", 36.0f, &config);
}

void Application::handleSDLEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);

        if (event.type == SDL_QUIT)
        {
            state = ApplicationState::QUIT;
        }

        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
        {
            state = ApplicationState::QUIT;
        }
    }
}

void Application::renderUI()
{
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    if (state == ApplicationState::NO_CURRENT_GAME)
    {
        mainMenuScene->render();
    }

    if (state == ApplicationState::PLAYING)
    {
        gameScene->render();
    }

    ImGui::Render();
}

void Application::display()
{
    static ImVec4 clear_color = {0.45f, 0.55f, 0.60f, 1.00f};

    SDL_RenderSetScale(renderer, io->DisplayFramebufferScale.x, io->DisplayFramebufferScale.y);
    SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
    SDL_RenderClear(renderer);
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
}

void Application::shutDown()
{
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Application::initialize()
{
    initializeSDL();
    initializeImGUI();

    loadFonts();

    initializeScenes();
}

void Application::startQuickMatch(unsigned short time_limit_min, unsigned short time_increment_sec)
{
    // TODO: call server
    gameScene->startNewGame(std::rand() % 2 ? PieceColor::BLACK : PieceColor::WHITE);

    state = ApplicationState::PLAYING;
}

void Application::suggestDraw()
{
    state = ApplicationState::NO_CURRENT_GAME;
}

void Application::resign()
{
    state = ApplicationState::NO_CURRENT_GAME;
}

void Application::initializeScenes()
{
    mainMenuScene = new MainMenuScene(window, renderer, io, [this](unsigned short a, unsigned short b){ this->startQuickMatch(a, b); }, font_opensans_36px);
    gameScene = new GameScene(window, renderer, io, font_opensans_18px, [this](){ this->suggestDraw(); }, [this](){ this->resign(); });
}

void Application::run()
{
    initialize();

    while (state != ApplicationState::QUIT)
    {
        handleSDLEvents();

        renderUI();

        display();
    }

    shutDown();
}
