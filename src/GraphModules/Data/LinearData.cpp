#include "LinearData.hpp"

size_t LinearData::size() const {
	return data.size();
}

const LinearDataParameters& LinearData::getParameters() const {
	return param;
}

const Position2d LinearData::operator [](size_t index) const 
{	
	double x = param.prop.offset + param.prop.step * static_cast<double>(index);
	double y = 0;
	if (index >= size()) {
		/* error access*/
		#ifdef _DEBUG
		return { x, 0 };
		#endif
	}
	else { y = data[index]; }
	return { x, y };
}

int64_t LinearData::ReturnIndexPoint(double x) const {
	int64_t index = static_cast<int64_t>(((x - param.prop.offset) / param.prop.step) + 0.5);
	if(index < 0 || index >= size())
	{ /* error access*/ }
	else { return index; }
	return -1;
}

void LinearData::SetStatus(bool status) {
	param.prop.active = status;
}

void LinearData::SetColor(COLORREF new_color) {
	param.prop.color = new_color;
}

void LinearData::SetIndexTrace(size_t new_index_trace) {
	param.index_trace = new_index_trace;
}

void LinearData::SetProperties(const TraceProperties& tp) {
	param.prop.color = tp.color;
	param.prop.offset = tp.offset;
	param.prop.step = tp.step;
}

bool LinearData::GetStatus() const {
	return param.prop.active;
}

size_t LinearData::GetIndexTrace() const {
	return param.index_trace;
}

COLORREF LinearData::GetColor() const {
	return param.prop.color;
}

double LinearData::GetUnprotectedX(size_t index) const {
	return param.prop.offset + param.prop.step * static_cast<double>(index);
}

double LinearData::GetUnprotectedY(size_t index) const {
	return data[index];
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

void LinearData::Init(const std::vector<double>& init_data, COLORREF init_color, std::wstring init_caption, double init_step, double init_offset) {
	data = init_data;
	param.index_trace = -1;
	param.prop.caption = init_caption;
	param.prop.color = init_color;
	param.prop.step = init_step;
	param.prop.offset = init_offset;
	param.prop.active = true;
}