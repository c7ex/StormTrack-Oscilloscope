#ifndef FPS_STATE_HPP
#define FPS_STATE_HPP

#include "ConfigUI.hpp"
#include "WindowState.hpp"
#include "GraphContext.hpp"

enum UserKeys;
class WindowState;
class GraphContext;

class FpsState {
private:
    double average_fps = ConfigUI::Fps::default_fps;
    const size_t window_smoov_fps = ConfigUI::Fps::default_window_smoov_fps;

private:
    bool active = ConfigUI::Fps::default_active;
    bool holder = true;

public:
    void SwitchActive(const WindowState& window);
	void Draw(GraphContext& context, HDC hdc, int fps);
};

#endif
