#ifndef _LEGENDITEM_HPP
#define _LEGENDITEM_HPP

#include<vector>
#include<windows.h>

#include"Vec2d.hpp"
#include"RaiiWinApi.hpp"
#include"GraphContext.hpp"
#include"TransformCoordinates.hpp"

#include"DataState.hpp"
#include"ConfigUI.hpp"

class GraphContext;
class TransformCoordinates;
class LinearData;
class DataState;

class LegendItem {
public:
	void Draw(HDC hdc, GraphContext& context, const DataState& data);
	void HitCheckAndToggle(GraphContext& context, HWND hwnd, DataState& data);

private:
    void DrawBackground(HDC hdc, const RECT& rect);
	void DrawFrame(HDC hdc, const RECT& rect);
    void DrawColorBox(HDC hdc, int x, int y, int size, const LinearData& current_data);
    void DrawCaptions(HDC hdc, int x, int y, const std::wstring& text);
};

#endif