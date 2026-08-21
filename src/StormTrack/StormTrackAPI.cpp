#include "StormTrack.hpp"

// -> control-API

bool StormTrack::Show(int nCmdShow) {
    if (windowThread.joinable()) {
        return false;
    }

    windowThread = std::thread(&StormTrack::ThreadProc, this, nCmdShow);

    while (!windowCreated && !windowClosed) {
        Sleep(10);
    }

    return windowCreated;
}

void StormTrack::Close() {
    if (hwnd && !windowClosed) {
        PostMessage(hwnd, WM_CLOSE, 0, 0);
    }
}

void StormTrack::WaitForClose() {
    if (windowThread.joinable()) {
        windowThread.join();
    }
}

bool StormTrack::IsActive() const {
    return (hwnd != nullptr) && (!windowClosed);
}

// -> view-API

size_t StormTrack::AddTrace(std::wstring caption, COLORREF color, double step, double offset) {
    return graphState.CreateTrace(caption, color, step, offset);
}

void StormTrack::JustView(std::vector<double>& load_data, std::wstring caption, COLORREF color, double step, double offset) {
    graphState.AddData(load_data, caption, color, step, offset);
}

bool StormTrack::FrameView(std::vector<double>& load_data, size_t trace_index) {
    return graphState.StreamUpdate(load_data, trace_index);
}

bool StormTrack::RealtimeView(std::vector<double>& load_data, size_t trace_index) {
    return graphState.StreamAppend(load_data, trace_index);
}

bool StormTrack::RealtimeView(const double load_value, size_t trace_index) {
	return graphState.StreamAppend(load_value, trace_index);
}