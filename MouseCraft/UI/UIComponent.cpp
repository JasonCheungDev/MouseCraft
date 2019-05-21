#include "UIComponent.h"
#include "../Rendering/ModelGen.h"
#include <iostream>
#include "../Core/OmegaEngine.h"
#include "../Core/ComponentFactory.h"

const float UIComponent::Z_STEP = 0.001f;

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
    widthType = UNIT_PERCENT;
    heightType = UNIT_PERCENT;
    aspectRatio = 1;

    ClickAction = "";
	zOverride = 0;

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

		// ===== Setup entity transform (this allows group rotation and scaling) =====

		// Transfer screen position to entity local position 
		auto currentPosition = GetEntity()->transform.getLocalPosition();
		auto localPosition = screenBounds.getCenter() - parentBounds.getCenter();
		GetEntity()->transform.setLocalPosition(glm::vec3(localPosition, 0));
		screenZ = parent->screenZ + Z_STEP;

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
	switch (widthType)
	{
	case UNIT_PIXEL:
		screenSize.x = size.x;
		break;
	case UNIT_PERCENT:
		screenSize.x = parent->screenSize.x * size.x;
		break;
	}

	switch (heightType)
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
	if (widthType == UNIT_SCALE) {
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

void UIComponent::InitalizeFromJson(json json)
{
	auto width = json["width"].get<std::string>();
	size_t found = width.find('%');
	if (found != std::string::npos)
	{
		widthType = UNIT_PERCENT;
		width.erase(found, 1);
		size.x = stof(width) / 100.0f;
	}
	else
	{
		widthType = UNIT_PIXEL;
		size.x = stof(width);
	}

	auto height = json["height"].get<std::string>();
	found = height.find('%');
	if (found != std::string::npos)
	{
		heightType = UNIT_PERCENT;
		height.erase(found, 1);
		size.y = stof(height) / 100.0f;
	}
	else
	{
		heightType = UNIT_PIXEL;
		size.y = stof(height);
	}

	auto offsetX = json["x"].get<std::string>();
	found = offsetX.find('%');
	if (found != std::string::npos)
	{
		anchorXType = ANCHOR_PERCENT;
		offsetX.erase(found, 1);
		anchorOffset.x = stof(offsetX) / 100.0f;
	}
	else
	{
		anchorXType = ANCHOR_PIXEL;
		anchorOffset.x = stof(offsetX);
	}

	auto offsetY = json["y"].get<std::string>();
	found = offsetY.find('%');
	if (found != std::string::npos)
	{
		anchorYType = ANCHOR_PERCENT;
		offsetY.erase(found, 1);
		anchorOffset.y = stof(offsetY) / 100.0f;
	}
	else
	{
		anchorYType = ANCHOR_PIXEL;
		anchorOffset.y = stof(offsetY);
	}

	if (json.find("anchor") != json.end())
	{
		auto ha = json["anchor"][0].get<std::string>();
		if (ha == "left")
			hAnchor = HorizontalAnchor::ANCHOR_LEFT;
		else if (ha == "center")
			hAnchor = HorizontalAnchor::ANCHOR_HCENTER;
		else if (ha == "right")
			hAnchor = HorizontalAnchor::ANCHOR_RIGHT;
		auto va = json["anchor"][1].get<std::string>();
		if (va == "top")
			vAnchor = VerticalAnchor::ANCHOR_TOP;
		else if (va == "center")
			vAnchor = VerticalAnchor::ANCHOR_VCENTER;
		else if (va == "bottom")
			vAnchor = VerticalAnchor::ANCHOR_BOTTOM;
	}

	if (json.find("color") != json.end())
	{
		color = Color(json["color"][0].get<float>(), 
			json["color"][1].get<float>(), 
			json["color"][2].get<float>(), 
			json["color"][3].get<float>());
	}

	if (json.find("z") != json.end())
	{
		zOverride = json["z"].get<float>();
	}
}

bool UIComponent::CompareZOrder(const UIComponent * lhs, const UIComponent * rhs)
{
	auto rightZ = (rhs->zOverride != 0) ? rhs->zOverride : rhs->screenZ;
	auto leftZ = (rhs->zOverride != 0) ? lhs->zOverride : lhs->screenZ;
	return leftZ < rightZ;
}

float UIComponent::ScreenWidth()
{
	return OmegaEngine::Instance().getWindow()->getWidth();
}

float UIComponent::ScreenHeight()
{
	return OmegaEngine::Instance().getWindow()->getHeight();
}

Component * UIComponent::Create(json json)
{
	auto ui = ComponentFactory::Create<UIComponent>(0, 0, 0, 0);

	ui->InitalizeFromJson(json);

	return ui;
}

ComponentRegistrar UIComponent::reg("UI", &UIComponent::Create);