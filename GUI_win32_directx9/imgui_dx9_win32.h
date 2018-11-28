#pragma once
// dear imgui: standalone example application for DirectX 9
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>

// Data
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			g_d3dpp.BackBufferWidth = LOWORD(lParam);
			g_d3dpp.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

class DX9_WIN32_MANAGER
{
public:



	WNDCLASSEX wc;
	HWND hwnd;
	LPDIRECT3D9 pD3D;
	MSG msg;

	HWND createWindow() {
		wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
		RegisterClassEx(&wc);

		hwnd = CreateWindow(_T("ImGui Example"), _T("Dear ImGui DirectX9 Example"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

		return hwnd;
	}

	double createD3DDevice(){

		if((pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
			{
				UnregisterClass(_T("ImGui Example"), wc.hInstance);
				return 0;
			}
		ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
		g_d3dpp.Windowed = TRUE;
		g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
		g_d3dpp.EnableAutoDepthStencil = TRUE;
		g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; // Present with vsync
																//g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; // Present without vsync, maximum unthrottled framerate

																// Create the D3DDevice
		if (pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
		{
			pD3D->Release();
			UnregisterClass(_T("ImGui Example"), wc.hInstance);
			return 0;
		}
		return 1;
	
	}
	int DX9_WIN32_CREATE() {
		createWindow();
		createD3DDevice();
		return 0;
	}
	double DX9_WIN32_INIT() {
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX9_Init(g_pd3dDevice);
		return 0;
	}


	double MSG_INIT() {
		ZeroMemory(&msg, sizeof(msg));
		ShowWindow(hwnd, SW_SHOWDEFAULT);
		UpdateWindow(hwnd);
		return 0;
	}











	DX9_WIN32_MANAGER();
	~DX9_WIN32_MANAGER();

private:

};

DX9_WIN32_MANAGER::DX9_WIN32_MANAGER()
{
}

DX9_WIN32_MANAGER::~DX9_WIN32_MANAGER()
{
	if (g_pd3dDevice) g_pd3dDevice->Release();
	if (pD3D) pD3D->Release();
	DestroyWindow(hwnd);
	UnregisterClass(_T("ImGui Example"), wc.hInstance);
}