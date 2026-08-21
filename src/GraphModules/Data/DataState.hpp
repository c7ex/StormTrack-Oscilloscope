#ifndef DATA_STATE_H
#define DATA_STATE_H

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
    explicit DataState();

    const std::vector<LinearData>& GetData() const;

    void load(std::vector<double>& load_data, double step = 1, double offset = 0);
    void stream(std::vector<double>& new_data, size_t trace_index);
    void append(std::vector<double>& new_data, size_t trace_index);
    void append(const double new_value, size_t trace_index);

    void update_data(const LegendItem& li);

    void draw(HDC hdc, const RenderCache& cache, const LegendItem& li);
};

#endif