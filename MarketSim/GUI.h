#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "variables.h"
#include "drone.h"
#include <algorithm>
#include "database.h"
#include "jsonParcer.h"
#include <algorithm>


class GUI {
private:
	char bufNameAch[256]{};
	char bufNameAs[256]{};

	std::vector<std::string> card = {};

	bool showPerformance = false;
	bool showPrices = false;
	bool showNames = false;

	float minValue = 0.0f;
	float maxValue = 2000.0f;

	const ImVec4 colGreen = ImVec4(0.30f, 0.85f, 0.45f, 1.0f);
	const ImVec4 colRed = ImVec4(0.95f, 0.35f, 0.35f, 1.0f);
	const ImVec4 colYellow = ImVec4(1.00f, 0.80f, 0.30f, 1.0f);
	const ImVec4 colAccent = ImVec4(0.40f, 0.70f, 1.00f, 1.0f);
	const ImVec4 colDim = ImVec4(0.60f, 0.60f, 0.65f, 1.0f);

	static bool colorButton(const char* label, const ImVec4& base, const ImVec2& size) {
		ImGui::PushStyleColor(ImGuiCol_Button, base);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(base.x + 0.12f, base.y + 0.12f, base.z + 0.12f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(base.x * 0.7f, base.y * 0.7f, base.z * 0.7f, 1.0f));
		bool pressed = ImGui::Button(label, size);
		ImGui::PopStyleColor(3);
		return pressed;
	}

	void sectionTitle(const char* title) {
		ImGui::Spacing();
		ImGui::TextColored(colAccent, "%s", title);
		ImGui::Separator();
		ImGui::Spacing();
	}

