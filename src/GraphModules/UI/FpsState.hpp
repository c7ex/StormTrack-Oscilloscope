#ifndef FPS_STATE_HPP
#define FPS_STATE_HPP

#include"WindowState.hpp"
#include"GraphContext.hpp"

enum UserKeys;
class WindowState;
class GraphContext;

class FpsState {
private:
    double average_fps = 60;
    const size_t window_smoov_fps = 64;
    
private:
    bool active = false;
    bool holder = false;

public:
    void SwitchActive(const WindowState& window);
	void Draw(GraphContext& context, HDC hdc, int fps);
};

#endif
