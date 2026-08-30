#include "StormTrack.hpp"

const wchar_t* StormTrack::CLASS_NAME = L"StormTrack";

LRESULT CALLBACK StormTrack::WindowProcStatic(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    StormTrack* pThis = nullptr;

    if (msg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pThis = reinterpret_cast<StormTrack*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else {
        pThis = reinterpret_cast<StormTrack*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (pThis) {
        return pThis->WindowProc(hwnd, msg, wParam, lParam);
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool StormTrack::RegisterWindowClass() {
    WNDCLASSEX wcCheck = {};
    if (GetClassInfoEx(hInstance, CLASS_NAME, &wcCheck)) {
        return true;
    }

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProcStatic;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = CLASS_NAME;

    constexpr int icon_width = 256;
    constexpr int icon_height = 256;

    HICON hIcon = WinApiIconLoader::CreateIconFromICOMemory(
        icon::StormTrackIconData,
        icon::StormTrackIconDataSize,
        icon_width, icon_height
    );

    if (!hIcon) {
        hIcon = LoadIcon(NULL, IDI_APPLICATION);
    }

    wc.hIcon = hIcon;

    return RegisterClassEx(&wc) != 0;
}

bool StormTrack::Create(int nCmdShow) {

    Vec2d window_size = graphState.ExtractWindowSize();

    hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        windowTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        window_size.x,
        window_size.y,
        nullptr,
        nullptr,
        hInstance,
        this
    );

    if (!hwnd) {
        return false;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    windowCreated = true;
    return true;
}

void StormTrack::ThreadProc(int nCmdShow) {
    if (!Create(nCmdShow)) {
        windowCreated = false;
        return;
    }

    SetTimer(hwnd, TimerId, TimerInterval, NULL);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

StormTrack::StormTrack(HINSTANCE hInst, const wchar_t* title) {
    StormTrackInitParameters init_parameters;
    Create(hInst, init_parameters, title);
}

StormTrack::StormTrack(HINSTANCE hInst, StormTrackInitParameters init_parameters, const wchar_t* title) {
    Create(hInst, init_parameters, title);
}

void StormTrack::Create(HINSTANCE hInst, StormTrackInitParameters init_parameters, const wchar_t* title)
{
    hInstance = hInst;
    hwnd = nullptr;
    windowTitle = title;
    windowCreated = false;
    windowClosed = false;

    RegisterWindowClass();

    graphState.InitializeWindowSize(
        init_parameters.window_size.x,
        init_parameters.window_size.y);

    graphState.InitializeVisibleArea(
        init_parameters.visible_area_size.x,
        init_parameters.visible_area_size.y);

    graphState.InitializeReferencePosition(
        init_parameters.start_coordination.x,
        init_parameters.start_coordination.y);
}

StormTrack::~StormTrack() {
    Close();
    if (windowThread.joinable()) {
        windowThread.join();
    }
}