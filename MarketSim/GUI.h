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
#include "database.h"
#include "jsonParcer.h"



class GUI {
private:
	char bufNameAch[256]{};
	char bufNameAs[256]{};

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
	void renderMainWindow(FPS& fps, FPVDrone& drone, BuildingSettings& build) {
		auto componentText = [](const char* text, int current, int required) {
			if (current >= required)
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));

			ImGui::Text(text, current, required);

			if (current >= required)
				ImGui::PopStyleColor();
			};

		auto comp = drone.getComp();

		ImGui::SetNextWindowSize(ImVec2(300.0f, 650.0f));
		ImGui::Begin("Fenêtre générale");

		ImGui::Checkbox("Ouvrir le magasin",&isOpen);

		if (ImGui::CollapsingHeader("Fenêtre de commande")) {
			auto equip = drone.getEquipPrice();

			ImGui::Text("Solde: %.1f", myBalance);
			ImGui::Dummy({ 0.0f,10.0f });
			ImGui::Text("Articles dans le panier: %d", card.size());
			ImGui::Dummy({ 0.0f,10.0f });


			for (const auto& [key, value] : equip) {
				//Name - Price
				if (std::ranges::find(card, key) != card.end()) {
					ImGui::TextColored({ 0.0f,0.8f,0.0f,1.0f }, "%s - %.1f$", key.c_str(), value);
				}
				else ImGui::Text("%s - %.1f$", key.c_str(), value);


				ImGui::SameLine(150);

				if (ImGui::Button(("Ajouter à panier##" + key).c_str())) card.push_back(key);
				if (ImGui::Button(("Supprimer de panier##" + key).c_str())) card.erase(
					std::remove(card.begin(), card.end(), key),
					card.end()
				);
			}
			ImGui::Dummy({ 0.0f,10.0f });
			if (ImGui::Button("Acheter tout")) {
				float totalPrice = 0.0f;

				for (auto& element : card) totalPrice += equip[element];

				if (myBalance >= totalPrice) {
					saveDataForPlayer(totalPrice, "-");

					for (auto& p : getDataForPlayer().players) myBalance = p.balance;

					drone.sendPackage(card, build);
					card.clear();
				}
				else ImGui::TextColored({ 1.0f,0.0f,0.0f,1.0f }, "Fonds insuffisants sur le solde");
			}
		}

		ImGui::Dummy({ 0.0f,10.0f });

		ImGui::Text("MIN FPS: %.0f", fps.minFps);
		ImGui::Text("MAX FPS : % .0f", fps.maxFps);
		ImGui::Text("FPS ACTUELLE: % .0f", fps.fps);

		ImGui::Dummy({ 0.0f,10.0f });

		ImGui::Text("%s prix: %.0f", getStringFromJson("DRONE_Achetee"), priceForT1);
		ImGui::Text("%s prix: % .0f", getStringFromJson("DRONE_Assemblee"), priceForT2);

		ImGui::Dummy({ 0.0f,10.0f });

		ImGui::Text("Saisir le prix pour achetee");
		ImGui::InputFloat("1##", &priceForT1);
		ImGui::Text("Saisir le prix pour assemblee");
		ImGui::InputFloat("2##", &priceForT2);
		ImGui::Text("Saisir le prix pour des lunettes");
		ImGui::InputFloat("3##", &priceForGoggles);
		ImGui::Text("Saisir le prix pour  le controlleur");
		ImGui::InputFloat("4##", &priceForController);
		ImGui::Dummy({ 0.0f,20.0f });

		ImGui::Text("Saisir le nom pour le drone assemblee");
		ImGui::InputText("5##",bufNameAs,sizeof(bufNameAs));
		if (ImGui::Button("Appliquer##")) changePlayerConfig("DRONE_Assemblee", bufNameAs);

		ImGui::Text("Saisir le nom pour le drone achetee");
		ImGui::InputText("6##", bufNameAch, sizeof(bufNameAch));
		if (ImGui::Button("Appliquer")) changePlayerConfig("DRONE_Achetee",bufNameAch);

		ImGui::Dummy({ 0.0f,20.0f });

		ImGui::Text("ASSEMLER");

		componentText("Cadre: %d / %d", comp.frame, 1);
		componentText("Camera: %d / %d", comp.camera, 1);
		componentText("Batterie: %d / %d", comp.battery, 1);
		componentText("Moteur: %d / %d", comp.motor, 4);
		componentText("Hélice: %d / %d", comp.propeller, 4);
		componentText("Controlleur de vol: %d / %d", comp.fc, 1);

		ImGui::End();
	}

	void renderAll(FPS& fps, FPVDrone& drone, BuildingSettings& build) {
		renderMainWindow(fps, drone, build);
	}
};