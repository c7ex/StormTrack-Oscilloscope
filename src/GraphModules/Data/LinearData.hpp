#ifndef LINEAR_DATA_HPP
#define LINEAR_DATA_HPP

/* Rules of Data
*  The data goes with a constant step of "X"
*  ranges X: [- : +]
*  ranges Y: [- : +]
*/

#include <windows.h>
#include <vector>

#include "Vec2d.hpp"

struct LinearDataParameters {
	double step;
	double offset;
	double active;
	COLORREF color;
	size_t index_trace;
};

class LinearData{
private:
	std::vector<double> data;
	double step;
	double offset;
	double active;
	COLORREF color;
	size_t index_trace;

public:
	size_t size() const;
	const LinearDataParameters getParameters() const;

public:
	const Position2d operator [](size_t index) const;

public:
	int64_t ReturnIndex(double x) const;
	
	void SetStatus(bool status);
	void SetColor(COLORREF new_color);
	void SetIndexTrace(size_t new_index_trace);

	bool GetStatus() const;
	size_t GetIndexTrace() const;
	COLORREF GetColor() const;

public:
	void LoadData(std::vector<double>& new_data);
	void AppendData(const std::vector<double>& new_data);
	void AppendData(const double new_value);

public:
	void Init(const std::vector<double>& init_data, double init_step, double init_offset);
	LinearData();
};

#endif