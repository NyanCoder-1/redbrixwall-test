#define WIN32_LEAN_AND_MEAN
// I prefer using std::min/std::max from <algorithm>
#define NOMINMAX
#include <Windows.h>
#include <iostream>
#include <memory>
#include "Render.h"

#include "Text.h"
#include "FieldMesh.h"

const wchar_t* wndClassName = L"redbrixwall-test[window]";
const wchar_t* wndTitle = L"redbrixwall-test: The Archers";
HWND hWnd = nullptr;
std::unique_ptr<Render> render;

bool PrepareWindow();

int main(int argc, char** argv)
{
	if (!PrepareWindow())
	{
		std::cout << "Could't create window\n";
		return 1;
	}
    render = std::make_unique<Render>();
    if (!render->Init(hWnd))
    {
        std::cout << "Could't create renderer\n";
        return 1;
    }

    FieldMesh fieldMesh(render.get());
    Text text(render.get(), U"Test text", false);
    text.SetPosition(500, 30);

    MSG msg = { 0 };
    // Main Loop
    while (WM_QUIT != msg.message)
    {
        if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        else
        {
            render->Draw(
                [&](Render* render) {
                    fieldMesh.Draw();
                },
                [&](Render* render) {
                    text.Draw();
                });
        }
    }

	return 0;
}

void RegisterWndClass();
LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

bool PrepareWindow()
{
	RegisterWndClass();

    auto wndStyle = (WS_OVERLAPPEDWINDOW & ~(WS_SIZEBOX | WS_MAXIMIZEBOX)) | WS_VISIBLE;
    RECT rc = { 0, 0, 1280, 720 };
    AdjustWindowRect(&rc, wndStyle, FALSE);
	hWnd = CreateWindowExW(0L, wndClassName, wndTitle, wndStyle, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, nullptr, nullptr);
    return hWnd;
}

void RegisterWndClass()
{
    WNDCLASSEXW wcex;
    memset(&wcex, 0, sizeof(WNDCLASSEX));
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = wndClassName;
    RegisterClassExW(&wcex);
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch (Message)
    {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hDC = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcW(hWnd, Message, wParam, lParam);
    }
    return 0;
}