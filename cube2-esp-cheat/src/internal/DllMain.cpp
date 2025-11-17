#include <Windows.h>
#include <thread>
#include <d3d11.h>
#include <dxgi.h>
#include "../../include/internal/DirectMemory.h"
#include "../../include/internal/AimbotInternal.h"
#include "../../include/Config.h"
#include "../../include/overlay/Menu.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

// Global instances
DirectMemory* g_Memory = nullptr;
AimbotInternal* g_Aimbot = nullptr;
bool g_Running = true;

// D3D11 variables
ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;
ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
HWND g_hWnd = nullptr;

typedef HRESULT(__stdcall* PresentFn)(IDXGISwapChain*, UINT, UINT);
PresentFn oPresent = nullptr;

bool g_ImGuiInitialized = false;
WNDPROC oWndProc = nullptr;

// Forward declarations
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void CreateRenderTarget();
void CleanupRenderTarget();
void RenderMenu();

LRESULT CALLBACK WndProcHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (g_ImGuiInitialized && CheatConfig::g_Config.menu.visible) {
        ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
    }
    return CallWindowProc(oWndProc, hWnd, msg, wParam, lParam);
}

HRESULT __stdcall PresentHook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    // First time initialization
    if (!g_ImGuiInitialized) {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&g_pd3dDevice))) {
            g_pSwapChain = pSwapChain;
            g_pd3dDevice->GetImmediateContext(&g_pd3dDeviceContext);

            DXGI_SWAP_CHAIN_DESC sd;
            pSwapChain->GetDesc(&sd);
            g_hWnd = sd.OutputWindow;

            // Hook WndProc
            oWndProc = (WNDPROC)SetWindowLongPtr(g_hWnd, GWLP_WNDPROC, (LONG_PTR)WndProcHook);

            // Initialize ImGui
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            io.IniFilename = nullptr;

            ImGui::StyleColorsDark();
            ImGuiStyle& style = ImGui::GetStyle();
            style.WindowRounding = 5.0f;
            style.FrameRounding = 3.0f;
            style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
            style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
            style.Colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.95f);
            style.Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.5f, 0.2f, 1.0f);
            style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.6f, 0.3f, 1.0f);
            style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.4f, 0.7f, 0.4f, 1.0f);

            ImGui_ImplWin32_Init(g_hWnd);
            ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

            CreateRenderTarget();
            g_ImGuiInitialized = true;

            printf("[+] ImGui initialized!\n");
        }
    }

    if (g_ImGuiInitialized) {
        RenderMenu();
    }

    return oPresent(pSwapChain, SyncInterval, Flags);
}

void CreateRenderTarget() {
    ID3D11Texture2D* pBackBuffer = nullptr;
    g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
    if (pBackBuffer) {
        g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
        pBackBuffer->Release();
    }
}

void CleanupRenderTarget() {
    if (g_mainRenderTargetView) {
        g_mainRenderTargetView->Release();
        g_mainRenderTargetView = nullptr;
    }
}

void RenderMenu() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    static Menu menu;

    // Status overlay
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(280, 100), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.8f);
    if (ImGui::Begin("Cube2 Internal Cheat", nullptr,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar)) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.5f, 1.0f), "Cube2 Internal v2.0 - INJECTED");
        ImGui::Separator();
        ImGui::Text("Aimbot: %s", CheatConfig::g_Config.aimbot.enabled ? "ON" : "OFF");
        ImGui::Text("ESP: %s", CheatConfig::g_Config.esp.enabled ? "ON" : "OFF");
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "INSERT = Menu");
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "END = Unload");
        ImGui::End();
    }

    // Main menu
    if (CheatConfig::g_Config.menu.visible) {
        menu.Render(nullptr);
    }

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
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

