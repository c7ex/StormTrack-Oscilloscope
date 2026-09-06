#include"ZoomState.hpp"

#define CURSORZOOM

void ZoomState::ApplyZoom(
    GraphContext& context,
    const TransformCoordinates& coreEngine,
    const TugboatState& ts,
    ScaleDirection direction,
    bool fast_zoom,
    bool mode_axes_X,
    bool mode_track)
{
    if (ts.IsActive()) return;   

    Size2d visible_area = context.GetVisibleArea();

#ifdef CURSORZOOM
    const Position2d mouse_position = context.GetMousePosition();
    const Position2d reference = context.GetReferencePosition();
    const Position2d saved_world_pos = coreEngine.ConvertToWorldCoords(mouse_position.x, mouse_position.y);
#else
    const Size2d saved_visible_area = visible_area;
#endif

    auto scale_factor = fast_zoom ? 
        ConfigUI::Zoom::fast_scale_factor : 
        ConfigUI::Zoom::default_scale_factor;

    if (mode_axes_X) {
        if (mode_track) {
			// only track X
            double track_visible_area_x = context.GetTrackVisibleAreaX();
            if (direction == ScaleDirection::ZoomIn) { track_visible_area_x /= scale_factor; }
            else { track_visible_area_x *= scale_factor; }
            context.SetTrackVisibleAreaX(track_visible_area_x);
            return;
        }
        else {
            // only X
            if (direction == ScaleDirection::ZoomIn) { visible_area.x /= scale_factor; }
            else { visible_area.x *= scale_factor; }
        }
    }
    else {
		// X/Y-autoscaling
        if (direction == ScaleDirection::ZoomIn) { visible_area /= scale_factor; }
        else { visible_area *= scale_factor; }
    }

    // board of scaling
    // need change with 1e-3 to ...?  after solving problem with small numeric
    visible_area.x = std::clamp(visible_area.x, static_cast<double>(ConfigUI::Zoom::min_scale), static_cast<double>(ConfigUI::Zoom::max_scale));
    visible_area.y = std::clamp(visible_area.y, static_cast<double>(ConfigUI::Zoom::min_scale), static_cast<double>(ConfigUI::Zoom::max_scale));

    context.SetVisibleArea(visible_area);

#ifdef CURSORZOOM
    const Position2d new_world_pos = coreEngine.ConvertToWorldCoords(mouse_position.x, mouse_position.y);
    const Offset2d pos_offset = saved_world_pos - new_world_pos;
    context.SetReferencePosition(reference + pos_offset);
#else
    const Offset2d center_offset = (saved_visible_area - visible_area) / 2.0;
    context.SetOffsetReferencePosition(center_offset);
#endif

    context.UpdateCoordinates(coreEngine);
}