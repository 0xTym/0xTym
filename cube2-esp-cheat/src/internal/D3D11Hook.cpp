#include "../../include/internal/D3D11Hook.h"
#include "../../include/overlay/Menu.h"
#include "../../include/Config.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <cstring>

// Static members
ID3D11Device* D3D11Hook::pDevice = nullptr;
ID3D11DeviceContext* D3D11Hook::pContext = nullptr;
IDXGISwapChain* D3D11Hook::pSwapChain = nullptr;
ID3D11RenderTargetView* D3D11Hook::pRenderTargetView = nullptr;
HWND D3D11Hook::hWnd = nullptr;

void* D3D11Hook::pPresentOriginal = nullptr;
void* D3D11Hook::pResizeBuffersOriginal = nullptr;

bool D3D11Hook::initialized = false;
bool D3D11Hook::imguiInitialized = false;

// Forward declaration
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static WNDPROC oWndProc = nullptr;

LRESULT CALLBACK WndProcHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (D3D11Hook::IsInitialized()) {
        ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

        // Block input to game when menu is visible
        if (CheatConfig::g_Config.menu.visible) {
            return DefWindowProc(hWnd, msg, wParam, lParam);
        }
    }

    return CallWindowProc(oWndProc, hWnd, msg, wParam, lParam);
}

bool D3D11Hook::Initialize() {
    if (initialized) {
        return true;
    }

    if (!FindSwapChain()) {
        return false;
    }

    if (!InstallHooks()) {
        return false;
    }

    initialized = true;
    return true;
}

void D3D11Hook::Shutdown() {
    if (!initialized) {
        return;
    }

    ShutdownImGui();
    RemoveHooks();
    CleanupRenderTarget();

    if (pSwapChain) {
        pSwapChain = nullptr;
    }
    if (pContext) {
        pContext = nullptr;
    }
    if (pDevice) {
        pDevice = nullptr;
    }

    initialized = false;
}

bool D3D11Hook::FindSwapChain() {
    // Create temporary window
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), 0, DefWindowProc, 0, 0, GetModuleHandle(nullptr),
                      nullptr, nullptr, nullptr, nullptr, L"TempClass", nullptr };
    RegisterClassEx(&wc);

    HWND hTempWnd = CreateWindowW(L"TempClass", L"", WS_OVERLAPPEDWINDOW,
                                   0, 0, 100, 100, nullptr, nullptr, wc.hInstance, nullptr);

    // Create temporary swap chain
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hTempWnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    ID3D11Device* pTempDevice = nullptr;
    ID3D11DeviceContext* pTempContext = nullptr;
    IDXGISwapChain* pTempSwapChain = nullptr;

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        &featureLevel, 1, D3D11_SDK_VERSION, &sd,
        &pTempSwapChain, &pTempDevice, nullptr, &pTempContext);

    if (FAILED(hr)) {
        DestroyWindow(hTempWnd);
        UnregisterClass(L"TempClass", wc.hInstance);
        return false;
    }

    // Get VTable
    void** pSwapChainVTable = *reinterpret_cast<void***>(pTempSwapChain);
    pPresentOriginal = pSwapChainVTable[8]; // Present is at index 8
    pResizeBuffersOriginal = pSwapChainVTable[13]; // ResizeBuffers at index 13

    // Cleanup
    pTempSwapChain->Release();
    pTempContext->Release();
    pTempDevice->Release();
    DestroyWindow(hTempWnd);
    UnregisterClass(L"TempClass", wc.hInstance);

    return true;
}

bool D3D11Hook::InstallHooks() {
    if (!pPresentOriginal) {
        return false;
    }

    // Simple inline hook (in production, use MinHook or similar)
    DWORD oldProtect;
    VirtualProtect(pPresentOriginal, 5, PAGE_EXECUTE_READWRITE, &oldProtect);

    // This is a simplified hook - in production use a proper hooking library
    // For now, we'll use a different approach: we'll hook it when we detect the game's swap chain

    return true;
}

void D3D11Hook::RemoveHooks() {
    // Restore original functions
    // (Simplified - in production use proper unhooking)
}

HRESULT __stdcall D3D11Hook::PresentHook(IDXGISwapChain* pChain, UINT SyncInterval, UINT Flags) {
    if (!pSwapChain) {
        pSwapChain = pChain;
        pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice);
        pDevice->GetImmediateContext(&pContext);

        DXGI_SWAP_CHAIN_DESC sd;
        pSwapChain->GetDesc(&sd);
        hWnd = sd.OutputWindow;

        // Hook WndProc
        oWndProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)WndProcHook);

        InitializeImGui();
        CreateRenderTarget();
    }

    if (imguiInitialized) {
        RenderImGui();
    }

    // Call original Present
    typedef HRESULT(__stdcall* PresentFn)(IDXGISwapChain*, UINT, UINT);
    return ((PresentFn)pPresentOriginal)(pChain, SyncInterval, Flags);
}

HRESULT __stdcall D3D11Hook::ResizeBuffersHook(IDXGISwapChain* pChain, UINT BufferCount,
                                                UINT Width, UINT Height,
                                                DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
    CleanupRenderTarget();

    typedef HRESULT(__stdcall* ResizeBuffersFn)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
    HRESULT hr = ((ResizeBuffersFn)pResizeBuffersOriginal)(pChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);

    CreateRenderTarget();
    return hr;
}

void D3D11Hook::InitializeImGui() {
    if (imguiInitialized || !pDevice || !pContext || !hWnd) {
        return;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    // Custom style
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.95f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.5f, 0.2f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.6f, 0.3f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.4f, 0.7f, 0.4f, 1.0f);

    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(pDevice, pContext);

    imguiInitialized = true;
}

void D3D11Hook::ShutdownImGui() {
    if (!imguiInitialized) {
        return;
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    imguiInitialized = false;
}

void D3D11Hook::RenderImGui() {
    if (!imguiInitialized) {
        return;
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Static menu instance
    static Menu menu;
    static DirectMemory* dummyMem = nullptr; // Not used in internal version

    // Render menu
    if (CheatConfig::g_Config.menu.visible) {
        menu.Render(*reinterpret_cast<MemoryReader*>(dummyMem));
    }

    // Status overlay
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(250, 80), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.7f);
    ImGui::Begin("Cube2 Internal Cheat", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.5f, 1.0f), "Cube2 Internal v2.0");
    ImGui::Separator();
    ImGui::Text("Status: Injected");
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Press INSERT for menu");
    ImGui::End();

    // Handle menu toggle
    static bool insertPressed = false;
    if (GetAsyncKeyState(VK_INSERT) & 0x8000) {
        if (!insertPressed) {
            CheatConfig::g_Config.menu.visible = !CheatConfig::g_Config.menu.visible;
            insertPressed = true;
        }
    } else {
        insertPressed = false;
    }

    ImGui::Render();
    pContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void D3D11Hook::CreateRenderTarget() {
    if (!pSwapChain || !pDevice) {
        return;
    }

    ID3D11Texture2D* pBackBuffer = nullptr;
    pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);

    if (pBackBuffer) {
        pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView);
        pBackBuffer->Release();
    }
}

void D3D11Hook::CleanupRenderTarget() {
    if (pRenderTargetView) {
        pRenderTargetView->Release();
        pRenderTargetView = nullptr;
    }
}
