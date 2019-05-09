#include "UIText.h"
#include "../Graphics/ModelGen.h"

const std::string UIText::DEFAULT_FONT = "res/fonts/arial.ttf";

UIText::UIText(std::string text, float fontSize, float x, float y, std::string fontPath) :
    UIComponent(0, 0, x, y), _text(text), _fontScale(fontSize), _spacing(1.0f), _fontPath(fontPath) 
{
	_textMesh = TextRenderer::Instance().GenerateTextMesh(text, _fontPath, _alignment);
	valid = false;
}

const std::string UIText::GetText() const
{
	return  _text;
}

void UIText::SetText(std::string text) 
{
	// If text is different, invalidate this UIComponent to be Resized
    if (text != _text) 
	{ 
        _text = text;
		_textMesh = TextRenderer::Instance().GenerateTextMesh(text, _fontPath, _alignment);
        valid = false;
    }
}

TextMesh * UIText::GetTextMesh() const
{
	return _textMesh;
}

void UIText::SetTextMesh(TextMesh * textMesh)
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

const std::string UIText::GetFont() const
{
	return _fontPath;
}

void UIText::SetFont(std::string font)
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

TextAlignment UIText::GetAlignment() const
{
	return _alignment;
}

void UIText::SetAlignment(TextAlignment alignment)
{
	_alignment = alignment;
	_textMesh = TextRenderer::Instance().GenerateTextMesh(_text, _fontPath, _alignment);
	// changing alignment will not change size (no need to resize).
}

float UIText::GetFontScale() const
{
	return _fontScale;
}

void UIText::SetFontScale(float scale)
{
	_fontScale = scale;
	valid = false;
}

void UIText::SetSpacing(float spacing)
{
	_spacing = spacing;
}

float UIText::GetSpacing() const
{
	return _spacing;
}

void UIText::CalculateScreenSize(const UIComponent * parent)
{
	screenSize = (_textMesh != nullptr) ? _textMesh->Size * _fontScale : glm::vec2();
}