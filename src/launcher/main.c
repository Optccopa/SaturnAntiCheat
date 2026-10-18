#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <strsafe.h>

#include "launcher/driver.h"

#define COL_BG RGB(216, 174, 109)
#define COL_ICON RGB(101, 95, 69)

#define COL_CLOSE_HOVER RGB(232, 17, 35)
#define COL_MIN_HOVER RGB(55, 55, 64)

static const wchar_t CLASS_NAME[] = L"saturnLauncher";
static const wchar_t LAUNCHER_NAME[] = L"Saturn Launcher";

enum { BTN_NONE, BTN_MIN, BTN_CLOSE };
enum { HEARTBEAT_TIMER = 1 };

static HEARTBEAT_STATUS g_heartbeat = HEARTBEAT_PENDING;

static const RECT HEARTBEAT_RECT = { 0, 0, 220, 32 };

static const struct { COLORREF col; const wchar_t *text; } HB[] = {
    [HEARTBEAT_SUCCESS] = { RGB(46, 160, 67), L"Driver OK" },
    [HEARTBEAT_FAILED] = { RGB(200, 40, 40), L"Driver not responding" },
    [HEARTBEAT_CS2_NOT_FOUND] = { RGB(220, 150, 20), L"CS2 Not found" },
    [HEARTBEAT_PENDING] = { RGB(120, 120, 120), L"Checking driver ..." },
};


static int g_hover = BTN_NONE;
static BOOL g_tracking = FALSE;

static BOOL g_closeHover = FALSE;
static BOOL g_minHover = FALSE;

static wchar_t status[256] = L"Starting launcher ...";

