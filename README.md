# StormTrack

**Read this in other languages:** [Русский](README.ru.md)

**StormTrack** — is a C++ WinAPI library for real-time time series visualization on Windows using GDI. Its main purpose is to speed up R&D projects that need visualization in Visual Studio. The library works as a software oscilloscope: it can plot both static and streaming data. The rendering window runs in a separate thread, keeping the console responsive. Great performance even with a million data points.

![Interface example](screen/Demo.gif) 

## Key Features

- **Multiple plots** in a single window.
- **Streaming updates** — data is continuously loaded in real time.
- **Static data** — medium-sized datasets (1M+).
- **Zoom** via mouse wheel (both XY or X-only with Shift held); zooming relative to cursor position.
- **Pan** (dragging) with left mouse button within the plot area.
- **Auto-fit on X** — the `A` hotkey toggles automatic adjustment of the visible area to match all active traces.
- **FPS display** — the `F` hotkey toggles the rendering performance overlay on/off.
- **Legend** — a list of traces with show/hide toggles (click the colored square).
- **Data tracking** — hovering over the plot highlights the nearest point and displays its coordinates.
- **Adaptive grid** with numeric labels.
- **Resizable plot area** — plot boundaries can be dragged.
- **Multithreading** — the window runs in its own thread without blocking the main console thread.

## Build Instructions

### Method 1 — Ready-to-use SDK (recommended)
1. Get the SDK. If a prebuilt SDK (.zip) is available in [Releases](https://github.com/c7ex/StormTrack-Oscilloscope/releases), download and extract it.
   Otherwise, open the solution and build the `StormTrack` project in Release configuration —
   `StormTrack.lib` and the `StormTrackHeaders` folder will appear in `bin/x64/Release`.
2. Copy the `StormTrackHeaders` folder and `StormTrack.lib` into your project.
3. In your Visual Studio project settings:
   - **C/C++ → General → Additional Include Directories** – add the path to the `StormTrackHeaders` folder.
   - **Linker → Input → Additional Dependencies** – add `StormTrack.lib`.
   - **Linker → General → Additional Library Directories** – specify the folder containing `StormTrack.lib`.
4. Include the header: `#include "StormTrack.hpp"`.

**Note:** the `Demo` project includes a usage example.

### Method 2 — Adding source files directly to your project
1. Create a console application in Visual Studio.
2. Add all source folders to your project: `GraphCore/`, `GraphModules/`, `RaiiWinApi/`, `StormTrack/`. Make sure to include all paths to `.hpp` and `.cpp` files.
3. Build in Debug/Release.

**Note:** if you encounter error C1010, disable precompiled headers in your project settings.

> **Compiler toolset compatibility**  
>  
> The prebuilt `StormTrack.lib` in the SDK is compiled with **Platform Toolset v143** (Visual Studio 2022).  
> If your project uses a newer toolset (e.g. **v145** from Visual Studio 2026 Preview), you may encounter linker errors (`LNK2038` or similar) due to ABI mismatches.  
>  
> **To resolve this:**  
> - **Option 1 (recommended):** Switch your project’s Platform Toolset to **v143** (or any version between v140 and v143). This ensures binary compatibility without rebuilding the SDK.  
> - **Option 2:** Rebuild `StormTrack.lib` yourself using your exact toolset. Download the source code, open the solution, change the project’s Platform Toolset to your version (e.g. v145), and build in Debug/Release configuration.

Your console application will then be ready to use the visualization.

## Quick Start

### Static Plot

```cpp
#include "StormTrack.hpp"

std::vector<double> data = // ...

HINSTANCE hInstance = GetModuleHandle(nullptr);
StormTrack window(hInstance, L"[Example] static data");
window.JustView(data, L"Data", RGB(255, 120, 120));
window.Show();

// ...

window.Close();
window.WaitForClose();
```

### Frame Streaming

Full data replacement every frame. The vector is moved — becomes empty after call.

```cpp
#include "StormTrack.hpp"

HINSTANCE hInstance = GetModuleHandle(nullptr);
StormTrack window(hInstance, L"[Example] frame streaming");
window.Show();

size_t traceId = window.AddTrace(L"Data", RGB(255, 120, 120));

for (;;) {
    std::vector<double> data = // ...
    window.FrameView(data, traceId); // data is moved, becomes empty
}

window.Close();
window.WaitForClose();
```

### Real-Time Accumulation

Appends data to the end of trace. The original vector is preserved.

```cpp
#include "StormTrack.hpp"

HINSTANCE hInstance = GetModuleHandle(nullptr);
StormTrack window(hInstance, L"[Example] real-time data");
window.Show();

size_t traceId = window.AddTrace(L"Data", RGB(255, 120, 120));

for (;;) {
    std::vector<double> chunk = // ...
    window.RealtimeView(chunk, traceId); // chunk is copied, stays intact
}

window.Close();
window.WaitForClose();
```

## API Reference

| Method | Description | Data ownership |
|--------|-------------|----------------|
| `JustView(data, name, color, step, offset)` | Load static plot once | Copies |
| `AddTrace(name, color, step, offset)` | Create trace, returns ID | — |
| `FrameView(data, traceId)` | Full frame replace | Moves |
| `RealtimeView(data, traceId)` | Append data to trace end | Copies |
| `Show()` | Open window in background thread | — |
| `Close()` | Send close signal to window | — |
| `WaitForClose()` | Block until window thread exits | — |
| `IsActive()` | Check if window is still open | — |

## Controls

| Action | Control |
|--------|---------|
| Zoom | Mouse wheel. Default — both X and Y. With `Shift` held — X only. With `Ctrl` held — fast zoom. Zoom is centered on cursor position. |
| Pan | Hold left mouse button over the plot area and drag. |
| Auto-fit X | `A` key (toggles on/off). When enabled, the visible area automatically adjusts to cover the full X range of all active traces. |
| FPS display | The `F` key (toggles on/off). When enabled, displays the number of rendered frames per second. |
| Legend (show/hide trace) | Click the colored square in the right panel. The trace is temporarily hidden or shown again. |
| Coordinate tracking | Hover over the plot — the nearest data point is highlighted, and a tooltip with its coordinates appears near the cursor. |
| Plot area resize | Move the cursor to the edge of the dark border (a double-sided arrow will appear) and drag the boundary. Expands or collapses the legend panel. |

## Requirements
- Windows XP or later
- A C++11-compatible compiler (Visual Studio 2015+, MinGW-w64 with GCC 5+)
- No external libraries: only standard `kernel32`, `user32`, `gdi32`

## License
MIT License. See the `LICENSE` file in the repository root.


