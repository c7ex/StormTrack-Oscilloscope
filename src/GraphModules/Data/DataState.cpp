#include "DataState.hpp"
#include <chrono>

DataState::DataState() {}

const std::vector<LinearData>& DataState::GetData() const {
    return data;
}

void DataState::load(std::vector<double>& load_data, double step, double offset) {
    LinearData ldata;
    ldata.Init(load_data, step, offset);
    data.push_back(ldata);
}

void DataState::stream(std::vector<double>& new_data, size_t trace_index) {
    data[trace_index].LoadData(new_data);
}

void DataState::append(std::vector<double>& new_data, size_t trace_index) {
    data[trace_index].AppendData(new_data);
}

void DataState::update_data(const LegendItem& li) {
    for (int i = 0; i < data.size(); ++i) {
        data[i].SetStatus(li.IsActive(i));
        data[i].SetColor(li.GetColor(i));
        data[i].SetIndexTrace(i);
    }
}

void DataState::draw(HDC hdc, const RenderCache& cache, const LegendItem& li) {
    //double timeGDI = 0;
    //auto start = std::chrono::high_resolution_clock::now();

    update_data(li);

    constexpr size_t CHUNK_SIZE = 32;
    std::vector<POINT> chunk(CHUNK_SIZE);

    for (const auto& trace : cache.GetCaches()) {
        if (!trace.is_active || trace.points.empty()) continue;

        COLORREF color = li.GetColor(trace.trace_id);
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