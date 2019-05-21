#pragma once

#include "UIManager.h"
#include "../Rendering/TextRenderer.h"
#include "../Loading/PrefabLoader.h"
#include <json.hpp>
using json = nlohmann::json;

/**
Type of UIComponent that renders text to the panel.
This component overrides UIComponent sizing with the text size.
*/
class UIText : public UIComponent {
public:
	// default font type to use
    static const std::string DEFAULT_FONT;

    UIText(std::string text, float x, float y, float fontScale = 1.0f, std::string fontPath = DEFAULT_FONT);

	// Current text
	const std::string GetText() const;

	// Change the text on this component
	void SetText(std::string text);

	// Gets the text mesh
	TextMesh* GetTextMesh() const;

	// Change the text mesh (and text) on this component.
	void SetTextMesh(TextMesh* textMesh);

	// Get path to current font
	const std::string GetFont() const;

	// Path to desired font
	void SetFont(std::string font);

	// Current text alignment
	TextAlignment GetAlignment() const;

	// Set the text alignment
	void SetAlignment(TextAlignment alignment);

	// Get the font scale 
	float GetFontScale() const;

	// Set the font scale
	void SetFontScale(float scale);

	// WARNING: Not used.
	void SetSpacing(float spacing);
	
	// WARNING: Not used.
	float GetSpacing() const;

	// Size if overriden by text mesh size.
	virtual void CalculateScreenSize(const UIComponent* parent) override;

	virtual glm::mat4 GetIndividualTransform() override;

private:
    std::string   _text;
    float         _fontScale;
	float         _spacing;
	std::string   _fontPath;
	TextAlignment _alignment;
	TextMesh*     _textMesh;

private:
	static Component* Create(json json);
	static ComponentRegistrar reg;
};