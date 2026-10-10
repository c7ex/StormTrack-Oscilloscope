#ifndef DATA_STATE_HPP
#define DATA_STATE_HPP

#include"RaiiWinApi.hpp"
#include"LinearData.hpp"
#include"LegendItem.hpp"
#include"RenderCache.hpp"

class LegendItem;
class RenderCache;

class DataState {
private:
    std::vector<LinearData> data;

public:
    void Draw(HDC hdc, const RenderCache& cache);

public:
    const size_t GetDataSize() const;
    const std::vector<LinearData>& GetData() const;
    bool GetActiveState(size_t trace_index);
    void SetActiveState(size_t trace_index, bool state);
    void UpdateProperties(size_t trace_index, const TraceProperties& tp);

public:
    void load(const std::vector<double>& load_data, COLORREF color, std::wstring caption, double step = 1., double offset = 0.);
    void stream(std::vector<double>& new_data, size_t trace_index);
    void append(const std::vector<double>& new_data, size_t trace_index);
    void append(const double new_value, size_t trace_index);

public:
    explicit DataState();
};

#endif