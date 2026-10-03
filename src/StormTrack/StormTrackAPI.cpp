#include "StormTrack.hpp"

#include "GraphState.hpp"

struct StormTrack::implcore {
    GraphState graphState;
};

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
    return implcore_->graphState.CreateTrace(caption, color, step, offset);
}

void StormTrack::JustView(std::vector<double>& load_data, std::wstring caption, COLORREF color, double step, double offset) {
    implcore_->graphState.AddData(load_data, caption, color, step, offset);
}

bool StormTrack::FrameView(std::vector<double>& load_data, size_t trace_index) {
    return implcore_->graphState.StreamUpdate(load_data, trace_index);
}

bool StormTrack::RealtimeView(std::vector<double>& load_data, size_t trace_index) {
    return implcore_->graphState.StreamAppend(load_data, trace_index);
}

bool StormTrack::RealtimeView(const double load_value, size_t trace_index) {
	return implcore_->graphState.StreamAppend(load_value, trace_index);
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
	implcore_->graphState.AddData(real_data, caption_re, color_re, step, offset);
	implcore_->graphState.AddData(imag_data, caption_im, color_im, step, offset);
}

bool StormTrack::FrameView(const std::vector<std::complex<double>>& load_data, size_t trace_index_re, size_t trace_index_im) {
    std::vector<double> real_data, imag_data;
    splitComplexData(load_data, real_data, imag_data);
    bool re = implcore_->graphState.StreamUpdate(real_data, trace_index_re);
    bool im = implcore_->graphState.StreamUpdate(imag_data, trace_index_im);
    return re && im;
}

bool StormTrack::RealtimeView(const std::vector<std::complex<double>>& load_data, size_t trace_index_re, size_t trace_index_im) {
    std::vector<double> real_data, imag_data;
    splitComplexData(load_data, real_data, imag_data);
    bool re = implcore_->graphState.StreamAppend(real_data, trace_index_re);
    bool im = implcore_->graphState.StreamAppend(imag_data, trace_index_im);
    return re && im;
}

bool StormTrack::RealtimeView(const std::complex<double> load_value, size_t trace_index_re, size_t trace_index_im) {
	bool re = implcore_->graphState.StreamAppend(load_value.real(), trace_index_re);
	bool im = implcore_->graphState.StreamAppend(load_value.imag(), trace_index_im);
	return re && im;
}

void StormTrack::UniqueTrace(std::wstring caption, COLORREF color, double step, double offset) {
    TraceProperties tp = { caption, color, step, offset };
    implcore_->graphState.ManagerDoTrace(tp);
}

void StormTrack::UniqueStream(std::vector<double>& load_data, std::wstring caption, COLORREF color, double step, double offset) {
    TraceProperties tp = { caption, color, step, offset };
    implcore_->graphState.ManagerStream(load_data, tp);
}

void StormTrack::UniqueStream(std::vector<double>& load_data, std::wstring caption) {
    implcore_->graphState.ManagerStream(load_data, caption);
}

void StormTrack::UniqueStream(const std::vector<std::complex<double>>& load_data, std::wstring caption, COLORREF color_re, COLORREF color_im, double step, double offset) {
    TraceProperties tp_re = { caption + L"-Re", color_re, step, offset};
    TraceProperties tp_im = { caption + L"-Im", color_im, step, offset };
    std::vector<double> real_data, imag_data;
    splitComplexData(load_data, real_data, imag_data);
    implcore_->graphState.ManagerStream(real_data, tp_re);
    implcore_->graphState.ManagerStream(imag_data, tp_im);
}

void StormTrack::UniqueStream(const std::vector<std::complex<double>>& load_data, std::wstring caption) {
    std::vector<double> real_data, imag_data;
    splitComplexData(load_data, real_data, imag_data);
    implcore_->graphState.ManagerStream(real_data, caption + L"-Re");
    implcore_->graphState.ManagerStream(imag_data, caption + L"-Im");
}

void StormTrack::UniquePushBack(const std::vector<double>& load_data, std::wstring caption, COLORREF color, double step, double offset) {
    TraceProperties tp = { caption, color, step, offset };
    implcore_->graphState.ManagerAppend(load_data, tp);
}

void StormTrack::UniquePushBack(const std::vector<double>& load_data, std::wstring caption) {
    implcore_->graphState.ManagerAppend(load_data, caption);
}

void StormTrack::UniquePushBack(const std::vector<std::complex<double>>& load_data, std::wstring caption, COLORREF color_re, COLORREF color_im, double step, double offset) {
    TraceProperties tp_re = { caption + L"-Re", color_re, step, offset };
    TraceProperties tp_im = { caption + L"-Im", color_im, step, offset };
    std::vector<double> real_data, imag_data;
    splitComplexData(load_data, real_data, imag_data);
    implcore_->graphState.ManagerAppend(real_data, tp_re);
    implcore_->graphState.ManagerAppend(imag_data, tp_im);
}

void StormTrack::UniquePushBack(const std::vector<std::complex<double>>& load_data, std::wstring caption) {
    std::vector<double> real_data, imag_data;
    splitComplexData(load_data, real_data, imag_data);
    implcore_->graphState.ManagerAppend(real_data, caption + L"-Re");
    implcore_->graphState.ManagerAppend(imag_data, caption + L"-Im");
}

void StormTrack::UniquePushBack(const double point, std::wstring caption, COLORREF color, double step, double offset) {
    TraceProperties tp = { caption, color, step, offset };
    implcore_->graphState.ManagerAppend(point, tp);
}

void StormTrack::UniquePushBack(const double point, std::wstring caption) {
    implcore_->graphState.ManagerAppend(point, caption);
}

void StormTrack::UniquePushBack(const std::complex<double> point, std::wstring caption, COLORREF color_re, COLORREF color_im, double step, double offset) {
    TraceProperties tp_re = { caption + L"-Re", color_re, step, offset };
    TraceProperties tp_im = { caption + L"-Im", color_im, step, offset };
    implcore_->graphState.ManagerAppend(point.real(), tp_re);
    implcore_->graphState.ManagerAppend(point.imag(), tp_im);
}

void StormTrack::UniquePushBack(const std::complex<double> point, std::wstring caption) {
    implcore_->graphState.ManagerAppend(point.real(), caption + L"-Re");
    implcore_->graphState.ManagerAppend(point.imag(), caption + L"-Im");
}
