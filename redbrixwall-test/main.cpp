#define WIN32_LEAN_AND_MEAN
// I prefer using std::min/std::max from <algorithm>
#define NOMINMAX
#include <Windows.h>
#include <iostream>
#include <memory>
#include "Render.h"

#include "Text.h"
#include "FieldMesh.h"
#include "CylinderMesh.h"
#include "ShadowMesh.h"
#include "ArrowMesh.h"

#include "Logic.h"

#include <chrono>

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
    Text team1Name(render.get(), U"Team Red", false);
    Text team2Name(render.get(), U"Team Blue", false);
    Text team1Count(render.get(), U"1234567890", true);
    Text team2Count(render.get(), U"1234567890", true);
    team1Name.SetPosition(20, 41);
    team2Name.SetPosition(20, 71);
    auto maxWidth = std::max(team1Name.GetWidth(), team2Name.GetWidth());
    team1Count.SetPosition(50 + maxWidth, 41);
    team2Count.SetPosition(50 + maxWidth, 71);

    Logic logic;

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
            logic.Update();
            
            // Set team count text
            {
                auto team1View = logic.GetRegistry().view<const Tags::Team1>();
                std::u32string team1CountText;
                for (auto character : std::to_wstring(team1View.size()))
                    team1CountText.push_back(character);
                team1Count.SetText(team1CountText);
                auto team2View = logic.GetRegistry().view<const Tags::Team2>();
                std::u32string team2CountText;
                for (auto character : std::to_wstring(team2View.size()))
                    team2CountText.push_back(character);
                team2Count.SetText(team2CountText);
            }

            auto shadowView = logic.GetRegistry().view<const Position, ShadowMesh>();
            render->Draw(
                [&](Render* render) {
                    fieldMesh.Draw();

                    // draw shadows
                    render->DisableDepthStencil();
                    auto shadowView = logic.GetRegistry().view<const Position, ShadowMesh>();
                    for (auto [entityWithShadow, positionComponent, shadowComponent] : shadowView.each())
                    {
                        shadowComponent.SetPosition(positionComponent.x, positionComponent.y);
                        shadowComponent.Draw();
                    }

                    // draw archers
                    render->EnableDepthStencil();
                    auto archerView = logic.GetRegistry().view<const Archer, const Team, const Position, const ColliderCylinder>();
                    for (auto [archerEntity, archerComponent, teamComponent, positionComponent, colliderComponent] : archerView.each())
                    {
                        if (!logic.GetRegistry().try_get<ShadowMesh>(archerEntity))
                        {
                            auto& newShadowComponent = logic.GetRegistry().emplace<ShadowMesh>(archerEntity, render);
                            newShadowComponent.SetPosition(positionComponent.x, positionComponent.y);
                        }
                        if (auto meshComponent = logic.GetRegistry().try_get<CylinderMesh>(archerEntity))
                        {
                            meshComponent->SetPosition(positionComponent.x, positionComponent.y, positionComponent.z);
                            meshComponent->Draw();
                        }
                        else
                        {
                            auto& newMeshComponent = logic.GetRegistry().emplace<CylinderMesh>(archerEntity, render);
                            newMeshComponent.SetHeight(colliderComponent.height);
                            newMeshComponent.SetRadius(colliderComponent.radius);
                            newMeshComponent.SetColor(teamsColors[teamComponent]);
                            newMeshComponent.UpdateGeometry();
                            newMeshComponent.SetPosition(positionComponent.x, positionComponent.y, positionComponent.z);
                            newMeshComponent.Draw();
                        }
                    }

                    // draw arrows
                    render->DrawModeLines();
                    auto arrowView = logic.GetRegistry().view<const Tags::Arrow, const Position, const Velocity>();
                    for (auto [arrowEntity, positionComponent, velocityComponent] : arrowView.each())
                    {
                        if (!logic.GetRegistry().try_get<ShadowMesh>(arrowEntity))
                        {
                            auto& newShadowComponent = logic.GetRegistry().emplace<ShadowMesh>(arrowEntity, render);
                            newShadowComponent.SetPosition(positionComponent.x, positionComponent.y);
                        }
                        if (auto meshComponent = logic.GetRegistry().try_get<ArrowMesh>(arrowEntity))
                        {
                            meshComponent->SetPositionAndRotation(positionComponent.x, positionComponent.y, positionComponent.z,
                                positionComponent.x + velocityComponent.dX, positionComponent.y + velocityComponent.dY, positionComponent.z + velocityComponent.dZ);
                            meshComponent->Draw();
                        }
                        else
                        {
                            auto& newMeshComponent = logic.GetRegistry().emplace<ArrowMesh>(arrowEntity, render);
                            newMeshComponent.SetPositionAndRotation(positionComponent.x, positionComponent.y, positionComponent.z,
                                positionComponent.x + velocityComponent.dX, positionComponent.y + velocityComponent.dY, positionComponent.z + velocityComponent.dZ);
                            newMeshComponent.Draw();
                        }
                    }
                    render->DrawModeTriangles();
                },
                [&](Render* render) {
                    team1Name.Draw();
                    team2Name.Draw();
                    team1Count.Draw();
                    team2Count.Draw();
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