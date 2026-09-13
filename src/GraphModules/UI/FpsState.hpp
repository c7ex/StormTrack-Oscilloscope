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
	double average_fps = static_cast<double>(ConfigUI::Fps::default_fps);
    bool active = ConfigUI::Fps::default_active;
    bool holder = true;

public:
	bool GetState() const;
    void SwitchActive(const WindowState& window);
	void Draw(GraphContext& context, HDC hdc, int fps);
};

#endif
