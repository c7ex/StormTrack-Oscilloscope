# StormTrack

**Read this in other languages:** [Русский](README.ru.md)

**StormTrack** is a C++ WinAPI library for real-time time series visualization on Windows using GDI. Its main purpose is to speed up R&D projects that need visualization in Visual Studio. The library works as a software oscilloscope: it can plot both static and streaming data. The rendering window runs in a separate thread, keeping the console responsive. Great performance even with a million data points.

![Interface example](screen/v1.6.0.gif) 

## Key Features

- **Multiple plots** in a single window.
- **Streaming updates** — data is continuously loaded in real time.
- **Static data** — medium-sized datasets (1M+).
- **Built‑in complex data support** — visualize I/Q signals with separate traces for real and imaginary parts using `std::complex<double>`.
- **Zoom** via mouse wheel (both XY or X-only with Shift held); zooming relative to cursor position.
- **Pan** (dragging) with left mouse button within the plot area.
- **Auto-fit on X** — the `A` hotkey toggles automatic adjustment of the visible area to match all active traces.
- **Auto-fit on Y** — the `S` hotkey toggles automatic scaling of the Y‑axis to fit all visible trace data.
- **Autotrack mode** — press `Q` to make the viewport automatically follow incoming data (the window «slides» to keep the latest points visible).
- **FPS display** — the `F` hotkey toggles the rendering performance overlay on/off.
- **Legend** — a list of traces with show/hide toggles (click the colored square).
- **Data tracking** — hovering over the plot highlights the nearest point and displays its coordinates.
- **Adaptive grid** with numeric labels.
- **Resizable plot area** — plot boundaries can be dragged.
- **Multithreading** — the window runs in its own thread without blocking the main console thread.
- **Smart rendering with anti‑aliasing artifacts elimination** — dynamic data compression and phase‑aligned binning ensure stable, flicker‑free panning even at extreme zoom levels.
- **Two streaming modes** — full frame replacement (`FrameView` with move semantics) or continuous appending (`RealtimeView` with copy), giving you full control over data ownership.
- **Zero external dependencies** — pure Win32/GDI, no Qt, no boost, no extra DLLs. Just include headers and link the static library.
- **Configurable UI appearance** — background colors, FPS counter visibility, and other visual parameters are exposed in a single `ConfigUI.hpp` header (requires rebuilding the project).

## Performance

All measurements below were taken with the frame time capped at **16 ms** (≈60 FPS),
which corresponds to the internal render timer interval.

> **Important:** the figures below describe the case when **all data points are rendered**.
> In practice, FPS depends directly on the **visible portion** of the displayed data:
> the library only renders the points that fall inside the current viewport,
> so zooming in (reducing the visible range) improves performance,
> while zooming out (showing the full dataset) is the heaviest case.
> The values below therefore represent the **worst-case scenario** — a fully
> visible dataset with autoscaling enabled.

> **A note on the term "FPS":** two different metrics are often confused here.
> - **Render FPS (internal)** — the number of fully rendered frames the library
>   is able to produce per second *if it were not limited by the timer*. This value
>   shows the real computational headroom of the renderer, and it is exactly what
>   is measured in the table below.
> - **Actual FPS (on-screen)** — the number of frames the user actually sees in the
>   window. This value is capped by the internal render timer at **≈60 FPS**
>   (interval 16 ms), so it can never exceed this limit, no matter how fast the
>   renderer runs.
>
> In other words: if the *render FPS* is **above 60**, the user sees a smooth
> **60 FPS**. If the *render FPS* drops **below 60** (e.g. ~34 or ~19 in the table),
> the user sees exactly that lower value — the timer simply cannot keep up,
> and each frame takes longer than 16 ms to compute.

**Test hardware:** Intel Core i5-7300HQ (laptop CPU, 2017–2018).

| Data points | Traces | Render FPS | Actual FPS       |
|-------------|--------|------------|------------------|
| 100K        | 10     | ~87        | 60 (capped)      |
| 500K        | 10     | ~50        | ~50              |
| 1M          | 10     | ~34        | ~34              |
| 1M          | 1      | ~140       | 60 (capped)      |
| 5M          | 1      | ~65        | 60 (capped)      |
| 10M         | 1      | ~37        | ~37              |
| 20M         | 1      | ~19        | ~19              |

