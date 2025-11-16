#pragma once
#include <d3d11.h>
#include <Windows.h>
#include <string>

// Forward declarations for ImGui
struct ImGuiContext;

class Overlay {
private:
    HWND targetWindow;
    HWND overlayWindow;

    ID3D11Device* device;
    ID3D11DeviceContext* deviceContext;
    IDXGISwapChain* swapChain;
    ID3D11RenderTargetView* renderTargetView;

    bool initialized;
    bool visible;

    WNDCLASSEXW windowClass;
    ImGuiContext* imguiContext;

public:
    Overlay();
    ~Overlay();

    bool Initialize(HWND targetWnd);
    void Shutdown();

    void BeginFrame();
    void EndFrame();
    void Render();

    bool IsVisible() const { return visible; }
    void SetVisible(bool vis) { visible = vis; }
    void ToggleVisible() { visible = !visible; }

    HWND GetOverlayWindow() const { return overlayWindow; }

private:
    bool CreateOverlayWindow();
    bool InitializeDirectX();
    bool InitializeImGui();

    void CleanupDirectX();
    void CleanupImGui();

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
