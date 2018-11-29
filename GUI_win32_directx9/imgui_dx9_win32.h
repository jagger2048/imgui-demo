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

#include <string>
#include <map>
#include <iostream>
const std::map<std::string, int> FILTER_TYPE{ { "lpf",1 },{ "hpf",2 } ,{ "apf",3 },{ "bpf",4 },{ "notch",5 } ,{ "peakingEQ",6 },{ "lowShelf",7 },{ "highShelf",8 } };
using namespace std;

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
	ImVec4 clear_color;
#define CLASS_NEME (_T("ImGui Example"))
#define TITLE (_T(" test case"))

	HWND create_window() {
		// Create application window
		wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, CLASS_NEME, NULL };
		RegisterClassEx(&wc);

		//hwnd = CreateWindow(_T("ImGui Example"), _T("Dear ImGui DirectX9 Example"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);
		hwnd = CreateWindow(CLASS_NEME, TITLE, WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

		return hwnd;
	}

	double create_D3D_device(){
		// Initialize Direct3D
		if((pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
			{
				UnregisterClass( CLASS_NEME, wc.hInstance);
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
			UnregisterClass(CLASS_NEME, wc.hInstance);
			return 0;
		}
		return 1;
	
	}
	int DW_CREATE() {
		create_window();
		create_D3D_device();
		return 0;
	}
	double DW_INIT() {
		// Setup Platform/Renderer bindings, and color

		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX9_Init(g_pd3dDevice);

		clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		return 0;
	}
	void DW_NEW_FRAME() {
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
	}

	double DW_SHOW_WINDOW() {
		ShowWindow(hwnd, SW_SHOWDEFAULT);
		UpdateWindow(hwnd);
		return 0;
	}
	void DW_RENDERING() {
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, false);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
		D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x*255.0f), (int)(clear_color.y*255.0f), (int)(clear_color.z*255.0f), (int)(clear_color.w*255.0f));
		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
		if (g_pd3dDevice->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			g_pd3dDevice->EndScene();
		}
		HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

		// Handle loss of D3D9 device
		if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			g_pd3dDevice->Reset(&g_d3dpp);
			ImGui_ImplDX9_CreateDeviceObjects();
		}

	}
	void DW_SHUTDOWN() {
		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
	}
	double MSG_INIT() {
		ZeroMemory(&msg, sizeof(msg));
		//ShowWindow(hwnd, SW_SHOWDEFAULT);
		//UpdateWindow(hwnd);
		return 0;
	}

	bool MSG_GET_QUIT() {
		return msg.message == WM_QUIT;
	}

	void MEG_CHECK() {
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			return;
		}
	}

	void DW_RELEASE() {
		if (g_pd3dDevice) g_pd3dDevice->Release();
		if (pD3D) pD3D->Release();
		DestroyWindow(hwnd);
		UnregisterClass(CLASS_NEME, wc.hInstance);
	}
	

	DX9_WIN32_MANAGER();
	~DX9_WIN32_MANAGER();

private:

};

DX9_WIN32_MANAGER::DX9_WIN32_MANAGER()
{
	//class_name = _class_name;
}

DX9_WIN32_MANAGER::~DX9_WIN32_MANAGER()
{
	DW_RELEASE();
}



class FILTER_DESIGN
{
public:
	std::string filter_type;
	float f0 = 1000;
	float fs = 48e3;
	float pi = 3.1415;
	//float w0 = 2 * pi*f0 / fs;
	float w0 = 0;

	float Q = 1;
	float Boost = 1;
	float BW = 1;					// % var2 = B, in Hz
	float S = 1;
	float a0, a1, a2, b0, b1, b2;
	float alpha = 1;
	float A = 1;