	void componentText(const char* text, int current, int required) {
		bool done = current >= required;

		if (done) ImGui::PushStyleColor(ImGuiCol_Text, colGreen);
		ImGui::Text(text, current, required);
		if (done) ImGui::PopStyleColor();

		float frac = required > 0 ? (float)current / (float)required : 1.0f;
		frac = std::clamp(frac, 0.0f, 1.0f);

		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, done ? colGreen : colAccent);
		ImGui::ProgressBar(frac, ImVec2(-FLT_MIN, 6.0f), "");
		ImGui::PopStyleColor();
		ImGui::Spacing();
	}

	static void pushStyle() {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 6.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(14.0f, 12.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 5.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 7.0f));
	}
	static void popStyle() { ImGui::PopStyleVar(6); }

	bool windowToggle(const char* label, bool& state) {
		const ImVec4 base = state ? ImVec4(0.20f, 0.55f, 0.35f, 1.0f) : ImVec4(0.25f, 0.28f, 0.35f, 1.0f);
		bool pressed = colorButton(label, base, ImVec2(-FLT_MIN, 0.0f));
		if (pressed) state = !state;
		return pressed;
	}

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
		auto comp = drone.getComp();

		pushStyle();

		ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(350.0f, 650.0f), ImGuiCond_FirstUseEver);
		ImGui::Begin("Fenêtre générale");

		ImGui::Checkbox("Ouvrir le magasin", &isOpen);

		sectionTitle("Fenêtres");
		const float spacing = ImGui::GetStyle().ItemSpacing.x;
		const float third = (ImGui::GetContentRegionAvail().x - spacing * 2.0f) / 3.0f;

		{
			auto toggleSmall = [&](const char* label, bool& state, bool sameLine) {
				if (sameLine) ImGui::SameLine();
				const ImVec4 base = state ? ImVec4(0.20f, 0.55f, 0.35f, 1.0f) : ImVec4(0.25f, 0.28f, 0.35f, 1.0f);
				if (colorButton(label, base, ImVec2(third, 0.0f))) state = !state;
				};
			toggleSmall("Performance", showPerformance, false);
			toggleSmall("Prix", showPrices, true);
			toggleSmall("Noms", showNames, true);
		}

		if (ImGui::CollapsingHeader("Fenêtre de commande")) {
			auto equip = drone.getEquipPrice();

			ImGui::Spacing();
			ImGui::Text("Solde:");
			ImGui::SameLine();
			ImGui::TextColored(colYellow, "%.1f", myBalance);

			ImGui::Text("Articles dans le panier:");
			ImGui::SameLine();
			ImGui::TextColored(card.empty() ? colDim : colGreen, "%d", (int)card.size());

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			const float btnWidth = (ImGui::GetContentRegionAvail().x - spacing) * 0.5f;

			for (const auto& [key, value] : equip) {
				ImGui::PushID(key.c_str());

				//Name - Price
				if (std::ranges::find(card, key) != card.end()) {
					ImGui::TextColored(colGreen, "%s - %.1f$", key.c_str(), value);
				}
				else ImGui::Text("%s - %.1f$", key.c_str(), value);

				if (colorButton(("Ajouter à panier##" + key).c_str(), ImVec4(0.10f, 0.60f, 0.25f, 1.0f), ImVec2(btnWidth, 0.0f)))
					card.push_back(key);

				ImGui::SameLine();

				if (colorButton(("Supprimer de panier##" + key).c_str(), ImVec4(0.75f, 0.15f, 0.15f, 1.0f), ImVec2(btnWidth, 0.0f)))
					card.erase(
						std::remove(card.begin(), card.end(), key),
						card.end()
					);

				ImGui::PopID();
				ImGui::Spacing();
			}

			ImGui::Separator();
			ImGui::Spacing();

			if (colorButton("Acheter tout", ImVec4(0.15f, 0.45f, 0.85f, 1.0f), ImVec2(-FLT_MIN, 32.0f))) {
				float totalPrice = 0.0f;

				for (auto& element : card) totalPrice += equip[element];

				if (myBalance >= totalPrice) {
					saveDataForPlayer(totalPrice, "-");

					for (auto& p : getDataForPlayer().players) myBalance = p.balance;

					drone.sendPackage(card, build);
					card.clear();
				}
				else ImGui::TextColored(colRed, "Fonds insuffisants sur le solde");
			}
			ImGui::Spacing();
		}

		sectionTitle("ASSEMBLER");

		componentText("Cadre: %d / %d", comp.frame, 1);
		componentText("Camera: %d / %d", comp.camera, 1);
		componentText("Batterie: %d / %d", comp.battery, 1);
		componentText("Moteur: %d / %d", comp.motor, 4);
		componentText("Hélice: %d / %d", comp.propeller, 4);
		componentText("Controlleur de vol: %d / %d", comp.fc, 1);

		ImGui::Dummy({ 0.0f, 10.0f });
		ImGui::Text("©Developped by SVB & Red.Fpv");

		ImGui::End();

		popStyle();
	}

	void renderPerformanceWindow(FPS& fps) {
		if (!showPerformance) return;

		pushStyle();
		ImGui::SetNextWindowPos(ImVec2(380.0f, 10.0f), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(260.0f, 140.0f), ImGuiCond_FirstUseEver);

		if (ImGui::Begin("Performance", &showPerformance)) {
			ImVec4 fpsColor = fps.fps >= 60.0f ? colGreen : (fps.fps >= 30.0f ? colYellow : colRed);

			ImGui::Text("MIN FPS: %.0f", fps.minFps);
			ImGui::Text("MAX FPS : % .0f", fps.maxFps);
			ImGui::PushStyleColor(ImGuiCol_Text, fpsColor);
			ImGui::Text("FPS ACTUELLE: % .0f", fps.fps);
			ImGui::PopStyleColor();
		}
		ImGui::End();
		popStyle();
	}
	void renderPricesWindow() {
		if (!showPrices) return;

		pushStyle();
		ImGui::SetNextWindowPos(ImVec2(380.0f, 170.0f), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(340.0f, 460.0f), ImGuiCond_FirstUseEver);

		if (ImGui::Begin("Prix", &showPrices)) {
			sectionTitle("Prix des drones");

			ImGui::Text("%s prix: %.0f", getStringFromJson("DRONE_Achetee"), priceForT1);
			ImGui::Text("%s prix: % .0f", getStringFromJson("DRONE_Assemblee"), priceForT2);

			sectionTitle("Réglage des prix");

			ImGui::PushItemWidth(-FLT_MIN);

			ImGui::TextColored(colDim, "Saisir le prix pour le drone achetée");
			ImGui::InputFloat("1##", &priceForT1);
			ImGui::TextColored(colDim, "Saisir le prix pour le drone assemblée");
			ImGui::InputFloat("2##", &priceForT2);
			ImGui::TextColored(colDim, "Saisir le prix pour des lunettes");
			ImGui::InputFloat("3##", &priceForGoggles1);
			ImGui::TextColored(colDim, "Saisir le prix pour des lunettes2");
			ImGui::InputFloat("4##", &priceForGoggles2);
			ImGui::TextColored(colDim, "Saisir le prix pour le controlleur");
			ImGui::InputFloat("5##", &priceForController1);
			ImGui::TextColored(colDim, "Saisir le prix pour le controlleur  CTR1");
			ImGui::InputFloat("6##", &priceForController2);

			priceForT1 = std::clamp(priceForT1, minValue, maxValue);
			priceForT2 = std::clamp(priceForT2, minValue, maxValue);
			priceForGoggles1 = std::clamp(priceForGoggles1, minValue, maxValue);
			priceForGoggles2 = std::clamp(priceForGoggles2, minValue, maxValue);
			priceForController1 = std::clamp(priceForController1, minValue, maxValue);
			priceForController2 = std::clamp(priceForController2, minValue, maxValue);

			ImGui::PopItemWidth();
		}
		ImGui::End();
		popStyle();
	}
	void renderNamesWindow() {
		if (!showNames) return;

		pushStyle();
		ImGui::SetNextWindowPos(ImVec2(740.0f, 10.0f), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(320.0f, 250.0f), ImGuiCond_FirstUseEver);

		if (ImGui::Begin("Noms des drones", &showNames)) {
			ImGui::PushItemWidth(-FLT_MIN);

			ImGui::TextColored(colDim, "Saisir le nom pour le drone assemblée");
			ImGui::InputText("7##", bufNameAs, sizeof(bufNameAs));
			if (colorButton("Appliquer##", ImVec4(0.20f, 0.50f, 0.80f, 1.0f), ImVec2(-FLT_MIN, 0.0f)))
				changePlayerConfig("DRONE_Assemblee", bufNameAs);

			ImGui::Spacing();

			ImGui::TextColored(colDim, "Saisir le nom pour le drone achetée");
			ImGui::InputText("8##", bufNameAch, sizeof(bufNameAch));
			if (colorButton("Appliquer", ImVec4(0.20f, 0.50f, 0.80f, 1.0f), ImVec2(-FLT_MIN, 0.0f)))
				changePlayerConfig("DRONE_Achetee", bufNameAch);

			ImGui::PopItemWidth();
		}
		ImGui::End();
		popStyle();
	}

	void renderAll(FPS& fps, FPVDrone& drone, BuildingSettings& build) {
		renderMainWindow(fps, drone, build);
		renderPerformanceWindow(fps);
		renderPricesWindow();
		renderNamesWindow();
	}
};