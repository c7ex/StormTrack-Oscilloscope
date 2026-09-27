#include "DataState.hpp"
#include <chrono>

DataState::DataState() {}

const size_t DataState::GetDataSize() const {
    return data.size();
}

const std::vector<LinearData>& DataState::GetData() const {
    return data;
}

bool DataState::GetActiveState(size_t trace_index) {
    if (trace_index < data.size()) {
        return data[trace_index].GetStatus();
    }
    return false;
}

void DataState::SetActiveState(size_t trace_index, bool state) {
    if (trace_index < data.size()) {
        data[trace_index].SetStatus(state);
    }
}

void DataState::UpdateProperties(size_t trace_index, const TraceProperties& tp) {
    if (trace_index < data.size()) {
        data[trace_index].SetProperties(tp);
    }
}

void DataState::load(const std::vector<double>& load_data, COLORREF color, std::wstring caption, double step, double offset) {
    LinearData ldata;
    ldata.Init(load_data, color, caption, step, offset);
    data.push_back(ldata);
    size_t new_index = data.size() - 1;
    data[new_index].SetIndexTrace(new_index);
}

void DataState::stream(std::vector<double>& new_data, size_t trace_index) {
    data[trace_index].LoadData(new_data);
}

void DataState::append(const std::vector<double>& new_data, size_t trace_index) {
    data[trace_index].AppendData(new_data);
}

void DataState::append(const double new_value, size_t trace_index) {
    data[trace_index].AppendData(new_value);
}

void DataState::Draw(HDC hdc, const RenderCache& cache) {
    //double timeGDI = 0;
    //auto start = std::chrono::high_resolution_clock::now();

    constexpr size_t CHUNK_SIZE = 32;
    std::vector<POINT> chunk(CHUNK_SIZE);

    for (const auto& trace : cache.GetCaches()) {
        if (!trace.is_active || trace.points.empty()) continue;

        COLORREF color = data[trace.trace_id].GetColor();
        rwa::PEN pen(hdc, PS_SOLID, trace.is_compressed ? 1 : 2, color);

        size_t idx = 0;
        while (idx < trace.points.size()) {
            size_t count = (std::min)(CHUNK_SIZE, trace.points.size() - idx);

            for (size_t i = 0; i < count; ++i) {
                const auto& p = trace.points[idx + i];
                chunk[i] = { static_cast<LONG>(p.x), static_cast<LONG>(p.y) };
            }

            //auto startGPI = std::chrono::high_resolution_clock::now();

            Polyline(hdc, chunk.data(), static_cast<int>(count));

            //double durationGDI = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - startGPI).count();
            //timeGDI += durationGDI;

            idx += count;

            if (idx < trace.points.size()) {
                --idx;
            }
        }
    }

    //double now = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
    //std::cout << "draw: " << now << " us (max: " << timeGDI << " us)\n";
}