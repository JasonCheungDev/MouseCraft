#pragma once

#include <string>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "../Core/Component.h"
#include "../Core/Entity.h"
#include "../Graphics/Color.h"
#include <algorithm>

enum VerticalAnchor {
    ANCHOR_TOP, ANCHOR_VCENTER, ANCHOR_BOTTOM
};

enum HorizontalAnchor {
    ANCHOR_LEFT, ANCHOR_HCENTER, ANCHOR_RIGHT
};

// The unit type for the anchor
enum AnchorType {
    ANCHOR_PERCENT, ANCHOR_PIXEL
};

// The unit type for the size of the UIComponent
enum UnitType {
    UNIT_PIXEL, UNIT_PERCENT, UNIT_SCALE
};

struct Rect
{
	float left;
	float top;
	float right;
	float bottom;

	float getWidth() { return abs(right - left); }
	float getHeight() { return abs(top - bottom); }
	float getXCenter() { return (left + right) / 2;}
	float getYCenter() { return (bottom + top) / 2; }
	glm::vec2 getCenter() { return glm::vec2(getXCenter(), getYCenter()); }
};

/**
Basic UI panel that makes up all of the UI system.
UI systems are made by creating multiple of these nested within each other
*/
class UIComponent : public Component {
public:
    UIComponent(float width, float height, float x, float y);
    ~UIComponent();

	// Recalculates screen position and size
    virtual void Resize();

	// Determine whether this panel uses transparency
    virtual bool IsTransparent();

	// String id used for uniquely identifying this UIComponent
	std::string			id;

	// Whether or not this UIComponent and its children should be drawn
    bool                visible;

	// UIComponent should be resized if Valid is set to false
	bool				valid;

	Color				color;

	float				zForce;
	float				z;
    
	glm::vec2			anchorOffset;
	VerticalAnchor      vAnchor;
    HorizontalAnchor    hAnchor;
    AnchorType          anchorXType;
    AnchorType          anchorYType;

	glm::vec2           size;
    UnitType            xType;
    UnitType            yType;

	// If using scaling type sizing on one side, defines the ratio by which to scale on
    float               aspectRatio;

	// Calculated screen coordinates, size and rotation in pixels
	Rect				screenBounds;
    glm::vec2           screenPosition;
    glm::vec2           screenSize;
    float				screenRotation;

	// Name of UIComponent's associated action to take when clicked
	// Blank string if no action
    std::string         ClickAction;

	//std::vector<Model*> models;

	virtual glm::mat4 GetTransform();

protected:
	virtual void setupModels();

	static float screenWidth();

	static float screenHeight();
};