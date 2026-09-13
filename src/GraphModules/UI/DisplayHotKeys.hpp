#ifndef DISPLAY_HOT_KEYS_HPP
#define DISPLAY_HOT_KEYS_HPP

#include "ConfigUI.hpp"
#include "FpsState.hpp"
#include "AutoScaler.hpp"
#include "WindowState.hpp"
#include "GraphContext.hpp"

enum UserKeys;
class WindowState;
class GraphContext;

class DisplayHotKeys {
private:
	void drawSingleHotKeyState(HDC hdc, GraphContext& context, int key, bool active, int& offset_x, int offset_y);

public:
	void drawHotKeyStates(HDC hdc, GraphContext& context, const AutoScaler& autoscaler, const FpsState& fps_state);
};

#endif