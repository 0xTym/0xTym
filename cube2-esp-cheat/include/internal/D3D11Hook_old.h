#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <Windows.h>

class D3D11Hook {
private:
    static ID3D11Device* pDevice;
    static ID3D11DeviceContext* pContext;
    static IDXGISwapChain* pSwapChain;
    static ID3D11RenderTargetView* pRenderTargetView;
    static HWND hWnd;

    static void* pPresentOriginal;
    static void* pResizeBuffersOriginal;

    static bool initialized;
    static bool imguiInitialized;

public:
    static bool Initialize();
    static void Shutdown();

    static bool IsInitialized() { return initialized; }

    // Hook callbacks
    static HRESULT __stdcall PresentHook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
    static HRESULT __stdcall ResizeBuffersHook(IDXGISwapChain* pSwapChain, UINT BufferCount,
                                               UINT Width, UINT Height,
                                               DXGI_FORMAT NewFormat, UINT SwapChainFlags);

private:
    static bool FindSwapChain();
    static bool InstallHooks();
    static void RemoveHooks();

    static void InitializeImGui();
    static void ShutdownImGui();
    static void RenderImGui();

    static void CreateRenderTarget();
    static void CleanupRenderTarget();
};
