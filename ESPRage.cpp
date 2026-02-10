#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <dwmapi.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "ESPRage.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "dwrite.lib")

// Variáveis globais
ID2D1Factory* factory = nullptr;
ID2D1HwndRenderTarget* target = nullptr;
ID2D1SolidColorBrush* solid_brush = nullptr;
IDWriteFactory* w_factory = nullptr;
HWND overlayWindow = nullptr;
HWND targetWindow = nullptr;
bool shouldRun = true;

// Configurações ESP (lidas do registry)
bool ESPDrawLine = false;
int ESPLinePos = 0; // 0 = Top, 1 = Bottom
bool ESPDrawBox = false;
int ESPStyleBox = 0; // 0 = Corner, 1 = Full
bool ESPDrawHealthBar = false;
int ESPPosHealthBar = 0; // 0 = Left, 1 = Right, 2 = Down
bool ESPDrawSkeleton = false;
bool ESPDrawName = false;
bool ESPDrawDistance = false;
int ESPMaxDistance = 40;

// Função para ler configurações do registry
void LoadESPConfigFromRegistry()
{
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\IMMORTAL\\ESP", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        DWORD dwType = REG_DWORD;
        DWORD dwValue = 0;
        DWORD dwSize = sizeof(DWORD);

        if (RegQueryValueExA(hKey, "ESPDrawLine", NULL, &dwType, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
            ESPDrawLine = (dwValue == 1);
        
        dwValue = 0;
        dwSize = sizeof(DWORD);
        if (RegQueryValueExA(hKey, "ESPLinePos", NULL, &dwType, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
            ESPLinePos = dwValue;

        dwValue = 0;
        dwSize = sizeof(DWORD);
        if (RegQueryValueExA(hKey, "ESPDrawBox", NULL, &dwType, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
            ESPDrawBox = (dwValue == 1);

        dwValue = 0;
        dwSize = sizeof(DWORD);
        if (RegQueryValueExA(hKey, "ESPStyleBox", NULL, &dwType, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
            ESPStyleBox = dwValue;

        dwValue = 0;
        dwSize = sizeof(DWORD);
        if (RegQueryValueExA(hKey, "ESPDrawHealthBar", NULL, &dwType, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
            ESPDrawHealthBar = (dwValue == 1);

        dwValue = 0;
        dwSize = sizeof(DWORD);
        if (RegQueryValueExA(hKey, "ESPPosHealthBar", NULL, &dwType, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
            ESPPosHealthBar = dwValue;

        dwValue = 0;
        dwSize = sizeof(DWORD);
        if (RegQueryValueExA(hKey, "ESPDrawSkeleton", NULL, &dwType, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
            ESPDrawSkeleton = (dwValue == 1);

        dwValue = 0;
        dwSize = sizeof(DWORD);
        if (RegQueryValueExA(hKey, "ESPDrawName", NULL, &dwType, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
            ESPDrawName = (dwValue == 1);

        dwValue = 0;
        dwSize = sizeof(DWORD);
        if (RegQueryValueExA(hKey, "ESPDrawDistance", NULL, &dwType, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
            ESPDrawDistance = (dwValue == 1);

        dwValue = 40;
        dwSize = sizeof(DWORD);
        if (RegQueryValueExA(hKey, "ESPMaxDistance", NULL, &dwType, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
            ESPMaxDistance = dwValue;

        RegCloseKey(hKey);
    }
}

// Função para desenhar linha
void DrawLine(float x1, float y1, float x2, float y2, float thickness, float r, float g, float b, float a = 1.0f)
{
    if (!solid_brush || !target) return;
    solid_brush->SetColor(D2D1::ColorF(r, g, b, a));
    target->DrawLine(D2D1::Point2F(x1, y1), D2D1::Point2F(x2, y2), solid_brush, thickness);
}

// Função para desenhar texto
void DrawString(const std::wstring& str, float fontSize, float x, float y, float r, float g, float b, float a = 1.0f)
{
    if (!w_factory || !target) return;

    IDWriteTextFormat* textFormat = nullptr;
    HRESULT hr = w_factory->CreateTextFormat(
        L"Arial",
        NULL,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        fontSize,
        L"en-US",
        &textFormat
    );

    if (SUCCEEDED(hr) && textFormat)
    {
        ID2D1SolidColorBrush* textBrush = nullptr;
        target->CreateSolidColorBrush(D2D1::ColorF(r, g, b, a), &textBrush);
        
        if (textBrush)
        {
            target->DrawText(
                str.c_str(),
                str.length(),
                textFormat,
                D2D1::RectF(x, y, x + 1000, y + 1000),
                textBrush
            );
            textBrush->Release();
        }
        textFormat->Release();
    }
}

// Função principal de desenho (será chamada em loop)
void DrawESP(int width, int height)
{
    if (!target) return;

    // Recarrega configurações do registry periodicamente
    static int frameCount = 0;
    if (frameCount % 60 == 0) // A cada 60 frames
    {
        LoadESPConfigFromRegistry();
    }
    frameCount++;

    // Limpa o buffer
    target->BeginDraw();
    target->Clear(D2D1::ColorF(0, 0, 0, 0));

    // Exemplo de desenho - você pode adaptar com a lógica real do rage
    float centerX = width / 2.0f;
    float centerY = height / 2.0f;

    if (ESPDrawLine)
    {
        if (ESPLinePos == 0) // Top
        {
            DrawLine(centerX, 0, centerX + 100, centerY - 50, 2.0f, 1.0f, 1.0f, 1.0f);
        }
        else // Bottom
        {
            DrawLine(centerX, height, centerX + 100, centerY + 50, 2.0f, 1.0f, 1.0f, 1.0f);
        }
    }

    if (ESPDrawBox)
    {
        float boxX = centerX - 50;
        float boxY = centerY - 50;
        float boxW = 100;
        float boxH = 100;
        
        if (ESPStyleBox == 0) // Corner
        {
            float cornerLength = 20.0f;
            DrawLine(boxX, boxY, boxX + cornerLength, boxY, 2.0f, 1.0f, 1.0f, 1.0f);
            DrawLine(boxX, boxY, boxX, boxY + cornerLength, 2.0f, 1.0f, 1.0f, 1.0f);
            DrawLine(boxX + boxW - cornerLength, boxY, boxX + boxW, boxY, 2.0f, 1.0f, 1.0f, 1.0f);
            DrawLine(boxX + boxW, boxY, boxX + boxW, boxY + cornerLength, 2.0f, 1.0f, 1.0f, 1.0f);
            DrawLine(boxX, boxY + boxH - cornerLength, boxX, boxY + boxH, 2.0f, 1.0f, 1.0f, 1.0f);
            DrawLine(boxX, boxY + boxH, boxX + cornerLength, boxY + boxH, 2.0f, 1.0f, 1.0f, 1.0f);
            DrawLine(boxX + boxW - cornerLength, boxY + boxH, boxX + boxW, boxY + boxH, 2.0f, 1.0f, 1.0f, 1.0f);
            DrawLine(boxX + boxW, boxY + boxH - cornerLength, boxX + boxW, boxY + boxH, 2.0f, 1.0f, 1.0f, 1.0f);
        }
        else // Full
        {
            DrawLine(boxX, boxY, boxX + boxW, boxY, 2.0f, 1.0f, 1.0f, 1.0f);
            DrawLine(boxX + boxW, boxY, boxX + boxW, boxY + boxH, 2.0f, 1.0f, 1.0f, 1.0f);
            DrawLine(boxX + boxW, boxY + boxH, boxX, boxY + boxH, 2.0f, 1.0f, 1.0f, 1.0f);
            DrawLine(boxX, boxY + boxH, boxX, boxY, 2.0f, 1.0f, 1.0f, 1.0f);
        }
    }

    if (ESPDrawHealthBar)
    {
        float barX, barY, barW = 5, barH = 100;
        float healthPercent = 0.75f; // Exemplo: 75% de vida
        
        if (ESPPosHealthBar == 0) // Left
        {
            barX = centerX - 60;
            barY = centerY - 50;
        }
        else if (ESPPosHealthBar == 1) // Right
        {
            barX = centerX + 60;
            barY = centerY - 50;
        }
        else // Down
        {
            barX = centerX - 50;
            barY = centerY + 60;
            barW = 100;
            barH = 5;
        }
        
        // Fundo da barra
        DrawLine(barX, barY, barX + barW, barY + barH, 3.0f, 0.0f, 0.0f, 0.0f);
        // Barra de vida
        if (ESPPosHealthBar == 2) // Down
        {
            DrawLine(barX, barY, barX + barW * healthPercent, barY, 3.0f, 0.0f, 1.0f, 0.0f);
        }
        else
        {
            DrawLine(barX, barY + barH, barX, barY + barH - (barH * healthPercent), 3.0f, 0.0f, 1.0f, 0.0f);
        }
    }

    if (ESPDrawSkeleton)
    {
        // Exemplo de esqueleto simplificado
        float headX = centerX;
        float headY = centerY - 40;
        float chestX = centerX;
        float chestY = centerY;
        float hipX = centerX;
        float hipY = centerY + 20;
        
        DrawLine(headX, headY, chestX, chestY, 2.0f, 1.0f, 1.0f, 1.0f);
        DrawLine(chestX, chestY, hipX, hipY, 2.0f, 1.0f, 1.0f, 1.0f);
        DrawLine(chestX, chestY, chestX - 20, chestY + 10, 2.0f, 1.0f, 1.0f, 1.0f);
        DrawLine(chestX, chestY, chestX + 20, chestY + 10, 2.0f, 1.0f, 1.0f, 1.0f);
        DrawLine(hipX, hipY, hipX - 15, hipY + 20, 2.0f, 1.0f, 1.0f, 1.0f);
        DrawLine(hipX, hipY, hipX + 15, hipY + 20, 2.0f, 1.0f, 1.0f, 1.0f);
    }

    if (ESPDrawName)
    {
        DrawString(L"PLAYER", 15.0f, centerX - 30, centerY - 60, 1.0f, 1.0f, 1.0f);
    }

    if (ESPDrawDistance)
    {
        DrawString(L"10.5m", 12.0f, centerX - 20, centerY + 60, 1.0f, 1.0f, 1.0f);
    }

    target->EndDraw();
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
        shouldRun = false;
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        shouldRun = false;
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

DWORD WINAPI OverlayThread(LPVOID lpParam)
{
    // Encontra a janela do HD-Player
    targetWindow = FindWindowA(NULL, "HD-Player");
    if (!targetWindow)
    {
        // Tenta encontrar por classe ou outros métodos
        EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
            char className[256];
            GetClassNameA(hwnd, className, sizeof(className));
            if (strstr(className, "HD") || strstr(className, "Player"))
            {
                *((HWND*)lParam) = hwnd;
                return FALSE;
            }
            return TRUE;
        }, (LPARAM)&targetWindow);
    }

    if (!targetWindow)
    {
        return 1;
    }

    // Cria a janela de overlay
    WNDCLASSA wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandleA(NULL);
    wc.lpszClassName = "ESPRageOverlay";
    RegisterClassA(&wc);

    RECT targetRect;
    GetWindowRect(targetWindow, &targetRect);
    
    overlayWindow = CreateWindowExA(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        "ESPRageOverlay",
        "ESP Overlay",
        WS_POPUP,
        targetRect.left, targetRect.top, 
        targetRect.right - targetRect.left,
        targetRect.bottom - targetRect.top,
        NULL, NULL, wc.hInstance, NULL
    );

    if (!overlayWindow)
    {
        return 1;
    }

    SetLayeredWindowAttributes(overlayWindow, RGB(0, 0, 0), 255, LWA_ALPHA);

    // Inicializa Direct2D
    D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &factory);
    
    GetClientRect(targetWindow, &targetRect);
    
    factory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)),
        D2D1::HwndRenderTargetProperties(overlayWindow, D2D1::SizeU(targetRect.right - targetRect.left, targetRect.bottom - targetRect.top),
            D2D1_PRESENT_OPTIONS_IMMEDIATELY), &target);

    if (target)
    {
        target->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f), &solid_brush);
        target->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    }

    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&w_factory));

    ShowWindow(overlayWindow, SW_SHOW);
    UpdateWindow(overlayWindow);

    // Carrega configurações iniciais
    LoadESPConfigFromRegistry();

    // Loop principal
    MSG msg = {};
    while (shouldRun)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (targetWindow && IsWindow(targetWindow))
        {
            GetWindowRect(targetWindow, &targetRect);
            
            SetWindowPos(overlayWindow, HWND_TOPMOST, 
                targetRect.left, targetRect.top,
                targetRect.right - targetRect.left,
                targetRect.bottom - targetRect.top,
                SWP_SHOWWINDOW);

            if (target)
            {
                target->Resize(D2D1::SizeU(targetRect.right - targetRect.left, targetRect.bottom - targetRect.top));
                DrawESP(targetRect.right - targetRect.left, targetRect.bottom - targetRect.top);
            }
        }
        else
        {
            // Janela alvo fechou, tenta encontrar novamente
            targetWindow = FindWindowA(NULL, "HD-Player");
            if (!targetWindow)
            {
                Sleep(1000);
            }
        }

        Sleep(16); // ~60 FPS
    }

    // Cleanup
    if (solid_brush) solid_brush->Release();
    if (target) target->Release();
    if (factory) factory->Release();
    if (w_factory) w_factory->Release();

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, 0, OverlayThread, NULL, 0, NULL);
        break;
    case DLL_PROCESS_DETACH:
        shouldRun = false;
        break;
    }
    return TRUE;
}
