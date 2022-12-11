#include <map>

#include "imgui/imgui.h"
#include "cProjectUI.h"

void cProjectUI::renderUI() {
	// Create a window called "Project Manager", with a menu bar.
	ImGui::Begin("Project Manager", NULL, ImGuiWindowFlags_MenuBar		| 
										  ImGuiWindowFlags_NoMove		| 
										  ImGuiWindowFlags_NoTitleBar	|
										  ImGuiWindowFlags_NoResize);
	// Menu for Project Management
	// TODO: Save Scene on XML file
	if (ImGui::BeginMenuBar()) {
		if(ImGui::BeginMenu("Project")){
			if (ImGui::MenuItem("New Scene (TODO)")) {
			}
			if (ImGui::MenuItem("Quit (TODO)")) {
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Game State")) {
			if (ImGui::MenuItem("New Game")) {
				m_projectManager->m_GameLoopState = GameState::NEW_GAME;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (ImGui::TreeNodeEx("Scenes:", ImGuiTreeNodeFlags_DefaultOpen)) {
		std::map<std::string, cScene*>::iterator itScenes;
		for (itScenes = m_projectManager->m_mScenes.begin();
			 itScenes != m_projectManager->m_mScenes.end();
			 itScenes++) {
			ImGui::Bullet();
			if (ImGui::SmallButton(itScenes->first.c_str())) {
				m_projectManager->LoadScene(itScenes->first.c_str());
				m_projectManager->isNewScene = true;
			}
			if (m_projectManager->m_selectedScene != nullptr)
				if (m_projectManager->m_selectedScene->m_name == itScenes->first) {
					ImGui::Text("  "); ImGui::SameLine();
					ImGui::Checkbox("Enable Game Loop", &m_projectManager->m_isGameLoopEnabled);
				}
		}
		ImGui::TreePop();
	}
	
	ImGui::End();
}

void cProjectUI::renderSceneUI() {
	// Scene Widget
	ImGui::Begin("Selected Scene", NULL, ImGuiWindowFlags_MenuBar		|
										 ImGuiWindowFlags_NoMove		|
										 ImGuiWindowFlags_NoTitleBar	|
										 ImGuiWindowFlags_NoResize);
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("Scene")) {
			if (ImGui::MenuItem("Save Scene")) {
				m_projectManager->SaveSelectedScene();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	// Camera Manipulation
	// First Checks if there's a selected scene
	if (m_projectManager->m_selectedScene != nullptr) {
		// Variable for ImGui to control Position
		float eye3f[3] = { m_projectManager->m_selectedScene->m_cameraEye.x,
						   m_projectManager->m_selectedScene->m_cameraEye.y,
						   m_projectManager->m_selectedScene->m_cameraEye.z };
		ImGui::Text("Camera Eye");
		// Camera Eye Slider
		if (ImGui::DragFloat3("X Y Z##EyeSlider", eye3f, 0.1f, 0.0f, 0.0f, "%.2f")) {
			m_projectManager->m_selectedScene->m_cameraEye.x = eye3f[0];
			m_projectManager->m_selectedScene->m_cameraEye.y = eye3f[1];
			m_projectManager->m_selectedScene->m_cameraEye.z = eye3f[2];
		}
		// Variable for ImGui to control Camera Target
		float target3f[3] = { m_projectManager->m_selectedScene->m_cameraTarget.x,
							  m_projectManager->m_selectedScene->m_cameraTarget.y,
							  m_projectManager->m_selectedScene->m_cameraTarget.z };
		ImGui::Text("Camera Target");
		// Camera Target Slider
		if (ImGui::DragFloat3("X Y Z##TargetSlider", target3f, 0.1f, 0.0f, 0.0f, "%.2f")) {
			m_projectManager->m_selectedScene->m_cameraTarget.x = target3f[0];
			m_projectManager->m_selectedScene->m_cameraTarget.y = target3f[1];
			m_projectManager->m_selectedScene->m_cameraTarget.z = target3f[2];
		}
	} else {
		ImGui::BulletText("No Scene Selected.");
	}
	// Checks if there's a selected scene
	//std::string lastModelID = "";
	if (ImGui::TreeNodeEx("Meshes:", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (m_projectManager->m_selectedScene != nullptr) {
			// Iterates through all models
			std::map<std::string, cMeshObject*>::iterator itMesh;
			for (itMesh = m_projectManager->m_selectedScene->m_mMeshes.begin();
				 itMesh != m_projectManager->m_selectedScene->m_mMeshes.end();
				 itMesh++) {
				/*if (lastModelID.empty()) {
					lastModelID = itMesh->second->m_parentModel->meshName;
					ImGui::TreeNode(lastModelID.c_str());
				} else if (lastModelID != itMesh->second->m_parentModel->meshName) {
					lastModelID = itMesh->second->m_parentModel->meshName;
					ImGui::TreePop();
					ImGui::TreeNode(lastModelID.c_str());
				}*/
				ImGui::Bullet();
				if (ImGui::SmallButton(itMesh->second->m_meshName.c_str())) {
					m_projectManager->m_selectedMesh = itMesh->second;
					// Internal Bool-Switch for RenderingUI
					this->isMeshSelected = true;
					this->isLightSelected = false;
				}
				// Iterates through all child models
				for (int childIndex = 0;
					childIndex < itMesh->second->vecChildMeshes.size();
					childIndex++) {
					ImGui::Indent();
					ImGui::Bullet();
					if (ImGui::SmallButton(itMesh->second->vecChildMeshes[childIndex]->m_meshName.c_str())) {
						m_projectManager->m_selectedMesh = itMesh->second->vecChildMeshes[childIndex];
						// Internal Bool-Switch for RenderingUI
						this->isMeshSelected = true;
						this->isLightSelected = false;
					}
					ImGui::Unindent();
				}
			}
			//ImGui::TreePop();
			// Iterates through all lights
			std::map<std::string, cLight*>::iterator itLight;
			for (itLight = m_projectManager->m_selectedScene->m_mLights.begin();
				 itLight != m_projectManager->m_selectedScene->m_mLights.end();
				 itLight++) {
				ImGui::Bullet();
				if (ImGui::SmallButton(itLight->first.c_str())) {
					m_projectManager->m_selectedLight = itLight->second;
					// Internal Bool-Switch for RenderingUI
					this->isLightSelected = true;
					this->isMeshSelected = false;
				}
			}
		} else {
			ImGui::BulletText("Empty.");
		}
		ImGui::TreePop();
	}

	ImGui::End();
}

void cProjectUI::renderMeshUI() {
	// Mesh Widget
	ImGui::Begin("Selected Mesh", NULL, ImGuiWindowFlags_MenuBar	|
										ImGuiWindowFlags_NoMove		|
										ImGuiWindowFlags_NoTitleBar	|
										ImGuiWindowFlags_NoResize | 
										ImGuiWindowFlags_AlwaysUseWindowPadding);
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("Mesh")) {
			if (ImGui::MenuItem("Add New Mesh (TODO)")) {
				// TODO:: Save Mesh Data
			}
			// TODO: Add models using this menu in the future?
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	// Checks if the selected mesh isnt nullptr
	if (m_projectManager->m_selectedMesh != nullptr) {
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), m_projectManager->m_selectedMesh->m_meshName.c_str());
		ImGui::SameLine();
		if (ImGui::Button("Look at")) {
			m_projectManager->m_selectedScene->m_cameraTarget = m_projectManager->m_selectedMesh->m_position;
		}
		// Variable for ImGui to control Position
		float pos3f[3] = { m_projectManager->m_selectedMesh->m_position.x,
							  m_projectManager->m_selectedMesh->m_position.y,
							  m_projectManager->m_selectedMesh->m_position.z };
		ImGui::Text("Position");
		// Position Slider
		if (ImGui::DragFloat3("X Y Z##PositionSlider", pos3f, 0.1f, 0.0f, 0.0f, "%.2f")) {
			m_projectManager->m_selectedMesh->m_position.x = pos3f[0];
			m_projectManager->m_selectedMesh->m_position.y = pos3f[1];
			m_projectManager->m_selectedMesh->m_position.z = pos3f[2];
		}
		// Variable for ImGui to control Rotation
		float rot3f[3] = { m_projectManager->m_selectedMesh->m_rotation.x,
								  m_projectManager->m_selectedMesh->m_rotation.y,
								  m_projectManager->m_selectedMesh->m_rotation.z };
		ImGui::Text("Rotation");
		// Rotation Slider
		if (ImGui::DragFloat3("X Y Z##RotationSlider", rot3f, 0.1f, 0.0f, 0.0f, "%.2f")) {
			m_projectManager->m_selectedMesh->m_rotation.x = rot3f[0];
			m_projectManager->m_selectedMesh->m_rotation.y = rot3f[1];
			m_projectManager->m_selectedMesh->m_rotation.z = rot3f[2];
		}
		ImGui::Text("Scale");
		// Scale Slider
		ImGui::DragFloat("##Scale", &m_projectManager->m_selectedMesh->m_scale, 0.01f, 0.0f, 0.0f, "%.2f");
		// RGB Checkbox
		ImGui::Text("Use RGB?"); ImGui::SameLine();
		ImGui::Checkbox("##RGB?", &m_projectManager->m_selectedMesh->m_bUse_RGBA_colour);
		// Variable for ImGui to control RGB
		float col4f[4] = { m_projectManager->m_selectedMesh->m_RGBA_colour.r,
						   m_projectManager->m_selectedMesh->m_RGBA_colour.g,
						   m_projectManager->m_selectedMesh->m_RGBA_colour.b,
						   m_projectManager->m_selectedMesh->m_RGBA_colour.a};
		// RGB Picker
		if (ImGui::ColorEdit4("R G B A##RGB", col4f, ImGuiColorEditFlags_Float)) {
			m_projectManager->m_selectedMesh->m_RGBA_colour.r = col4f[0];
			m_projectManager->m_selectedMesh->m_RGBA_colour.g = col4f[1];
			m_projectManager->m_selectedMesh->m_RGBA_colour.b = col4f[2];
			m_projectManager->m_selectedMesh->m_RGBA_colour.a = col4f[3];
		}
		ImGui::Text("Is Wireframe?"); ImGui::SameLine();
		// isWireframe Checkbox
		ImGui::Checkbox("##Wireframe?", &m_projectManager->m_selectedMesh->m_isWireframe);
		ImGui::Text("Do not Light?"); ImGui::SameLine();
		// DoNotLight Checkbox
		ImGui::Checkbox("##Light?", &m_projectManager->m_selectedMesh->m_bDoNotLight);
		ImGui::Text("Is Visible?"); ImGui::SameLine();
		// isVisible Checkbox
		ImGui::Checkbox("##Visible?", &m_projectManager->m_selectedMesh->m_bIsVisible);
		ImGui::Text("Display BoundingBox?"); ImGui::SameLine();
		// isVisible Checkbox
		ImGui::Checkbox("##Display BoundingBox", &m_projectManager->m_selectedMesh->m_displayBoundingBox);
	} else {
		ImGui::BulletText("No Mesh Selected.");
	}

	ImGui::End();
}

void cProjectUI::renderLighthUI() {
	ImGui::Begin("Selected Light", NULL, ImGuiWindowFlags_MenuBar |
										 ImGuiWindowFlags_NoMove |
										 ImGuiWindowFlags_NoTitleBar |
										 ImGuiWindowFlags_NoResize | 
										 ImGuiWindowFlags_AlwaysUseWindowPadding);
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("Light")) {
			if (ImGui::MenuItem("Save Light Data")) {
				// TODO: Save Light Data
			}
			// TODO: Add lights using this menu in the future?
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	// Checks if the selected mesh isnt nullptr
	if (m_projectManager->m_selectedLight != nullptr) {
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), m_projectManager->m_selectedLight->m_friendlyName.c_str());
		ImGui::SameLine();
		if (ImGui::Button("Look at")) {
			m_projectManager->m_selectedScene->m_cameraTarget = m_projectManager->m_selectedLight->m_position;
		}
		// Light Type
		static const char* current_item; // Combo current selected item
		// Array used on the combo
		const char* lightType[3] = { "Point Light", "Spot Light", "Directional" };
		// Sets current_item same from selected light
		switch ((cLight::eLightType)m_projectManager->m_selectedLight->m_param1.x) {
		case cLight::POINT_LIGHT:
			current_item = "Point Light";
			break;
		case cLight::SPOT_LIGHT:
			current_item = "Spot Light";
			break;
		case cLight::DIRECTIONAL_LIGHT:
			current_item = "Directional";
			break;
		}
		// Light ComboBox
		ImGui::Text("Light Type");
		if (ImGui::BeginCombo("##Light Type", current_item)) {
			for (int i = 0; i < 3; i++) {
				bool is_selected = (current_item == lightType[i]);
				if (ImGui::Selectable(lightType[i], is_selected)) {
					current_item = lightType[i];
					m_projectManager->m_selectedLight->SetLightType(cLight::eLightType(i));
				}if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		// Variable for ImGui to control Position
		float pos3f[3] = { m_projectManager->m_selectedLight->m_position.x,
						   m_projectManager->m_selectedLight->m_position.y,
						   m_projectManager->m_selectedLight->m_position.z };
		ImGui::Text("Position");
		// Position Slider
		if (ImGui::DragFloat3("X Y Z##PositionSlider", pos3f, 0.1f, 0.0f, 0.0f, "%.2f")) {
			m_projectManager->m_selectedLight->m_position.x = pos3f[0];
			m_projectManager->m_selectedLight->m_position.y = pos3f[1];
			m_projectManager->m_selectedLight->m_position.z = pos3f[2];
		}
		// Variable for ImGui to control Diffuse
		ImGui::Text("Diffuse");
		float dif3f[4] = { m_projectManager->m_selectedLight->m_diffuse.x,
						   m_projectManager->m_selectedLight->m_diffuse.y,
						   m_projectManager->m_selectedLight->m_diffuse.z,
						   m_projectManager->m_selectedLight->m_diffuse.w };
		// RGB Picker
		if (ImGui::ColorEdit4("R G B A##DiffuseRGB", dif3f, ImGuiColorEditFlags_Float)) {
			m_projectManager->m_selectedLight->m_diffuse.x = dif3f[0];
			m_projectManager->m_selectedLight->m_diffuse.y = dif3f[1];
			m_projectManager->m_selectedLight->m_diffuse.z = dif3f[2];
			m_projectManager->m_selectedLight->m_diffuse.w = dif3f[3];
		}
		if (current_item == "Spot Light" || current_item == "Directional") {
			// Variable for ImGui to control Direction
			float dir3f[3] = { m_projectManager->m_selectedLight->m_direction.x,
							   m_projectManager->m_selectedLight->m_direction.y,
							   m_projectManager->m_selectedLight->m_direction.z };
			ImGui::Text("Direction");
			// Direction Slider
			if (ImGui::DragFloat3("X Y Z##DirectionSlider", dir3f, 0.1f, 0.0f, 0.0f, "%.2f")) {
				m_projectManager->m_selectedLight->m_direction.x = dir3f[0];
				m_projectManager->m_selectedLight->m_direction.y = dir3f[1];
				m_projectManager->m_selectedLight->m_direction.z = dir3f[2];
			}
			ImGui::Text("Use RGB?"); ImGui::SameLine();
			// RGB Checkbox
			if (ImGui::Checkbox("##RGB?", &m_projectManager->m_selectedLight->m_useRGB)) {
				// If False - Set Light to White
				if (m_projectManager->m_selectedLight->m_useRGB == false) {
					m_projectManager->m_selectedLight->SetToWhite();
				}
			}
		}
		if (current_item == "Spot Light") {
			// Inner Angle
			ImGui::Text("Inner Angle");
			ImGui::SameLine();
			ImGui::DragFloat("##InnerAngleSlider", &m_projectManager->m_selectedLight->m_param1.y, 0.1f, 0.0f, 0.0f, "%.2f");
			// Outer Angle
			ImGui::Text("Outer Angle");
			ImGui::SameLine();
			ImGui::DragFloat("##OuterAngleSlider", &m_projectManager->m_selectedLight->m_param1.z, 0.1f, 0.0f, 0.0f, "%.2f");
		}
		// Variable for ImGui to control RGB
		ImGui::Text("Specular Reflection");
		float spec4f[4] = { m_projectManager->m_selectedLight->m_specular.r,
						   m_projectManager->m_selectedLight->m_specular.g,
						   m_projectManager->m_selectedLight->m_specular.b,
						   m_projectManager->m_selectedLight->m_specular.a };
		// RGB Picker
		if (ImGui::ColorEdit4("R G B A##SpecularRGB", spec4f, ImGuiColorEditFlags_Float)) {
			m_projectManager->m_selectedLight->m_specular.r = spec4f[0];
			m_projectManager->m_selectedLight->m_specular.g = spec4f[1];
			m_projectManager->m_selectedLight->m_specular.b = spec4f[2];
			m_projectManager->m_selectedLight->m_specular.a = spec4f[3];
		}
		// Attenuations
		ImGui::Text("Attenuation:");
		// Constant 
		ImGui::Text("Constant");
		ImGui::SameLine();
		ImGui::DragFloat("##ConstantInput", &m_projectManager->m_selectedLight->m_attenuation.x, 0.1f, 0.0f, 0.0f, "%.2f");
		// Linear
		ImGui::Text("Linear");
		ImGui::SameLine();
		//ImGui::PushItemWidth(-FLT_MIN);
		ImGui::DragFloat("##LinearInput", &m_projectManager->m_selectedLight->m_attenuation.y, 0.01f, 0.0f, 0.0f, "%.3f");
		//ImGui::PopItemWidth();
		// Quadratic
		ImGui::Text("Quadratic");
		ImGui::SameLine();
		ImGui::DragFloat("##QuadraticInput", &m_projectManager->m_selectedLight->m_attenuation.z, 0.01f, 0.0f, 0.0f, "%.7f");
		// Distance Cut Off
		ImGui::Text("Dist. Cut-Off");
		ImGui::SameLine();
		ImGui::DragFloat("##Cut-OffInput", &m_projectManager->m_selectedLight->m_attenuation.w, 0.001f, 0.0f, 0.0f, "%.2f");
		// Turn On/Off Checkbox
		ImGui::Text("Turn On/Off");
		ImGui::SameLine();
		bool onoff = (bool)m_projectManager->m_selectedLight->m_param2.x;
		if (ImGui::Checkbox("##OnOff", &onoff)) {
			if (onoff)
				m_projectManager->m_selectedLight->TurnOn();
			else
				m_projectManager->m_selectedLight->TurnOff();
		}
		// Show Model Checkbox
		ImGui::Text("Show Model?");
		ImGui::SameLine();
		ImGui::Checkbox("##ShowModel?", &m_projectManager->m_selectedLight->m_showModel);

	} else {
		ImGui::BulletText("No Light Selected.");
	}

	ImGui::End();
}