	int design(std::string _filter_type){
		auto iter = FILTER_TYPE.find(_filter_type);
		if (iter == FILTER_TYPE.end()) {
			std::cout << "Invail filter type\n";
			return 0;
		}
		else {
			w0 = 2 * pi*f0 / fs;

			switch (iter->second) {
			case 1:		// lpf
				alpha = sin(w0) / (2 * Q);
				a0 = 1 + alpha;
				a1 = -2 * cos(w0);
				a2 = 1 - alpha;
				b0 = (1 - cos(w0)) / 2;
				b1 = 1 - cos(w0);
				b2 = (1 - cos(w0)) / 2;
				break;
			case 2:		//'hpf':
				alpha = sin(w0) / (2 * Q);
				a0 = 1 + alpha;
				a1 = -2 * cos(w0);
				a2 = 1 - alpha;
				b0 = (1 + cos(w0)) / 2;
				b1 = -(1 + cos(w0));
				b2 = (1 + cos(w0)) / 2;
				break;
			case 3://'apf':
				alpha = sin(w0) / (2 * Q);
				b0 = 1 - alpha;
				b1 = -2 * cos(w0);
				b2 = 1 + alpha;
				a0 = 1 + alpha;
				a1 = -2 * cos(w0);
				a2 = 1 - alpha;
				break;

			case 4://'bpf':// % Q = 1时是特殊情况
				alpha = sin(w0)*sinh(log(2) / 2 * BW * w0 / sin(w0)); //% BW
				b0 = Q * alpha;
				b1 = 0;
				b2 = -Q * alpha;
				a0 = 1 + alpha;
				a1 = -2 * cos(w0);
				a2 = 1 - alpha;
				break;

			case 5://'notch':
				alpha = sin(w0)*sinh(log(2) / 2 * BW * w0 / sin(w0)); //% BW
				b0 = 1;
				b1 = -2 * cos(w0);
				b2 = 1;
				a0 = 1 + alpha;
				a1 = -2 * cos(w0);
				a2 = 1 - alpha;
				break;

			case 6://'peakingEQ': //% paramatric ， Boost 为中心频率的增益

				   //% %         BW = 2 * pi*B / fs;
				   //%         f0 = 1000;
				   //%         fs = 48e3;
				   //%         Boost = 3;
				   //%         BW = 1;
				   //%         w0 = 2 * pi*f0. / fs;

				A = pow(10, (Boost / 40));
				alpha = sin(w0)*sinh(log(2) / 2 * BW * w0 / sin(w0));
				//%         alpha = sin(w0) / (2 * Q); % 简化版，Q为 0.07~15
				//	% BW； 1 / Q = sinh(log(2) / 2 * BW * w0 / sin(w0)) % !!!
				//	%     %so Q = 1 / sinh(log(2) / 2 * BW * w0 / sin(w0)); % Q与BW成反比
				b0 = 1 + alpha * A;
				b1 = -2 * cos(w0);
				b2 = 1 - alpha * A;
				a0 = 1 + alpha / A;
				a1 = -2 * cos(w0);
				a2 = 1 - alpha / A;
				break;


			case 7://'lowShelf':
				A = pow(10, (Boost / 40));
				alpha = sin(w0) / 2 * sqrt((A + 1 / A)*(1 / S - 1) + 2);	// % S
																			//	% 1 / Q = sqrt((A + 1 / A)*(1 / S - 1) + 2), S = 1，Q = sqrt(2) / 2
				b0 = A * ((A + 1) - (A - 1)*cos(w0) + 2 * sqrt(A)*alpha);
				b1 = 2 * A*((A - 1) - (A + 1)*cos(w0));
				b2 = A * ((A + 1) - (A - 1)*cos(w0) - 2 * sqrt(A)*alpha);
				a0 = (A + 1) + (A - 1)*cos(w0) + 2 * sqrt(A)*alpha;
				a1 = -2 * ((A - 1) + (A + 1)*cos(w0));
				a2 = (A + 1) + (A - 1)*cos(w0) - 2 * sqrt(A)*alpha;
				break;

			case 8://'highShelf':
				A = pow(10, (Boost / 40.0));
				alpha = sin(w0) / 2.0 * sqrt((A + 1.0 / A)*(1.0 / S - 1) + 2);// % S
				b0 = A * ((A + 1) + (A - 1)*cos(w0) + 2 * sqrt(A)*alpha);
				b1 = -2 * A*((A - 1) + (A + 1)*cos(w0));
				b2 = A * ((A + 1) + (A - 1)*cos(w0) - 2 * sqrt(A)*alpha);
				a0 = (A + 1) - (A - 1)*cos(w0) + 2 * sqrt(A)*alpha;
				a1 = 2 * ((A - 1) - (A + 1)*cos(w0));
				a2 = (A + 1) - (A - 1)*cos(w0) - 2 * sqrt(A)*alpha;
			default:
				break;
			}
		}
		//return	vector<vector<float>>{ {b0 / a0, b1 / a0, b2 / a0}, { a0 / a0, a1 / a0, a2 / a0 }};
		return 0;
	}
	FILTER_DESIGN();
	~FILTER_DESIGN();

private:

};

FILTER_DESIGN::FILTER_DESIGN()
{
}

FILTER_DESIGN::~FILTER_DESIGN()
{
}
