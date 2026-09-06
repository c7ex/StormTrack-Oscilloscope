#ifndef STORM_TRACK_HPP
#define STORM_TRACK_HPP

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

#include <windows.h>
#include <thread>
#include <atomic>
#include <complex>

#include"GraphState.hpp"
#include"WinApiIconLoader.hpp"

/*
    <StormTrack>
    description:
        A Windows window wrapper class that runs in a separate thread.
        This class encapsulates Windows API window creation and management,
        allowing windows to be created and controlled from console applications
        without blocking the main thread.

    Usage example:
        HINSTANCE hInstance = GetModuleHandle(nullptr);
        StormTrack window(hInstance, L"My Window");
        
        use:
            - JustView
            - AddTrace + FrameView/RealtimeView

        window.Show(); // Window runs in background thread
        window.Close(); // Close window
        window.WaitForClose(); // Wait for thread to finish

    Console remains interactive!

    C++11 or later
 */

struct StormTrackInitParameters {
    Vec2d window_size = { 800, 600 };
    Vec2d visible_area_size = { 100, 100 };
    Vec2d start_coordination = { 0, 0 };
};

class StormTrack {
private:
    HWND hwnd;
    HINSTANCE hInstance;
    static const wchar_t* CLASS_NAME;
    const wchar_t* windowTitle;
    std::thread windowThread;
    std::atomic<bool> windowCreated;
    std::atomic<bool> windowClosed;

private:
    GraphState graphState;

private:
    HBITMAP hBackBuffer = nullptr;
    int bufferWidth = 0;
    int bufferHeight = 0;
    
private:
    const UINT_PTR TimerId = 1;
    UINT TimerInterval = 24;

private:
    static LRESULT CALLBACK WindowProcStatic(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    bool RegisterWindowClass();
    bool Create(int nCmdShow);
    void ThreadProc(int nCmdShow);

public:
    StormTrack(HINSTANCE hInst, const wchar_t* title = L"StormTrack");
    StormTrack(HINSTANCE hInst, StormTrackInitParameters init_parameters = {}, const wchar_t* title = L"StormTrack");
    void Create(HINSTANCE hInst, StormTrackInitParameters init_parameters, const wchar_t* title);
    ~StormTrack();
    
public:
    bool Show(int nCmdShow = SW_SHOWDEFAULT);
    void Close();
    void WaitForClose();
    bool IsActive() const;

public:
    size_t AddTrace(std::wstring caption, COLORREF color, double step = 1, double offset = 0);

    // single
    void JustView(std::vector<double>& load_data, std::wstring caption, COLORREF color, double step = 1, double offset = 0);
    bool FrameView(std::vector<double>& load_data, size_t trace_index);
    bool RealtimeView(std::vector<double>& load_data, size_t trace_index);
    bool RealtimeView(const double load_value, size_t trace_index);

    // complex
    void JustView(const std::vector<std::complex<double>>& load_data, std::wstring caption_re, std::wstring caption_im, COLORREF color_re, COLORREF color_im, double step = 1, double offset = 0);
    bool FrameView(const std::vector<std::complex<double>>& load_data, size_t trace_index_re, size_t trace_index_im);
    bool RealtimeView(const std::vector<std::complex<double>>& load_data, size_t trace_index_re, size_t trace_index_im);
	bool RealtimeView(const std::complex<double> load_value, size_t trace_index_re, size_t trace_index_im);
};

#endif