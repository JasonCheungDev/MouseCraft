#pragma once

#include <map>
#include <glm/vec4.hpp>
#include <json.hpp>
#include "Core/Component.h"
#include "Core/ComponentFactory.h"
#include "Loading/PrefabLoader.h"
#include "Event/Subject.h"
#include "Event/Handler.h"
#include "UI/UIComponent.h"

using json = nlohmann::json;

class UISelectable : public Component
{
public:
	UISelectable(std::string id, std::string data = "");

	UISelectable(std::string id,
		std::string up, 
		std::string down,
		std::string left,
		std::string right,
		std::string data = "");;
	
	void OnInitialized() override;

	std::string id;
	std::string up,down,left,right;	
	std::string activationData;
	Subject<> OnSelected, OnDeselected; 
	Subject<std::string> OnActivated;
	glm::vec4 selectedColor = glm::vec4(1, 1, 1, 1);
	glm::vec4 deselectedColor = glm::vec4(1, 1, 1, 0.5f);
	int defaultSelect = -1;	// Set to any value greater than 0 to have this selected if nothing else is. The highest value enabled component will be selected. 

protected:
	virtual void DefaultOnSelected();
	virtual void DefaultOnDeselect();

private:
	Handler<UISelectable> handleDefaultOnSelect = Handler<UISelectable>(this, &UISelectable::DefaultOnSelected);
	Handler<UISelectable> handleDefaultOnDeselect = Handler<UISelectable>(this, &UISelectable::DefaultOnDeselect);
	UIComponent* ui;

private:
	static Component* Create(json json);
	static ComponentRegistrar reg;
};

