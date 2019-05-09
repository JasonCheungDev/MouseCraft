#pragma once

#include "UIComponent.h"

// Root element for UIComponents. 
// Controls visibility and transparency of all children.
// Always the size of the window. Cannot be resized, ignores parent.
class UIRoot : public UIComponent
{
public:
	UIRoot();
	~UIRoot();

	// prevent resizing
	virtual void Resize() override {};

	void OnInitialized() override;

	// This is the root, don't allow change. 
	void SetRoot(UIRoot* root) override {};

	// This is the root, return self.
	virtual UIRoot* GetRoot() override { return this; };
};

