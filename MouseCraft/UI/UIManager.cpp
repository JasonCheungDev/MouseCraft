#include "UIManager.h"

#include <stack>
#include <iostream>
#include <vector>
#include "ImageComponent.h"
#include "TextComponent.h"
#include "../ComponentMan.h"
#include "Canvas.h"

UIManager::UIManager(){}

void UIManager::Update(float dt) 
{
	auto canvases = ComponentMan<UIRoot>::Instance().All();
	for (auto& canvas : canvases)
		if (canvas->GetActive())
			canvas->Resize();
}

void UIManager::ClickUI(UIComponent* root, float x, float y)
{
	std::cerr << "WARNING: UIManager ClickUI() functionality is not implemented yet!" << std::endl;
	return;

	// TODO: need to handle rotation correctly
	UIComponent* select = root;
	findTopClick(&select, root, x, root->screenSize.y - y);
	if (select != root) {
		_clickFunctions[select->ClickAction]();
	}
}

void UIManager::DefineClickFunction(const std::string name, void(*f)()) 
{
	_clickFunctions[name] = f;
}

void UIManager::findTopClick(UIComponent** top, UIComponent* comp, const float x, const float y) 
{	
	std::cerr << "WARNING: UIManager findTopClick() functionality is not implemented yet!" << std::endl;
	return;
	
	// TODO: need to handle rotation correctly
	//for (Entity* e : comp->GetEntity()->GetChildren()) {
	//	UIComponent* c = e->GetComponent<UIComponent>();
	//	if (c->visible && pointInRect(x, y, c->screenPosition.y + c->screenSize.y,
	//		c->screenPosition.x + c->screenSize.x, c->screenPosition.x, c->screenPosition.y)) {
	//		if (c->ClickAction != "" && c->z > (*top)->z) {
	//			*top = c;
	//		}
	//		findTopClick(top, c, x, y);
	//	}
	//}
}

bool UIManager::pointInRect(float px, float py, float rTop, float rRight, float rLeft, float rBottom) 
{
	// TODO: Use Rect instead 
    return (px > rLeft && px < rRight && py < rTop && px > rBottom);
}