static void updateStatus(HWND hwnd, const wchar_t *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    StringCchVPrintfW(status, ARRAYSIZE(status), fmt, args);
    va_end(args);

    InvalidateRect(hwnd, NULL, TRUE);
    UpdateWindow(hwnd);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rc;
            GetClientRect(hwnd, &rc);

            if (g_closeHover) {
                RECT btn = { rc.right - 46, 0, rc.right, 32 };
                HBRUSH red = CreateSolidBrush(COL_CLOSE_HOVER);
                FillRect(hdc, &btn, red);
                DeleteObject(red);
            }

            if (g_minHover) {
                RECT btn = { rc.right - 92, 0, rc.right - 46, 32 };
                HBRUSH gray = CreateSolidBrush(COL_MIN_HOVER);
                FillRect(hdc, &btn, gray);
                DeleteObject(gray);
            }

            LOGBRUSH lb = { BS_SOLID, COL_ICON, 0 };
            HPEN pen = ExtCreatePen(PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_FLAT, 3, &lb, 0, NULL);
            HPEN old = SelectObject(hdc, pen);

            // close
            int cx = rc.right - 23, cy = 16;
            MoveToEx(hdc, cx - 5, cy - 5, NULL); LineTo(hdc, cx + 5, cy + 5);
            MoveToEx(hdc, cx + 5, cy - 5, NULL); LineTo(hdc, cx - 5, cy + 5);

            // minimize
            int mx = rc.right - 69;
            MoveToEx(hdc, mx - 5, cy, NULL); LineTo(hdc, mx + 5, cy);

            // loading status
            SetBkMode(hdc, TRANSPARENT);

            RECT tr = rc;
            DrawTextW(hdc, status, -1, &tr, DT_CENTER | DT_CALCRECT); // doesn't draw
            int h = tr.bottom - tr.top;

            RECT out = rc;
            out.top = (rc.bottom - h) / 2;
            out.bottom = out.top + h;
            DrawTextW(hdc, status, -1, &out, DT_CENTER);

            // heartbeat status
            HBRUSH dot = CreateSolidBrush(HB[g_heartbeat].col);
            HGDIOBJ prevBrush = SelectObject(hdc, dot);
            HGDIOBJ prevPen = SelectObject(hdc, GetStockObject(NULL_PEN));
            Ellipse(hdc, 12, 11, 23, 22);
            SelectObject(hdc, prevPen);
            SelectObject(hdc, prevBrush);
            DeleteObject(dot);

            RECT t = HEARTBEAT_RECT;
            t.left = 30;
            DrawTextW(hdc, HB[g_heartbeat].text, -1, &t, DT_SINGLELINE | DT_VCENTER | DT_LEFT);

            // cleanup
            SelectObject(hdc, old);
            DeleteObject(pen);

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_LBUTTONUP: {
            RECT rc;
            GetClientRect(hwnd, &rc);
            int x = GET_X_LPARAM(lp), y = GET_Y_LPARAM(lp);

            if (y < 32) {
                if (x >= rc.right - 46) {
                    PostMessageW(hwnd, WM_CLOSE, 0, 0);
                }
                else if (x >= rc.right - 92) {
                    ShowWindow(hwnd, SW_MINIMIZE);
                }
            }
            return 0;
        }

        case WM_NCHITTEST: {
            LRESULT hit = DefWindowProcW(hwnd, msg, wp, lp);
            POINT pt = { GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };
            RECT rc;
            ScreenToClient(hwnd, &pt);
            GetClientRect(hwnd, &rc);
            if (hit == HTCLIENT && !(pt.y < 32 && pt.x >= rc.right - 92)){
                return HTCAPTION;
            }
            return hit;
        }

        case WM_TIMER: {
            if (wp == HEARTBEAT_TIMER) {
                HEARTBEAT_STATUS hb = pingDriver();
                if (hb != g_heartbeat) {
                    g_heartbeat = hb;
                    InvalidateRect(hwnd, &HEARTBEAT_RECT, TRUE);
                }
            }
            return 0;
        }

        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        } 

        case WM_MOUSEMOVE: {
            if (!g_tracking) {
                TRACKMOUSEEVENT tme = { sizeof tme, TME_LEAVE, hwnd, 0 };
                TrackMouseEvent(&tme);
                g_tracking = TRUE;
            }

            RECT rc;
            GetClientRect(hwnd, &rc);
            int x = GET_X_LPARAM(lp), y = GET_Y_LPARAM(lp);
            BOOL overClose = (y < 32 && x >= rc.right - 46);
            BOOL overMin = (y < 32 && x >= rc.right - 92 && x < rc.right - 46);

            if (overClose != g_closeHover || overMin != g_minHover) {
                g_closeHover = overClose;
                g_minHover   = overMin;
                InvalidateRect(hwnd, NULL, TRUE);
            }
            return 0;
        }

        case WM_MOUSELEAVE: {
            g_tracking = FALSE;
            if (g_closeHover || g_minHover) {
                g_closeHover = FALSE;
                g_minHover   = FALSE;
                InvalidateRect(hwnd, NULL, TRUE);
            }
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);

    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursorW(NULL, MAKEINTRESOURCEW(32512));

    wc.hbrBackground = CreateSolidBrush(COL_BG);

    RegisterClassW(&wc);

    int w = 400, h = 240;
    int x = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
    int y = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;

    HWND hwnd = CreateWindowExW(
        0, CLASS_NAME, LAUNCHER_NAME,
        WS_POPUP | WS_MINIMIZEBOX,
        x, y, w, h,
        NULL, NULL, hInstance, NULL);

    
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    updateStatus(hwnd, L"Loading SaturnAntiCheatDriver.sys ...");

    wchar_t sysPath[MAX_PATH];
    GetModuleFileNameW(NULL, sysPath, MAX_PATH);

    wchar_t *slash = wcsrchr(sysPath, L'\\');
    if (slash) {
        wcscpy(slash + 1, L"SaturnAntiCheatDriver.sys");
    }

    DWORD err = startDriver(sysPath);
    if (err) {
        if (err == ERROR_INVALID_IMAGE_HASH) {
            updateStatus(hwnd, L"Test signing is not enabled\nPlease refer to README.md > Install > PC Setup");
        } else {
            updateStatus(hwnd, L"Driver failed to load (error %lu)", err);
        }
        
    } else {
        updateStatus(hwnd, L"Anticheat running ...");
        SetTimer(hwnd, HEARTBEAT_TIMER, 1000, NULL);

        INT_PTR r = (INT_PTR)ShellExecuteW(NULL, L"open",
            L"C:\\Program Files (x86)\\Steam\\steam.exe",
            L"-applaunch 730 -insecure", NULL, SW_SHOWNORMAL);
        if (r <= 32) {
            updateStatus(hwnd, L"Failed to launch game with -insecure, error: %i", (int)r);
        }
    }
    InvalidateRect(hwnd, NULL, TRUE);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    stopDriver();

    return 0;
}
