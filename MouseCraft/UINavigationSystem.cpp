#include "UINavigationSystem.h"

#include "Core/Component.h"
#include "Input/InputSystem.h"

UINavigationSystem::UINavigationSystem()
{
	EventManager::Subscribe(EventName::COMPONENT_ADDED, this);
	EventManager::Subscribe(EventName::COMPONENT_REMOVED, this);
	EventManager::Subscribe(EventName::INPUT_BUTTON, this);
}


UINavigationSystem::~UINavigationSystem()
{
	EventManager::Unsubscribe(EventName::COMPONENT_ADDED, this);
	EventManager::Unsubscribe(EventName::COMPONENT_REMOVED, this);
	EventManager::Unsubscribe(EventName::INPUT_BUTTON, this);

}

void UINavigationSystem::Update(float dt)
{

}

void UINavigationSystem::Notify(EventName eventName, Param	* params)
{
	if (eventName == EventName::COMPONENT_ADDED)
	{
		auto data = static_cast<TypeParam<Component*>*>(params)->Param;
		auto uis = dynamic_cast<UISelectable*>(data);
		if (uis)
		{
			selectableMap[uis->id] = uis;
		}
	}
	else if (eventName == EventName::COMPONENT_REMOVED)
	{
		auto data = static_cast<TypeParam<Component*>*>(params)->Param;
		auto uis = dynamic_cast<UISelectable*>(data);
		if (uis)
		{
			if (uis == current)
				current = nullptr;
			selectableMap.erase(uis->id);
		}
	}
	else if (eventName == EventName::INPUT_BUTTON)
	{
		auto data = static_cast<TypeParam<ButtonEvent>*>(params)->Param;
		
		if (!data.isDown) return;

		// nothing is selected, try to default select.
		if (!current)
		{
			if (data.button == Button::UP || data.button == Button::DOWN || data.button == Button::LEFT || data.button == Button::RIGHT)
			{
				UISelectable* candidate = nullptr;
				for (auto& kvp : selectableMap)
					if (kvp.second->defaultSelect > 0)
						if (!candidate || kvp.second->defaultSelect > candidate->defaultSelect)
							candidate = kvp.second;

				if (!candidate)
					std::cout << "WARNING: UINavigationSystem - Nothing is selected and can be default selected!" << std::endl;
				else
					Select(candidate->id);
			}
			return;
		}

		switch (data.button)
		{
		case Button::UP:
			if (current->up != "")
				Select(current->up);
			break;
		case Button::DOWN:
			if (current->down != "")
				Select(current->down);
			break;
		case Button::LEFT:
			if (current->left != "")
				Select(current->left);
			break;
		case Button::RIGHT:
			if (current->right != "")
				Select(current->right);
			break;
		case Button::SOUTH:
			if (current)
				current->OnActivated.Notify(current->activationData);
			break;
		default:
			break;
		}
	}
}

void UINavigationSystem::Select(std::string id)
{
	if (current) 
		current->OnDeselected.Notify();
	
	auto uis = selectableMap[id];
	uis->OnSelected.Notify();
	current = uis;

	std::cout << "selected: " << uis->id << std::endl;
}
