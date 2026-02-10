#pragma once

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>

// Declarações de funções
void LoadESPConfigFromRegistry();
void DrawLine(float x1, float y1, float x2, float y2, float thickness, float r, float g, float b, float a = 1.0f);
void DrawString(const std::wstring& str, float fontSize, float x, float y, float r, float g, float b, float a = 1.0f);
void DrawESP(int width, int height);
DWORD WINAPI OverlayThread(LPVOID lpParam);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
