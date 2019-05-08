#pragma once

#include "UIComponent.h"

// Root element for UIComponents. 
// Controls visibility and transparency of all children.
// Always the size of the window. Cannot be resized, ignores parent.
class Canvas : public UIComponent
{
public:
	Canvas();
	~Canvas();

	// prevent resizing
	virtual void Resize() override {};

	void OnInitialized() override;
};

