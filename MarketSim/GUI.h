#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "variables.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "drone.h"
#include <algorithm>

class GUI {
private:
	bool orderWin = false;
	std::vector<std::string> card = {};
public:
	void destroyGui() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
	void setupGui(GLFWwindow* window) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

		ImGui::StyleColorsDark();
	}
	void renderOrderWindow(FPVDrone& drone, BuildingSettings& build) {
		ImGui::Begin("Order window");

		auto equip = drone.getEquipPrice();

		ImGui::Text("Balance: %.1f", myBalance);
		ImGui::Dummy({0.0f,10.0f});
		ImGui::Text("Items in card: %d", card.size());
		ImGui::Dummy({ 0.0f,10.0f });


		for (const auto& [key, value] : equip) {
			//Name - Price
			if (std::ranges::find(card, key) != card.end()) {
				ImGui::TextColored({0.0f,0.8f,0.0f,1.0f}, "%s - %.1f$", key.c_str(), value);
			}
			else ImGui::Text("%s - %.1f$", key.c_str(), value);
			

			ImGui::SameLine(150);

			if (ImGui::Button(("Add to cart##" + key).c_str())) card.push_back(key);
			if (ImGui::Button(("Delete from cart##" + key).c_str())) card.erase(
				std::remove(card.begin(), card.end(), key),
				card.end()
			);
		}
		ImGui::Dummy({ 0.0f,10.0f });
		if (ImGui::Button("Buy all")) {
			float totalPrice = 0.0f;

			for (auto& element : card) totalPrice += equip[element];
			
			if (myBalance >= totalPrice) { 
				myBalance -= totalPrice;
				drone.sendPackage(card, build); 
				card.clear();
			}
			else ImGui::TextColored({1.0f,0.0f,0.0f,1.0f}, "NOT ENOUGHT MONEY IN BALANCE");
		}

		ImGui::End();
	}
	void renderMainWindow(FPS& fps) {
		ImGui::SetNextWindowSize(ImVec2(300.0f, 250.0f));
		ImGui::Begin("Main settings");

		if (ImGui::Button("Order menu")) orderWin = true;
		ImGui::Dummy({ 0.0f,10.0f });

		ImGui::Text("MIN FPS: %.0f",fps.minFps);
		ImGui::Text("MAX FPS : % .0f",fps.maxFps);
		ImGui::Text("CURRENT FPS : % .0f",fps.fps);

		ImGui::End();
	}

	void renderAll(FPS& fps, FPVDrone& drone, BuildingSettings& build) {
		renderMainWindow(fps);

		if (orderWin) renderOrderWindow(drone,build);
	}
};