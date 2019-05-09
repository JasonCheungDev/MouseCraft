#include "UIComponent.h"
#include "../Rendering/ModelGen.h"
#include <iostream>
#include "../Core/OmegaEngine.h"

UIComponent::UIComponent(float width, float height, float x, float y) :
    size(width, height), anchorOffset(x, y) 
{
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
}

UIComponent::~UIComponent() 
{
}

void UIComponent::Resize() 
{
	if (this->GetEntity() != nullptr) 
	{
		// Get parent size  
		Entity* parentEntity = this->GetEntity()->GetParent();
		UIComponent* parent = nullptr;
		if (parentEntity) 
		{
			parent = parentEntity->GetComponent<UIComponent>();
			
		}
		
		if (parentEntity == nullptr || parent == nullptr)
		{
			std::cerr << "ERROR: UIComponent cannot resize, it has no parent!" << std::endl;
			return;
		}

		// Calculate screen size
		CalculateScreenSize(parent);
		
		// Calculate screen bounds 
		Rect parentBounds = parent->screenBounds;
		switch (hAnchor)
		{
		case ANCHOR_LEFT:
			screenBounds.left	= parentBounds.left;
			screenBounds.right	= screenBounds.left + screenSize.x;
			break;
		case ANCHOR_HCENTER:
			screenBounds.left	= parentBounds.getXCenter() - (screenSize.x / 2);
			screenBounds.right	= parentBounds.getXCenter() + (screenSize.x / 2);
			break;
		case ANCHOR_RIGHT:
			screenBounds.left	= parentBounds.right - screenSize.x;
			screenBounds.right	= parentBounds.right;
			break;
		}

		switch (vAnchor) 
		{
		case ANCHOR_BOTTOM:
			screenBounds.bottom = parentBounds.bottom;
			screenBounds.top	= screenBounds.bottom + screenSize.y;
			break;
		case ANCHOR_VCENTER:
			screenBounds.bottom = parentBounds.getYCenter() - (screenSize.y / 2);
			screenBounds.top    = parentBounds.getYCenter() + (screenSize.y / 2);
			break;
		case ANCHOR_TOP:
			screenBounds.bottom = parentBounds.top - screenSize.y;
			screenBounds.top	= parentBounds.top;
			break;
		}

		// Apply offset 
		float xPixelOffset		= (anchorXType == ANCHOR_PERCENT) ? anchorOffset.x * parentBounds.getWidth() : anchorOffset.x;
		float yPixelOffset		= (anchorYType == ANCHOR_PERCENT) ? anchorOffset.y * parentBounds.getHeight() : anchorOffset.y;
		screenBounds.left		+= xPixelOffset;
		screenBounds.right		+= xPixelOffset;
		screenBounds.top		+= yPixelOffset;
		screenBounds.bottom		+= yPixelOffset;

		// ===== Setup entity transform (this allows entity rotation and scaling) =====

		// Transfer screen position to entity local position 
		auto localPosition = screenBounds.getCenter() - parentBounds.getCenter();
		GetEntity()->transform.setLocalPosition(glm::vec3(localPosition, 0));

		// ===== Iterate resize on child panels =====
		auto& children = this->GetEntity()->GetChildren();
		for (Entity* child : children) 
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

void UIComponent::CalculateScreenSize(const UIComponent * parent)
{
	// Calculate pixel size of panel based on Unit Type
	switch (xType)
	{
	case UNIT_PIXEL:
		screenSize.x = size.x;
		break;
	case UNIT_PERCENT:
		screenSize.x = parent->screenSize.x * size.x;
		break;
	}

	switch (yType)
	{
	case UNIT_PIXEL:
		screenSize.y = size.y;
		break;
	case UNIT_PERCENT:
		screenSize.y = parent->screenSize.y * size.y;
		break;
	case UNIT_SCALE:
		screenSize.y = parent->screenSize.x / aspectRatio;
		break;
	}
	if (xType == UNIT_SCALE) {
		screenSize.x = parent->screenSize.y * aspectRatio;
	}
}

glm::mat4 UIComponent::GetTransform()
{
	return GetEntity()->transform.getWorldTransformation();
}

glm::mat4 UIComponent::GetIndividualTransform()
{
	// INFO: half-size as the quad is -1...1 (instead of 0...1, kinda jank).
	return glm::scale(glm::mat4(1.0f), glm::vec3(screenSize, 0.0f) * 0.5f);
}

void UIComponent::SetRoot(UIRoot * root)
{
	this->root = root;
	auto& children = this->GetEntity()->GetChildren();
	for (Entity* child : children)
	{
		UIComponent* comp = child->GetComponent<UIComponent>();
		if (comp != nullptr)
		{
			comp->SetRoot(root);
		}
	}
}

bool UIComponent::IsTransparent() 
{
    return color.getAlpha() < 1.0f;
}

UIRoot * UIComponent::GetRoot()
{
	return this->root;
}

void UIComponent::OnInitialized()
{
	auto parent = GetEntity()->GetParent();
	if (parent)
	{
		auto uic = parent->GetComponent<UIComponent>();
		if (uic) SetRoot(uic->GetRoot());
	}
}

float UIComponent::ScreenWidth()
{
	return OmegaEngine::Instance().getWindow()->getWidth();
}

float UIComponent::ScreenHeight()
{
	return OmegaEngine::Instance().getWindow()->getHeight();
}