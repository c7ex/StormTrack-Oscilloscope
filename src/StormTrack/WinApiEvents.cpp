#include "StormTrack.hpp"

LRESULT StormTrack::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_SIZE: {
        graphState.HandlerChangeSizeWindow(lParam);

        int width = LOWORD(lParam);
        int height = HIWORD(lParam);

        if (width > 0 && height > 0) {
            if (hBackBuffer) DeleteObject(hBackBuffer);
            HDC hdc = GetDC(hwnd);
            hBackBuffer = CreateCompatibleBitmap(hdc, width, height);
            ReleaseDC(hwnd, hdc);
            bufferWidth = width;
            bufferHeight = height;
        }

        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT clientRect;
        GetClientRect(hwnd, &clientRect);

        if (hBackBuffer) {
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, hBackBuffer);

            HBRUSH blackBrush = CreateSolidBrush(ConfigUI::GeneralGraph::boundary);
            FillRect(memDC, &clientRect, blackBrush);
            DeleteObject(blackBrush);

            graphState.Render(memDC);

            BitBlt(hdc, 0, 0, bufferWidth, bufferHeight, memDC, 0, 0, SRCCOPY);

            SelectObject(memDC, oldBmp);
            DeleteDC(memDC);
        }

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_ERASEBKGND: {
        return 1;
    }

    case WM_MOUSEMOVE: {
        // tracking leave mouse
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(TRACKMOUSEEVENT);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = hwnd;
        TrackMouseEvent(&tme);

        graphState.HandlerMouseMove(lParam);

        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }

    case WM_LBUTTONDOWN: {
        graphState.HandlerLButtonDown(lParam, hwnd);
        return 0;
    }

    case WM_LBUTTONUP: {
        graphState.HandlerLButtonUp(lParam);
        return 0;
    }

    case WM_MOUSEWHEEL: {
        graphState.HandlerMouseWheel(lParam, wParam, hwnd);
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }

    case WM_MOUSELEAVE: {
        graphState.HandlerMouseLeave(lParam);
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }

    case WM_SETCURSOR: {
        if (LOWORD(lParam) == HTCLIENT)
        {
            bool dataTrackerIsActive = graphState.ExtractFlagOfDataTrack();
            if (dataTrackerIsActive) {
                if (graphState.CursorMonitor(CursorType::_HIDDEN)) SetCursor(NULL);
                return TRUE;
            }

            bool plotResizerIsActive = graphState.ExtractFlagOfPlotResize();
            if (!plotResizerIsActive) {
                CursorType ct = graphState.ExtractCursorType();
                
                if (ct == CursorType::_ARROW) SetCursor(LoadCursor(NULL, IDC_ARROW));
                else if (ct == CursorType::_CROSS) SetCursor(LoadCursor(NULL, IDC_CROSS));
                else if (ct == CursorType::_WE) SetCursor(LoadCursor(NULL, IDC_SIZEWE));
                else if (ct == CursorType::_NS) SetCursor(LoadCursor(NULL, IDC_SIZENS));
                else if (ct == CursorType::_NWSE) SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
                else if (ct == CursorType::_NESW) SetCursor(LoadCursor(NULL, IDC_SIZENESW));
            }
            
            return TRUE;
        }
        break;
    }

    case WM_KEYDOWN: {
        graphState.HandlerKeyDown(wParam);
        return 0;
    }


    case WM_KEYUP: {
        graphState.HandlerKeyUp(wParam);
        return 0;
    }

    case WM_DESTROY: {
        if (hBackBuffer) {
            DeleteObject(hBackBuffer);
            hBackBuffer = nullptr;
        }
        PostQuitMessage(0);
        KillTimer(hwnd, TimerId);
        windowClosed = true;
        return 0;
    }

    case WM_TIMER: {
        if (graphState.PlotAreaActiveStatus()) SendMessage(hwnd, WM_SETCURSOR, (WPARAM)hwnd, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}