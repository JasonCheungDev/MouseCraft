#include "UISelectable.h"

UISelectable::UISelectable(std::string id, std::string data)
	: id(id), activationData(data)
{
	OnSelected.Attach(handleDefaultOnSelect);
	OnDeselected.Attach(handleDefaultOnDeselect);
}

UISelectable::UISelectable(std::string id, std::string up, std::string down, std::string left, std::string right, std::string data)
	: id(id), up(up), down(down), left(left), right(right), activationData(data)
{
	OnSelected.Attach(handleDefaultOnSelect);
	OnDeselected.Attach(handleDefaultOnDeselect);
}

void UISelectable::OnInitialized()
{
	ui = GetEntity()->GetComponent<UIComponent>();
}

void UISelectable::DefaultOnSelected()
{
	if (ui)
	{
		ui->color = Color(selectedColor.r, selectedColor.g, selectedColor.b, selectedColor.a);
		ui->GetEntity()->transform.scale(1.2f);
	}
}

void UISelectable::DefaultOnDeselect()
{
	if (ui)
	{
		ui->color = Color(deselectedColor.r, deselectedColor.g, deselectedColor.b, deselectedColor.a);
		ui->GetEntity()->transform.scale(1.0f/1.2f);
	}
}

Component* UISelectable::Create(json json)
{
	std::string id = json["id"].get<std::string>();
	
	std::string up = "";
	if (json.find("up") != json.end())
		up = json["up"].get<std::string>();

	std::string down = "";
	if (json.find("down") != json.end())
		down = json["down"].get<std::string>();

	std::string left = "";
	if (json.find("left") != json.end())
		left = json["left"].get<std::string>();

	std::string right = "";
	if (json.find("right") != json.end())
		right = json["right"].get<std::string>();

	std::string data = "";
	if (json.find("data") != json.end())
		data = json["data"].get<std::string>();

	if (json.find("data") != json.end())
		data = json["data"].get<std::string>();

	auto ui = ComponentFactory::Create<UISelectable>(id, up,down,left,right,data);

	if (json.find("default") != json.end())
		ui->defaultSelect = json["default"].get<int>();

	return ui;
}

ComponentRegistrar UISelectable::reg("UISelectable", &UISelectable::Create);