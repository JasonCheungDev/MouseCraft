#include "UIRoot.h"

#include "../Core/ComponentFactory.h"

UIRoot::UIRoot() : UIComponent(0,0,0,0)
{
	screenBounds = {
		0.0f,
		ScreenHeight(),
		ScreenWidth(),
		0.0f
	};

	screenSize = screenBounds.getSize();
}

UIRoot::~UIRoot()
{
}

void UIRoot::Resize()
{
	auto& children = this->GetEntity()->GetChildren();
	for (Entity* child : children)
	{
		UIComponent* comp = child->GetComponent<UIComponent>();
		if (comp != nullptr)
		{
			comp->Resize();
		}
	}
}

void UIRoot::OnInitialized()
{
	GetEntity()->transform.setLocalPosition(glm::vec3(screenBounds.getCenter(), 0.0f));
	//GetEntity()->transform.computeLocalTransformation();
	//GetEntity()->transform.computeWorldTransformation();
	
	auto& children = this->GetEntity()->GetChildren();
	for (Entity* child : children)
	{
		UIComponent* comp = child->GetComponent<UIComponent>();
		if (comp != nullptr)
		{
			comp->SetRoot(this);
		}
	}
}

Component* UIRoot::Create(nlohmann::json json)
{
	return ComponentFactory::Create<UIRoot>();
}

ComponentRegistrar UIRoot::reg("UIRoot", &UIRoot::Create);