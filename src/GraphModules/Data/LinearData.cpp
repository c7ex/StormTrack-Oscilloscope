#include "LinearData.hpp"

size_t LinearData::size() const {
	return data.size();
}

const LinearDataParameters LinearData::getParameters() const {
	return LinearDataParameters{ step, offset, active };
}

const Position2d LinearData::operator [](size_t index) const 
{	
	double x = offset + step * static_cast<double>(index);
	double y = 0;
	if (index >= size())
	{
		/* error access*/
		#ifdef _DEBUG
		return { x, 0 };
		#endif
	}
	else { y = data[index]; }
	return { x, y };
}

int64_t LinearData::ReturnIndex(double x) const {
	int64_t index = static_cast<int64_t>(((x - offset) / step) + 0.5);
	if(index < 0 || index >= size())
	{ /* error access*/ }
	else { return index; }
	return -1;
}

void LinearData::SetStatus(bool status) {
	active = status;
}

void LinearData::SetColor(COLORREF new_color) {
	color = new_color;
}

void LinearData::SetIndexTrace(size_t new_index_trace) {
	index_trace = new_index_trace;
}

bool LinearData::GetStatus() const {
	return active;
}

size_t LinearData::GetIndexTrace() const {
	return index_trace;
}

COLORREF LinearData::GetColor() const {
	return color;
}

void LinearData::LoadData(std::vector<double>& new_data) {
	data = std::move(new_data);
}

void LinearData::AppendData(const std::vector<double>& new_data) {
	data.insert(data.end(), new_data.begin(), new_data.end());
}

void LinearData::AppendData(const double new_value) {
	data.push_back(new_value);
}

void LinearData::Init(const std::vector<double>& init_data, double init_step, double init_offset) {
	data = init_data;
	step = init_step;
	offset = init_offset;
	active = true;
}

LinearData::LinearData() {
	step = 1;
	offset = 0;
	active = true;
	color = RGB(255, 255, 255);
	index_trace = 0;
}