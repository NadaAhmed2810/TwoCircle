// TwoCircle.cpp : Defines the entry point for the application.

#include "framework.h"
#include "TwoCircle.h"
#include <vector>
#include <algorithm>
#include <cmath>
#define MAX_LOADSTRING 100

using namespace std;


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
vector<POINT> Points;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TWOCIRCLE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow)) {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TWOCIRCLE));

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TWOCIRCLE));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TWOCIRCLE);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd) {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}
void drawLineBresenham(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    int sx = (x2 > x1) ? 1 : -1;
    int sy = (y2 > y1) ? 1 : -1;

    bool isSteep = dy > dx;

    if (isSteep) {
        swap(x1, y1);
        swap(x2, y2);
        swap(dx, dy);
        swap(sx, sy);
    }

    int d = 2 * dy - dx;
    int x = x1;
    int y = y1;

    for (int i = 0; i <= dx; i++) {
        if (isSteep)
            SetPixel(hdc, y, x, color);
        else
            SetPixel(hdc, x, y, color);

        x += sx;
        if (d > 0) {
            y += sy;
            d -= 2 * dx;
        }
        d += 2 * dy;
    }
}

void drawDiagonalsInSectors(HDC hdc, int xc, int yc, int r, COLORREF color) {
    const double PI = 3.14159265358979323846;
    const double angleStep = PI / 180; 

    
    for (double angle = 0; angle <= PI / 4; angle += angleStep) {
        int x = (xc + r * cos(angle));
        int y = (yc - r * sin(angle));
        drawLineBresenham(hdc, xc, yc, x, y, color);
    }

  
    for (double angle = PI; angle <= 5 * PI / 4; angle += angleStep) {
        int x = (xc + r * cos(angle));
        int y = (yc - r * sin(angle));
        drawLineBresenham(hdc, xc, yc, x, y, color);
    }
}

void draw8Points(HDC hdc, int xc, int yc, int x, int y, COLORREF REF) {
    SetPixel(hdc, xc + x, yc + y, REF);
    SetPixel(hdc, xc - x, yc + y, REF);
    SetPixel(hdc, xc + x, yc - y, REF);
    SetPixel(hdc, xc - x, yc - y, REF);
    SetPixel(hdc, xc + y, yc + x, REF);
    SetPixel(hdc, xc - y, yc + x, REF);
    SetPixel(hdc, xc + y, yc - x, REF);
    SetPixel(hdc, xc - y, yc - x, REF);

}

void drawBresenhamCircle(HDC hdc, int xc, int yc, int r, COLORREF REF) {
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    draw8Points(hdc, xc, yc, x, y, REF);
    while (x <= y) {
        x++;
        if (d < 0)
            d = d + 4 * x + 6;
        else {
            y--;
            d = d + 4 * (x - y) + 10;
        }
        draw8Points(hdc, xc, yc, x, y, REF);
    }
}

void DrawTwoCircle(HDC hdc) {
    if (Points.size() < 3) return;
	//sort(Points.begin(),Points.end()); I cant because point not implement i comparer

    int x1 = Points[0].x, y1 = Points[0].y;
    int x2 = Points[1].x, y2 = Points[1].y;
    int x3 = Points[2].x, y3 = Points[2].y;

    int r1 = (int)hypot(x2 - x1, y2 - y1);
    int r2 = (int)hypot(x3 - x2, y3 - y2);

    drawBresenhamCircle(hdc, x1, y1, r1, RGB(255, 0, 0));
    drawBresenhamCircle(hdc, x3, y3, r2, RGB(0, 0, 255));
    drawDiagonalsInSectors(hdc, x3, y3, r2, RGB(180, 155, 200));
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_LBUTTONDOWN: {
        if (Points.size() < 3) {
            POINT p = { LOWORD(lParam), HIWORD(lParam) };
            Points.push_back(p);
        }
        if (Points.size() == 3)
            InvalidateRect(hWnd, NULL, TRUE);
        break;
    }
    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        DrawTwoCircle(hdc);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