## Build Instructions

### Method 1 — Ready-to-use SDK (recommended)
1. Get the SDK. If a prebuilt SDK (.zip) is available in [Releases](https://github.com/c7ex/StormTrack-Oscilloscope/releases), download and extract it.
   Otherwise, open the solution `vs\StormTrack.sln` and build the project in Release configuration —
   `StormTrack.lib` and `StormTrack.hpp` will appear in `bin\x64\Release`.
2. Copy `StormTrack.lib` and `StormTrack.hpp` into your project.
3. In your Visual Studio project settings:
   - **C/C++ → General → Additional Include Directories** – specify the folder containing `StormTrack.hpp`.
   - **Linker → Input → Additional Dependencies** – add `StormTrack.lib`.
   - **Linker → General → Additional Library Directories** – specify the folder containing `StormTrack.lib`.
4. Include the header: `#include "StormTrack.hpp"`.

**Note:** the `Demo` project includes a usage example.

### Method 2 — CMake (Windows)
An alternative to the Visual Studio solution. Requires CMake 3.16+ and Visual Studio 2017+.
1. Run `cmake\build.cmd` — the Visual Studio version is detected automatically.
2. Files will appear in `bin\x64\Release\`:
   - `StormTrack.lib`
   - `StormTrack.hpp`
   - `StormTrackDemo.exe`
3. Intermediate build files (solution, VS projects) are placed in `cmake\build\`.

To clean build artifacts, run `cmake\clean.cmd`.

### Method 3 — Adding source files directly to your project
1. Create a console application in Visual Studio.
2. Add all source folders to your project: `GraphCore\`, `GraphModules\`, `RaiiWinApi\`, `StormTrack\`. Make sure to include all paths to `.hpp` and `.cpp` files.
3. If you want to add the icon, include the `res` folder (`ico2hpp.vbs` + `stormtrack.ico`) and add a pre-build command for icon generation. Example command: `cscript //nologo "$(ProjectDir)..\..\res\ico2hpp.vbs" "$(ProjectDir)..\..\res\stormtrack.ico" "$(ProjectDir)..\..\src\StormTrack"` — the key requirement is that the generated `StormTrackIconData.hpp` is visible to your project. As an alternative, take `StormTrackIconData.hpp` from the prebuilt SDK.
4. Build in Debug/Release.

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

The built SDK can also save ready-made builds for different toolsets (v143, v145).

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

### Complex Data (I/Q)

The library supports `std::complex<double>` out of the box. You can visualize the real and imaginary parts as separate traces using the same `FrameView` and `RealtimeView` methods.

```cpp
#include "StormTrack.hpp"

HINSTANCE hInstance = GetModuleHandle(nullptr);

// Initialize window
StormTrack window(hInstance, L"[Example] I/Q streaming");

// Add two traces: I (real) and Q (imag)
size_t traceI = window.AddTrace(L"I (real)", RGB(255, 100, 100));
size_t traceQ = window.AddTrace(L"Q (imag)", RGB(100, 100, 255));

window.Show();

// Frame replacement (full update)
for (;;) {
    std::vector<std::complex<double>> iq_data;
    // ... generate or receive I/Q data ...

    // Update both traces simultaneously
    window.FrameView(iq_data, traceI, traceQ);
}

window.Close();
window.WaitForClose();
```

### Unique API

A high-level API where you don't need to store `trace_index` — each trace is addressed by its unique name. If a trace with that name doesn't exist yet, it will be created; if it does, the data will be appended to it. Recommended for most use cases.

```cpp
#include <complex>
#include <vector>
#include "StormTrack.hpp"

int main() {
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    StormTrack window(hInstance, L"[Example] named API");
    window.Show();

    // Create a trace (or update parameters of an existing one)
    window.UniqueTrace(L"Data 1", RGB(255, 0, 0));

    // Streaming: create the trace if it doesn't exist, replace its data vector
    std::vector<double> data = /*...*/;
    window.UniqueStream(data, L"Data 1");

    // Accumulation: create the trace if it doesn't exist, append a data vector
    std::vector<double> chunk = /*...*/;
    window.UniquePushBack(chunk, L"Data 2", RGB(100, 100, 200));

    // Accumulation: create the trace if it doesn't exist, append a single data point
    window.UniquePushBack(3.14, L"Data 2");

    // Complex data: automatically creates two traces "Signal-Re" and "Signal-Im"; this is a streaming example
    std::vector<std::complex<double>> iq_data = /*...*/;
    window.UniqueStream(iq_data, L"Signal", RGB(0, 255, 0), RGB(0, 0, 255));

    // Not needed in this example — the program does not close the window by itself.
    // window.Close();

    // Wait until the user closes the window
    window.WaitForClose();

    return 0;
}
```

## API Reference

| Method | Description | Data ownership |
|--------|-------------|----------------|
| `JustView(data, name, color, step, offset)` | Load static plot once | Copies |
| `AddTrace(name, color, step, offset)` | Create trace, returns ID | — |
| `FrameView(data, traceId)` | Full frame replacement | Moves |
| `RealtimeView(data, traceId)` | Append data to trace end (vector) | Copies |
| `RealtimeView(value, traceId)` | Append a single value to trace end | Copies |
| `JustView(complex_data, caption_re, caption_im, color_re, color_im, step, offset)` | Load static complex data (separate traces for real and imaginary parts) | Copies |
| `FrameView(complex_data, trace_index_re, trace_index_im)` | Full frame replacement for complex data (updates both Re and Im traces at once) | Copies |
| `RealtimeView(complex_data, trace_index_re, trace_index_im)` | Append complex data block to the end of Re and Im traces | Copies |
| `RealtimeView(complex_value, trace_index_re, trace_index_im)` | Append a single complex value to the end of Re and Im traces | Copies |
| `Show()` | Open window in background thread | — |
| `Close()` | Send close signal to window | — |
| `WaitForClose()` | Block until window thread exits | — |
| `IsActive()` | Check if window is still open | — |
| `UniqueTrace(caption, color, step, offset)` | Create a trace by name or update an existing one | — |
| `UniqueStream(data, caption[, color, step, offset])` | Stream `std::vector<double>` to a named trace | Moves |
| `UniqueStream(complex_data, caption[, color_re, color_im, ...])` | Stream `std::vector<std::complex<double>>` — creates `caption-Re` and `caption-Im` traces | Copies |
| `UniquePushBack(data, caption[, color, step, offset])` | Append a `std::vector<double>` to a named trace | Copies |
| `UniquePushBack(point, caption[, color, ...])` | Append a single `double` value to a named trace | Copies |
| `UniquePushBack(complex_data, caption[, color_re, color_im, ...])` | Append a block of complex data to `-Re` and `-Im` traces | Copies |
| `UniquePushBack(complex_point, caption[, color_re, color_im, ...])` | Append a single complex value to `-Re` and `-Im` traces | Copies |

## Controls

| Action | Control |
|--------|---------|
| Zoom | Mouse wheel. Default — both X and Y. With `Shift` held — X only. With `Ctrl` held — fast zoom. Zoom is centered on cursor position. |
| Pan | Hold left mouse button over the plot area and drag. |
| Auto-fit X | `A` key (toggles on/off). When enabled, the visible area automatically adjusts to cover the full X range of all active traces. |
| Auto-fit Y | `S` key (toggles on/off). When enabled, the Y‑axis automatically scales to fit all visible trace data. |
| FPS display | `F` key (toggles on/off). When enabled, displays the number of rendered frames per second. |
| Toggle autotrack mode (window follows incoming data) | `Q` key |
| Legend (show/hide trace) | Click the colored square in the right panel. The trace is temporarily hidden or shown again. |
| Coordinate tracking | Hover over the plot — the nearest data point is highlighted, and a tooltip with its coordinates appears near the cursor. |
| Plot area resize | Move the cursor to the edge of the dark border (a double-sided arrow will appear) and drag the boundary. Expands or collapses the legend panel. |

## Requirements

**Using the SDK**
- Windows 7 or later
- A C++17-compatible compiler
- No external libraries: only standard `kernel32`, `user32`, `gdi32`

**Building with CMake**
- CMake 3.16+
- Visual Studio 2017+

## License
MIT License. See the `LICENSE` file in the repository root.