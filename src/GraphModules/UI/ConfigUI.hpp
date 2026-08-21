#ifndef CONFIG_UI_HPP
#define CONFIG_UI_HPP

#include"Windows.h"

class ConfigUI {
public:
    class LegendItem {
    public:
        static constexpr int item_size = 15;
        static constexpr int padding_start_x = 10;
        static constexpr int padding_start_y = 10;
        static constexpr int spacing = 5;
        static constexpr int right_offset = 10;
        static constexpr int text_freespace = 50;
        static constexpr int step_checkbox_y = 25;

        static constexpr COLORREF background = RGB(30, 20, 40);
        static constexpr COLORREF pen_frame = RGB(60, 60, 65);
        static constexpr COLORREF unactive_checkbox = RGB(0, 0, 0);
        static constexpr COLORREF captions_color = RGB(200, 200, 200);
    };

    class DataTracker {
    public:
        static constexpr int radiusTrackDataInPixels = 8;
        static constexpr int maximalSearchCountPounts = 1024;

        static constexpr int padding = 3;
        static constexpr int offsetX = 8;
        static constexpr int offsetY = 8;

        static constexpr COLORREF textColor = RGB(0, 0, 0);
        static constexpr COLORREF borderColor = RGB(0, 0, 0);
    };

    class Zoom {
    public:
        static constexpr double default_scale_factor = 1.07;
        static constexpr double fast_scale_factor = 1.2;

        static constexpr double min_scale = 1e-3;
        static constexpr double max_scale = 1e13;
    };

    class Plot {
    public:
        static constexpr int width_board = 3;
        static constexpr double minumal_x = 0.25;
        static constexpr double minumal_y = 0.25;
    };

    class Margins {
    public:
        static constexpr double clip_scale_x = 0.03;
        static constexpr double clip_scale_y = 0.03;
    };

    class Fps {
	public:		
		static constexpr bool default_active = true;
        static constexpr size_t default_fps = 60;
        static constexpr size_t default_window_smoov_fps = 64;
    };

    class AutoScaler {
    public:
		static constexpr bool default_autoscaler_x_active = true;
    };
};

#endif