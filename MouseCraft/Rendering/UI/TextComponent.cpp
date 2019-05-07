#include "TextComponent.h"

#include "..\..\Core\Entity.h"

void TextComponent::setText(std::string s)
{
	text = s;
}

std::string TextComponent::getText()
{
	return text;
}

void TextComponent::clear()
{
	text = "";
}

glm::vec2 TextComponent::getPosition()
{
	return GetEntity()->transform.getWorldPosition();
}
