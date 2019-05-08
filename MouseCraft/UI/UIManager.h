#pragma once

#include <map>
#include <string>
#ifndef NOMINMAX
#define NOMINMAX 1
#endif
#include <windows.h>
#include "UIComponent.h"
#include "../Core/System.h"

/**
Static structure used to store the UI data, and methods to interact with UI
*/
class UIManager : public System 
{
public:
	UIManager();

	void Update(float dt) override;

	// Process a click and execute any relevant click functions
	void ClickUI(UIComponent* root, float x, float y);

	// Define an on-click function passing in its label and the function to execute
    void DefineClickFunction(const std::string name, void(*f)());

private:
	// Find the top element under your cursor that has an on-click event and store it in the top pointer location
	void findTopClick(UIComponent** top, UIComponent* comp, const float x, const float y);

	// Is the given point in the defined rectangle
    bool pointInRect(float px, float py, float rTop, float rRight, float rLeft, float rBottom);

	// Map of all global functions that can be invoked
    std::map<const std::string, void(*)()> _clickFunctions;
};