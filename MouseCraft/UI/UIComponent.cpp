#include "UIComponent.h"
#include "../Graphics/ModelGen.h"
#include <iostream>
#include "../Core/OmegaEngine.h"

UIComponent::UIComponent(float width, float height, float x, float y) :
    size(width, height), anchorOffset(x, y) {

    // Set default values for a panel
	id = "";
    visible = true;
	valid = true;
    vAnchor = ANCHOR_TOP;
    hAnchor = ANCHOR_LEFT;
    anchorXType = ANCHOR_PIXEL;
    anchorYType = ANCHOR_PIXEL;
    xType = UNIT_PERCENT;
    yType = UNIT_PERCENT;
    aspectRatio = 1;

    ClickAction = "";
	zForce = -1;
	z = 0;

	color = Color(1.0f, 1.0f, 1.0f, 1.0f);

    // Initialize Renderable data with a standard quad
	// models = std::vector<Model*>();
}

UIComponent::~UIComponent() {
	//for (Model* model : models) {
	//	delete model;
	//}
}

void UIComponent::Resize() 
{
	if (this->GetEntity() != nullptr) 
	{
		// get parent size  
		Rect parentBounds;
		glm::vec2 parentSize; // = { 0.0f, 0.0f };
		Entity* parent = this->GetEntity()->GetParent();
		if (parent != nullptr && parent->GetComponent<UIComponent>() != nullptr) 
		{
			UIComponent* cmp = parent->GetComponent<UIComponent>();
			parentSize = glm::vec2(cmp->screenBounds.getWidth(), cmp->screenBounds.getHeight());
			parentBounds = cmp->screenBounds;
		}
		else
		{
			auto w = OmegaEngine::Instance().getWindow();
			parentSize = glm::vec2(w->getWidth(), w->getHeight());
			parentBounds = { 0, parentSize.y, parentSize.x, 0 };
		}

		// TODO: REMOVE
		if (parentSize == glm::vec2())
		{
			std::cerr << "ERROR: UI BORKED" << std::endl;
			return;
		}

		// Calculate pixel size of panel based on Unit Type
		switch (xType) 
		{
		case UNIT_PIXEL:
			screenSize.x = size.x;
			break;
		case UNIT_PERCENT:
			screenSize.x = parentSize.x * size.x;
			break;
		}
		
		switch (yType) 
		{
		case UNIT_PIXEL:
			screenSize.y = size.y;
			break;
		case UNIT_PERCENT:
			screenSize.y = parentSize.y * size.y;
			break;
		case UNIT_SCALE:
			screenSize.y = screenSize.x / aspectRatio;
			break;
		}
		if (xType == UNIT_SCALE) {
			screenSize.x = screenSize.y * aspectRatio;
		}

		// Calculate screen bounds 
		switch (hAnchor)
		{
		case ANCHOR_LEFT:
			screenBounds.left  = parentBounds.left;
			screenBounds.right = screenBounds.left + screenSize.x;
			break;
		case ANCHOR_HCENTER:
			screenBounds.left  = parentBounds.getXCenter() - (screenSize.x / 2);
			screenBounds.right = parentBounds.getXCenter() + (screenSize.x / 2);
			break;
		case ANCHOR_RIGHT:
			screenBounds.left  = parentBounds.right - screenSize.x;
			screenBounds.right = parentBounds.right;
			break;
		}

		switch (vAnchor) 
		{
		case ANCHOR_BOTTOM:
			screenBounds.bottom = parentBounds.bottom;
			screenBounds.top = screenBounds.bottom + screenSize.y;
			break;
		case ANCHOR_VCENTER:
			screenBounds.bottom = parentBounds.getYCenter() - (screenSize.y / 2);
			screenBounds.top    = parentBounds.getYCenter() + (screenSize.y / 2);
			break;
		case ANCHOR_TOP:
			screenBounds.bottom = parentBounds.top - screenSize.y;
			screenBounds.top = parentBounds.top;
			break;
		}

		// Apply offset 
		float xPixelOffset = (anchorXType == ANCHOR_PERCENT) ? anchorOffset.x * parentSize.x : anchorOffset.x;
		float yPixelOffset = (anchorYType == ANCHOR_PERCENT) ? anchorOffset.y * parentSize.y : anchorOffset.y;
		screenBounds.left += xPixelOffset;
		screenBounds.right += xPixelOffset;
		screenBounds.top += yPixelOffset;
		screenBounds.bottom += yPixelOffset;

		// Generate vertices of quad from position and size of panel
		// setupModels();

		// Iterate resize on child panels
		auto children = this->GetEntity()->GetChildren();
		for (Entity *child : children) 
		{
			UIComponent* comp = child->GetComponent<UIComponent>();
			if (comp != nullptr) 
			{
				comp->Resize();
			}
		}

		valid = true;
	}
}

void UIComponent::setupModels() {
	//models.clear();
	//models.push_back(ModelGen::makeQuad(ModelGen::Axis::Z, screenSize.x, screenSize.y));

	//// Shenanigans to set the position independent of parent location
	//// This is fine since the parents are resized first
	//glm::vec3 parentPos = { 0.0f, 0.0f, 0.0f };
	//Entity* parent = this->GetEntity()->GetParent();
	//if (parent != nullptr) {
	//	parentPos = parent->transform.getWorldPosition();
	//}
	//this->GetEntity()->transform.setLocalPosition(glm::vec3(
	//	screenPosition.x + screenSize.x / 2 - parentPos.x,
	//	screenPosition.y + screenSize.y / 2 - parentPos.y,
	//	z - parentPos.z));
}

bool UIComponent::IsTransparent() {
    return color.getAlpha() < 1.0f;
}