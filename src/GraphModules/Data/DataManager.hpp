#ifndef DATA_MANAGER_HPP
#define DATA_MANAGER_HPP

#include <vector>
#include <string>

#include "DataState.hpp"

class DataManager {
private:
	size_t not_founded_trace_bonds = static_cast<size_t>(-1);

private:
	struct trace_bond {
		std::wstring name;
		size_t index;
	};

private:
	std::vector<trace_bond> trace_bonds;

private:
	size_t found_trace_bonds(const std::wstring& name) {
		for (size_t i = 0; i < trace_bonds.size(); ++i) {
			if (name == trace_bonds[i].name) {
				return trace_bonds[i].index;
			}
		}

		return not_founded_trace_bonds;
	}

	void init_trace_bonds(const std::vector<double>& trace_data, const TraceProperties& tp, DataState& data) {
		data.load(trace_data, tp.color, tp.caption, tp.step, tp.offset);
		size_t new_index_trace = data.GetDataSize() - 1;
		trace_bond new_trace_bond = { tp.caption, new_index_trace };
		trace_bonds.push_back(new_trace_bond);
	}

public:
	void SetTraceParameters(const TraceProperties& tp, DataState& data) {
		const std::wstring& name = tp.caption;
		size_t index_trace = found_trace_bonds(name);

		if (index_trace == not_founded_trace_bonds) {
			std::vector<double> empty_data = {};
			init_trace_bonds(empty_data, tp, data);
		} else {
			data.UpdateProperties(index_trace, tp);
		}
	}

public:
	// stream case (vector)
	void Refresh(std::vector<double>& new_data, const TraceProperties& tp, DataState& data) {
		const std::wstring& name = tp.caption;
		size_t index_trace = found_trace_bonds(name);

		if (index_trace == not_founded_trace_bonds) {
			init_trace_bonds(new_data, tp, data);
		} else {
			data.UpdateProperties(index_trace, tp);
			data.stream(new_data, index_trace);
		}
	}

	void Refresh(std::vector<double>& new_data, std::wstring caption, DataState& data) {
		const std::wstring& name = caption;
		size_t index_trace = found_trace_bonds(name);

		if (index_trace == not_founded_trace_bonds) {
			TraceProperties tp = { caption };
			init_trace_bonds(new_data, tp, data);
		} else {
			data.stream(new_data, index_trace);
		}
	}

public:
	// realtime case (vector)
	void Append(const std::vector<double>& new_data, const TraceProperties& tp, DataState& data) {
		const std::wstring& name = tp.caption;
		size_t index_trace = found_trace_bonds(name);

		if (index_trace == not_founded_trace_bonds) {
			init_trace_bonds(new_data, tp, data);
		} else {
			data.UpdateProperties(index_trace, tp);
			data.append(new_data, index_trace);
		}
	}

	void Append(const std::vector<double>& new_data, std::wstring caption, DataState& data) {
		const std::wstring& name = caption;
		size_t index_trace = found_trace_bonds(name);

		if (index_trace == not_founded_trace_bonds) {
			TraceProperties tp = { caption };
			init_trace_bonds(new_data, tp, data);
		} else {
			data.append(new_data, index_trace);
		}
	}

public:
	// realtime case (point)
	void Append(const double point, const TraceProperties& tp, DataState& data) {
		const std::wstring& name = tp.caption;
		size_t index_trace = found_trace_bonds(name);

		if (index_trace == not_founded_trace_bonds) {
			std::vector<double> point_data = { point };
			init_trace_bonds(point_data, tp, data);
		}
		else {
			data.UpdateProperties(index_trace, tp);
			data.append(point, index_trace);
		}
	}

	void Append(const double point, std::wstring caption, DataState& data) {
		const std::wstring& name = caption;
		size_t index_trace = found_trace_bonds(name);

		if (index_trace == not_founded_trace_bonds) {
			TraceProperties tp = { caption };
			std::vector<double> point_data = { point };
			init_trace_bonds(point_data, tp, data);
		} else {
			data.append(point, index_trace);
		}
	}
};

#endif