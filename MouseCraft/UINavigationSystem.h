#pragma once

#include "Core/System.h"
#include "Event/EventManager.h"
#include "UISelectable.h"

class UINavigationSystem : public System, public ISubscriber
{
public:
	UINavigationSystem();
	~UINavigationSystem();
	void Update(float dt) override;
	virtual void Notify(EventName eventName, Param *params) override;
	void Select(std::string id);
	UISelectable* GetSelected() const { return current; }

private:
	std::map<std::string, UISelectable*> selectableMap;
	UISelectable* current;
};

