// GUI_win32_directx9.cpp: 定义控制台应用程序的入口点。
//

#include "imgui_dx9_win32.h"
#include "iostream"
#include <map>
#include <string>
#include <vector>
#include "filter_design.cpp"
using namespace std;
// Helper to display a little (?) mark which shows a tooltip when hovered.
static void ShowHelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}
int main(int, char**)
{
	DX9_WIN32_MANAGER DWM;
	DWM.DW_CREATE();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();

	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

	// Setup Platform/Renderer bindings, and color
	DWM.DW_INIT();


	// Setup Style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();		// 设置主题

	// Load Fonts
	ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\simhei.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
	IM_ASSERT(font != NULL);

	bool show_demo_window = true;
	bool show_another_window = false;

	// Main loop
	DWM.MSG_INIT();
	DWM.DW_SHOW_WINDOW();


	FILTER_DESIGN fd;
	vector<vector<float>> coefs;
	string ba_to_show = "";
	while ( !DWM.MSG_GET_QUIT() )
	{
		// Poll and handle messages (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.

		DWM.MEG_CHECK();

		// Start the Dear ImGui frame
		DWM.DW_NEW_FRAME();
		ImGui::NewFrame();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			// example Audio EQ design 
			ImGui::Begin(u8"滤波器设计");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text(u8"请选择需要设计的滤波器样式：");               // Display some text (you can use a format strings too)
			ImGui::Checkbox(u8"打开一个窗口", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox(u8"打开另一个窗口", &show_another_window);

			ImGui::SliderFloat(u8"滑块测试", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
			ImGui::ColorEdit3(u8"设置背景颜色", (float*)&(DWM.clear_color)); // Edit 3 floats representing a color

			if (ImGui::Button(u8"确定"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			if(ImGui::TreeNode("Audio EQ Filter Design")) {
				static ImGuiComboFlags flags = 0;
				const char* items[] = { "lpf", "hpf", "apf", "bpf", "notch", "peakingEQ", "lowShelf", "highShelf"};

				static const char* item_current = items[0];            // Here our selection is a single pointer stored outside the object.
				if (ImGui::BeginCombo("Type", item_current, flags)) // The second parameter is the label previewed before opening the combo.
				{
					for (int n = 0; n < IM_ARRAYSIZE(items); n++)
					{
						bool is_selected = (item_current == items[n]);
						if (ImGui::Selectable(items[n], is_selected))
							item_current = items[n];		// 传出选择的元素
						if (is_selected)
							ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
					}
					ImGui::EndCombo();

					fd.filter_type = string(item_current);  // update filter type
				}
				// 显示 UI 获取参数
				ImGui::InputFloat("F0", &fd.f0, 0.01f, 1.0f);
				ImGui::SameLine();
				ShowHelpMarker("The 'significant frequency',wherever it's 'happenin'.Center Frequency or Corner Frequency, or shelf midpoint frequency, depending on which filter type.");
				ImGui::InputFloat("Fs", &fd.fs, 0.01f, 1.0f);
				ImGui::SameLine();
				ShowHelpMarker("The sampling frequency");


				auto iter = FILTER_TYPE.find(item_current);
				if (iter != FILTER_TYPE.end()) {
					//fd.filter_type = item_current;
					if (iter->second <=3)
					{
						// Q		||	a l hpf
						ImGui::InputFloat("Q", &fd.Q, 0.01f, 1.0f);
						ImGui::SameLine();
						ShowHelpMarker("Q > 0.1, if Q = 0.707, it's the typically Butterworth filter.");
					}
					else if (iter->second <=5) {
						// Q  BW	||	  bpf notch
						ImGui::InputFloat("Q", &fd.Q, 0.01f, 1.0f);// Q = f0 /BW ?? 中心频率/带宽
						ImGui::SameLine();
						ShowHelpMarker("Constant skirt gain, when setting Q = 1 ,bpf has constant 0 dB peak gain,otherwise, peak gain = Q.");
						ImGui::InputFloat("BW", &fd.BW, 0.01f, 1.0f);
						ImGui::SameLine();
						ShowHelpMarker("The bandwidth in octaves ,between -3 dB frequencies for BPF and notch");

					}
					else if (iter->second == 6)
					{
						// Boost BW	||	peaking EQ
						ImGui::InputFloat("Boost", &fd.Boost, 0.01f, 1.0f);// dBgain
						ImGui::SameLine();
						ShowHelpMarker("Boost gain in dB ");
						ImGui::InputFloat("BW", &fd.BW, 0.01f, 1.0f);
						ImGui::SameLine();
						ShowHelpMarker("The bandwidth in octaves between midpoint(dBgain / 2) gain frequencies for peaking EQ.");
					}
					else {
						// Boost S	||	low high shelf
						ImGui::InputFloat("Boost", &fd.Boost, 0.01f, 1.0f); // dBgain
						ImGui::SameLine();
						ShowHelpMarker("Boost gain in dB ");
						ImGui::InputFloat("Shelf slope", &fd.S, 0.01f, 1.0f);
						ImGui::SameLine();
						ShowHelpMarker("Shelf slope parameter (for shelving EQ only).  When S = 1,	the shelf slope is as steep as it can be and remain monotonically increasing or decreasing gain with frequency.The shelf slope, in dB/octave, remains proportional to S for all other values for a fixed f0/Fs and dBgain");

					}
				}

				// 根据参数计算 b,a

				// 显示 b,a 
				ImGui::Text(ba_to_show.data()); ImGui::SameLine();

				ImGui::SameLine();
				if (ImGui::Button(u8"计算并复制")){
					//ImGui::SetClipboardText(const char* text);

					// if update
					coefs = fd.design();
					string coefs_output;
					coefs_output = "b = [ ";
					for (auto b : coefs[0]) {
						coefs_output += to_string(b) + " ";
					}
					coefs_output += "]\na = [ ";
					for (auto a : coefs[1]) {
						coefs_output += to_string(a) + " ";
					}
					coefs_output += "]";
					ImGui::SetClipboardText(coefs_output.data());
					ba_to_show = coefs_output;
					//std::cout << "复制了系数" << std::endl;
				}
				

				ImGui::TreePop();
			}

			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}

		// Rendering
		ImGui::EndFrame();
		DWM.DW_RENDERING();
	}

	DWM.DW_SHUTDOWN();
	ImGui::DestroyContext();

	return 0;
}


