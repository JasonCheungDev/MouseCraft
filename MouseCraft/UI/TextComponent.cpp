#include "TextComponent.h"
#include "../Graphics/ModelGen.h"

const std::string TxtComponent::DEFAULT_FONT = "res/fonts/arial.ttf";

TxtComponent::TxtComponent(std::string text, float fontSize, float x, float y, std::string fontPath) :
    UIComponent(0, 0, x, y), _text(text), _fontScale(fontSize), _spacing(1.0f), _fontPath(fontPath) 
{
	_textMesh = TextRenderer::Instance().GenerateTextMesh(text, _fontPath, _alignment);
	valid = false;
}

const std::string TxtComponent::GetText() const
{
	return  _text;
}

void TxtComponent::SetText(std::string text) 
{
	// If text is different, invalidate this UIComponent to be Resized
    if (text != _text) 
	{ 
        _text = text;
		_textMesh = TextRenderer::Instance().GenerateTextMesh(text, _fontPath, _alignment);
        valid = false;
    }
}

TextMesh * TxtComponent::GetTextMesh() const
{
	return _textMesh;
}

void TxtComponent::SetTextMesh(TextMesh * textMesh)
{
	// If text is different, invalidate this UIComponent to be Resized
	if (textMesh->Text != _text)
	{
		_text = textMesh->Text;
		_fontPath = textMesh->Font;
		_textMesh = textMesh;
		valid = false;
	}
}

const std::string TxtComponent::GetFont() const
{
	return _fontPath;
}

void TxtComponent::SetFont(std::string font)
{
	if (_fontPath != font)
	{
		_fontPath = font;

		// Invalidate this UIComponent to be resized as font may change text size.
		if (_textMesh)
		{
			_textMesh = TextRenderer::Instance().GenerateTextMesh(_text, _fontPath, _alignment);
		}
	}
}

TextAlignment TxtComponent::GetAlignment() const
{
	return _alignment;
}

void TxtComponent::SetAlignment(TextAlignment alignment)
{
	_alignment = alignment;
	_textMesh = TextRenderer::Instance().GenerateTextMesh(_text, _fontPath, _alignment);
	// changing alignment will not change size (no need to resize).
}

float TxtComponent::GetFontScale() const
{
	return _fontScale;
}

void TxtComponent::SetFontScale(float scale)
{
	_fontScale = scale;
	valid = false;
}

void TxtComponent::SetSpacing(float spacing)
{
	_spacing = spacing;
}

float TxtComponent::GetSpacing() const
{
	return _spacing;
}

void TxtComponent::CalculateScreenSize(const UIComponent * parent)
{
	screenSize = (_textMesh != nullptr) ? _textMesh->Size * _fontScale : glm::vec2();
}