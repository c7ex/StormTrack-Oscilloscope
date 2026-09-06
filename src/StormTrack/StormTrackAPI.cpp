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

void splitComplexData(const std::vector<std::complex<double>>& complex_data, std::vector<double>& real_data, std::vector<double>& imag_data) {
	real_data.resize(complex_data.size());
	imag_data.resize(complex_data.size());
	for (size_t i = 0; i < complex_data.size(); ++i) {
		real_data[i] = complex_data[i].real();
		imag_data[i] = complex_data[i].imag();
	}
}

void StormTrack::JustView(const std::vector<std::complex<double>>& load_data, std::wstring caption_re, std::wstring caption_im, COLORREF color_re, COLORREF color_im, double step, double offset) {
	std::vector<double> real_data, imag_data;
	splitComplexData(load_data, real_data, imag_data);
	graphState.AddData(real_data, caption_re, color_re, step, offset);
	graphState.AddData(imag_data, caption_im, color_im, step, offset);
}

bool StormTrack::FrameView(const std::vector<std::complex<double>>& load_data, size_t trace_index_re, size_t trace_index_im) {
    std::vector<double> real_data, imag_data;
    splitComplexData(load_data, real_data, imag_data);
    bool re = graphState.StreamUpdate(real_data, trace_index_re);
    bool im = graphState.StreamUpdate(imag_data, trace_index_im);
    return re && im;
}

bool StormTrack::RealtimeView(const std::vector<std::complex<double>>& load_data, size_t trace_index_re, size_t trace_index_im) {
    std::vector<double> real_data, imag_data;
    splitComplexData(load_data, real_data, imag_data);
    bool re = graphState.StreamAppend(real_data, trace_index_re);
    bool im = graphState.StreamAppend(imag_data, trace_index_im);
    return re && im;
}

bool StormTrack::RealtimeView(const std::complex<double> load_value, size_t trace_index_re, size_t trace_index_im) {
	bool re = graphState.StreamAppend(load_value.real(), trace_index_re);
	bool im = graphState.StreamAppend(load_value.imag(), trace_index_im);
	return re && im;
}