#ifndef CONFIG_UI_HPP
#define CONFIG_UI_HPP

#include"Windows.h"

class ConfigUI {
public:
    class GeneralGraph {
    public:
        static constexpr COLORREF background = RGB(10, 14, 23);
        static constexpr COLORREF boundary = RGB(8, 0, 15);

        static constexpr int base_margins_left = 65;
        static constexpr int base_margins_top = 35;
        static constexpr int base_margins_right = 240;
        static constexpr int base_margins_bottom = 45;
    };

    class Ruler {
    public:
        static constexpr double alpha_gauss_ratio = 0.5;
        static constexpr double alpha_threshold_visible = 0.05;

        static constexpr int max_count_grid_lines = 70;
        static constexpr double optimal_count_grid_lines = 14;
        static constexpr double accuracy_grid_draw = 1e-10;
        static constexpr int grid_line_width = 1;

        static constexpr int caption_offset_x = 5;
        static constexpr int caption_offset_y = 25;

        static constexpr COLORREF caption_main_color = RGB(230, 230, 230);
        static constexpr COLORREF caption_sub_color = RGB(230, 230, 230);

        static constexpr COLORREF grid_main_color = RGB(130, 130, 130);
        static constexpr COLORREF grid_sub_color = RGB(130, 130, 130);

        static constexpr COLORREF border_active_color = RGB(15, 15, 15);
        static constexpr COLORREF border_unactive_color = RGB(50, 50, 50);

        static constexpr double transparent_color_min = 0.0;
        static constexpr double transparent_color_max = 1.0;

        static constexpr double text_format_scientific_threshold = 0.001;
        static constexpr double text_format_large_threshold = 1000.0;
        static constexpr double text_format_zero_epsilon = 1e-12;

        static constexpr double grid_calculator_base = 10.0;
        static constexpr double grid_calculator_subdivision = 10.0;
    };

    class LegendItem {
    public:
        static constexpr double background_right_padding = 0.03; // m.b. not like margins
        static constexpr double background_left_padding = 0.03; // like margins

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
        static constexpr int radiusTrackDataInPixels = 7;
        static constexpr int maximalSearchCountPounts = 1024;

        static constexpr int padding = 3;
        static constexpr int offsetX = 8;
        static constexpr int offsetY = 8;

        static constexpr COLORREF textColor = RGB(0, 0, 0);
        static constexpr COLORREF borderColor = RGB(0, 0, 0);
		static constexpr COLORREF freefly_backgroundColor = RGB(255, 255, 200);
        static constexpr COLORREF warning_backgroundColor = RGB(255, 30, 70);
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
        static constexpr int width_board = 4;
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
		static constexpr bool default_active = false;
        static constexpr size_t default_fps = 60;
        static constexpr size_t default_window_smoov_fps = 64;
    };

    class AutoScaler {
    public:
        static constexpr bool default_autoscaler_track_active = false;
        static constexpr bool default_autoscaler_x_active = true;
        static constexpr bool default_autoscaler_y_active = true;
        static constexpr double default_track_visible_area_x = 100.0;
        static constexpr double default_singularity_case_x_range_min = -0.5;
        static constexpr double default_singularity_case_x_range_max = 0.5;
        static constexpr double default_singularity_case_y_range_min = -0.5;
        static constexpr double default_singularity_case_y_range_max = 0.5;
        static constexpr double padding_scale_x = 0.04;
        static constexpr double padding_scale_y = 0.04;
    };

    class BindHotKeys {
    public:
        static constexpr int toggle_fast_zoom = VK_CONTROL;
        static constexpr int toggle_x_mode_zoom = VK_SHIFT;
        static constexpr int toggle_autoscale_track = 'Q';
        static constexpr int toggle_autoscale_x = 'A';
        static constexpr int toggle_autoscale_y = 'S';
        static constexpr int toggle_visible_fps = 'F';
    };
};

#endif