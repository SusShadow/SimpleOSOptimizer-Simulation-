#define UNICODE
#define _UNICODE
#pragma comment(lib, "Comctl32.lib")
#include <windows.h>
#include <commctrl.h>

// Colors
#define CLR_BG          RGB(15, 15, 25)
#define CLR_PANEL       RGB(20, 20, 38)
#define CLR_BTN         RGB(25, 60, 120)
#define CLR_BTN_HOVER   RGB(35, 90, 180)
#define CLR_BTN_PRESS   RGB(15, 40, 90)
#define CLR_ACCENT      RGB(50, 120, 220)
#define CLR_TEXT        RGB(200, 220, 255)
#define CLR_TITLE       RGB(80, 160, 255)

// IDs
#define ID_BTN1 101
#define ID_BTN2 102
#define ID_BTN3 103
#define ID_BTN4 104

// Globals
HFONT hFontUI    = NULL;
HFONT hFontTitle = NULL;

bool btnHover[4] = {};
bool btnPress[4] = {};
HWND hBtns[4]   = {};

HFONT CreateUIFont(int size, bool bold)
{
    return CreateFont(
        size, 0, 0, 0,
        bold ? FW_BOLD : FW_NORMAL,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_SWISS,
        L"Segoe UI"
    );
}

void DrawRoundRect(HDC hdc, RECT r, int radius, COLORREF fill, COLORREF stroke, bool drawStroke)
{
    HBRUSH br = CreateSolidBrush(fill);
    HPEN   pn = drawStroke ? CreatePen(PS_SOLID, 1, stroke) : (HPEN)GetStockObject(NULL_PEN);
    HBRUSH oldBr = (HBRUSH)SelectObject(hdc, br);
    HPEN   oldPn = (HPEN)SelectObject(hdc, pn);
    RoundRect(hdc, r.left, r.top, r.right, r.bottom, radius, radius);
    SelectObject(hdc, oldBr);
    SelectObject(hdc, oldPn);
    DeleteObject(br);
    DeleteObject(pn);
}

LRESULT CALLBACK BtnProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, UINT_PTR uId, DWORD_PTR)
{
    int idx = (int)(uId - ID_BTN1);

    switch (msg)
    {
        case WM_MOUSEMOVE:
        {
            if (!btnHover[idx]) {
                btnHover[idx] = true;
                TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 };
                TrackMouseEvent(&tme);
                InvalidateRect(hwnd, NULL, FALSE);
            }
            break;
        }

        case WM_MOUSELEAVE:
        {
            btnHover[idx] = false;
            btnPress[idx] = false;
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        }

        case WM_LBUTTONDOWN:
        {
            btnPress[idx] = true;
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        }

        case WM_LBUTTONUP:
        {
            if (btnPress[idx]) {
                btnPress[idx] = false;
                InvalidateRect(hwnd, NULL, FALSE);
                SendMessage(GetParent(hwnd), WM_COMMAND,
                    MAKEWPARAM((UINT)(uId), BN_CLICKED), (LPARAM)hwnd);
            }
            break;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rc;
            GetClientRect(hwnd, &rc);

            // Erase with window background color
            HBRUSH bgBrush = CreateSolidBrush(CLR_BG);
            FillRect(hdc, &rc, bgBrush);
            DeleteObject(bgBrush);

            // Button fill
            COLORREF fill;
            if (btnPress[idx])      fill = CLR_BTN_PRESS;
            else if (btnHover[idx]) fill = CLR_BTN_HOVER;
            else                    fill = CLR_BTN;

            DrawRoundRect(hdc, rc, 10, fill, CLR_ACCENT, true);

            // Label
            wchar_t text[64];
            GetWindowText(hwnd, text, 64);
            SelectObject(hdc, hFontUI);
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, CLR_TEXT);
            DrawText(hdc, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;
    }

    return DefSubclassProc(hwnd, msg, wp, lp);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            hFontUI    = CreateUIFont(-16, false);
            hFontTitle = CreateUIFont(-22, true);

            const wchar_t* labels[4] = {
                L"Action 1", L"Action 2", L"Action 3", L"Action 4"
            };

            for (int i = 0; i < 4; i++) {
                hBtns[i] = CreateWindowEx(
                    0, L"BUTTON", labels[i],
                    WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                    0, 0, 10, 10,
                    hwnd, (HMENU)(INT_PTR)(ID_BTN1 + i), NULL, NULL
                );
                SendMessage(hBtns[i], WM_SETFONT, (WPARAM)hFontUI, TRUE);
                SetWindowSubclass(hBtns[i], BtnProc, ID_BTN1 + i, 0);
            }
            break;
        }

        case WM_SIZE:
        {
            int W = LOWORD(lParam), H = HIWORD(lParam);
            int btnW = 260, btnH = 52, gap = 18;
            int totalH = 4 * btnH + 3 * gap;
            int startY = (H - totalH) / 2 + 30;
            int x = (W - btnW) / 2;

            for (int i = 0; i < 4; i++) {
                int y = startY + i * (btnH + gap);
                SetWindowPos(hBtns[i], NULL, x, y, btnW, btnH, SWP_NOZORDER);
            }
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        }

        case WM_COMMAND:
        {
            int id = LOWORD(wParam);
            if (id >= ID_BTN1 && id <= ID_BTN4) {
                wchar_t msgText[64];
                wsprintf(msgText, L"Action %d triggered!", id - ID_BTN1 + 1);
                MessageBox(hwnd, msgText, L"CoreTune", MB_OK | MB_ICONINFORMATION);
            }
            break;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rc;
            GetClientRect(hwnd, &rc);

            // Background
            HBRUSH bgBrush = CreateSolidBrush(CLR_BG);
            FillRect(hdc, &rc, bgBrush);
            DeleteObject(bgBrush);

            // Top accent bar
            RECT bar = { rc.left, rc.top, rc.right, rc.top + 3 };
            HBRUSH accentBrush = CreateSolidBrush(CLR_ACCENT);
            FillRect(hdc, &bar, accentBrush);
            DeleteObject(accentBrush);

            // Title
            SelectObject(hdc, hFontTitle);
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, CLR_TITLE);
            RECT titleRect = { 0, 14, rc.right, 60 };
            DrawText(hdc, L"CoreTune", -1, &titleRect, DT_CENTER | DT_TOP | DT_SINGLELINE);

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;

        case WM_DESTROY:
        {
            DeleteObject(hFontUI);
            DeleteObject(hFontTitle);
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    WNDCLASSEX wc      = {};
    wc.cbSize          = sizeof(WNDCLASSEX);
    wc.lpfnWndProc     = WndProc;
    wc.hInstance       = hInstance;
    wc.hCursor         = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground   = CreateSolidBrush(CLR_BG);  // ← dark background, not white
    wc.lpszClassName   = L"OptimizerWindow";
    wc.style           = CS_HREDRAW | CS_VREDRAW;
    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(
        WS_EX_APPWINDOW,
        L"OptimizerWindow",
        L"CoreTune",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 900, 600,
        NULL, NULL, hInstance, NULL
    );

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}