bool HookD3D11() {
    // Create temporary window and device to get VTable
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), 0, DefWindowProc, 0, 0, GetModuleHandle(nullptr),
                      nullptr, nullptr, nullptr, nullptr, L"TempDX", nullptr };
    RegisterClassEx(&wc);
    HWND hWnd = CreateWindowW(L"TempDX", L"", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100,
                              nullptr, nullptr, wc.hInstance, nullptr);

    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    ID3D11Device* pDevice = nullptr;
    ID3D11DeviceContext* pContext = nullptr;
    IDXGISwapChain* pSwapChain = nullptr;
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

    if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
                                              &featureLevel, 1, D3D11_SDK_VERSION, &sd,
                                              &pSwapChain, &pDevice, nullptr, &pContext))) {
        DestroyWindow(hWnd);
        UnregisterClass(L"TempDX", wc.hInstance);
        return false;
    }

    // Get VTable
    void** pVTable = *reinterpret_cast<void***>(pSwapChain);
    void* pPresentAddress = pVTable[8];

    // Save original
    oPresent = (PresentFn)pPresentAddress;

    // Patch VTable
    DWORD oldProtect;
    VirtualProtect(&pVTable[8], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect);
    pVTable[8] = &PresentHook;
    VirtualProtect(&pVTable[8], sizeof(void*), oldProtect, &oldProtect);

    // Cleanup
    pSwapChain->Release();
    pContext->Release();
    pDevice->Release();
    DestroyWindow(hWnd);
    UnregisterClass(L"TempDX", wc.hInstance);

    printf("[+] D3D11 Present hooked at: 0x%p\n", pPresentAddress);
    return true;
}

// Main cheat thread
DWORD WINAPI MainThread(LPVOID lpParam) {
    // Allocate console for debugging
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    freopen_s(&f, "CONOUT$", "w", stderr);

    printf("========================================\n");
    printf(" Cube 2: Sauerbraten Internal Cheat\n");
    printf(" Version 2.0 - Fixed\n");
    printf("========================================\n\n");

    printf("[+] DLL Injected Successfully!\n");
    printf("[+] Initializing...\n\n");

    // Initialize memory
    g_Memory = new DirectMemory();
    if (!g_Memory->Initialize()) {
        printf("[-] Failed to initialize memory!\n");
        return 1;
    }

    printf("[+] Module Base: 0x%p\n", (void*)g_Memory->GetModuleBase());

    // Initialize aimbot
    g_Aimbot = new AimbotInternal(*g_Memory);
    printf("[+] Aimbot initialized\n");

    // Hook D3D11
    printf("[+] Hooking DirectX11...\n");
    Sleep(3000); // Wait for game to initialize D3D11

    if (!HookD3D11()) {
        printf("[-] Failed to hook D3D11!\n");
    } else {
        printf("[+] D3D11 hooked successfully!\n");
    }

    printf("\n========================================\n");
    printf(" READY!\n");
    printf("========================================\n");
    printf(" Controls:\n");
    printf("   INSERT = Toggle Menu\n");
    printf("   END    = Unload Cheat\n");
    printf("========================================\n\n");

    // Main loop
    while (g_Running) {
        // Check for unload key
        if (GetAsyncKeyState(VK_END) & 0x8000) {
            printf("[!] Unloading cheat...\n");
            g_Running = false;
            break;
        }

        // Update aimbot
        if (g_Aimbot && CheatConfig::g_Config.aimbot.enabled) {
            g_Aimbot->Update();
        }

        Sleep(1);
    }

    // Cleanup
    printf("[+] Cleaning up...\n");

    if (g_ImGuiInitialized) {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    if (oWndProc && g_hWnd) {
        SetWindowLongPtr(g_hWnd, GWLP_WNDPROC, (LONG_PTR)oWndProc);
    }

    CleanupRenderTarget();

    if (g_Aimbot) delete g_Aimbot;
    if (g_Memory) delete g_Memory;

    printf("[+] Cleanup complete. Unloading DLL...\n");
    Sleep(1000);

    fclose(stdout);
    fclose(stderr);
    FreeConsole();

    FreeLibraryAndExitThread(static_cast<HMODULE>(lpParam), 0);
    return 0;
}

// DLL Entry Point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    switch (dwReason) {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hModule);
            CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
            break;

        case DLL_PROCESS_DETACH:
            g_Running = false;
            break;
    }

    return TRUE;
}
