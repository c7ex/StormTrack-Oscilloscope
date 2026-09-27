#ifndef LINEAR_DATA_HPP
#define LINEAR_DATA_HPP

/* Rules of Data
*  The data goes with a constant step of "X"
*  ranges X: [- : +]
*  ranges Y: [- : +]
*/

#include <windows.h>
#include <vector>
#include <string>

#include "Vec2d.hpp"

struct TraceProperties {
	std::wstring caption = L"New Trace";
	COLORREF color = RGB(255, 255, 255);
	double step = 1;
	double offset = 0;
	bool active = true;
};

struct LinearDataParameters {
	size_t index_trace = -1;
	TraceProperties prop;
};

class LinearData{
private:
	std::vector<double> data;
	LinearDataParameters param;

public:
	size_t size() const;
	const LinearDataParameters& getParameters() const;

public:
	const Position2d operator [](size_t index) const;

public:
	int64_t ReturnIndexPoint(double x) const;
	
	void SetStatus(bool status);
	void SetColor(COLORREF new_color);
	void SetIndexTrace(size_t new_index_trace);
	void SetProperties(const TraceProperties& tp);

	bool GetStatus() const;
	size_t GetIndexTrace() const;
	COLORREF GetColor() const;

public:
	void LoadData(std::vector<double>& new_data);
	void AppendData(const std::vector<double>& new_data);
	void AppendData(const double new_value);

public:
	void Init(const std::vector<double>& init_data, COLORREF init_color, std::wstring init_caption, double init_step, double init_offset);
};

#endif