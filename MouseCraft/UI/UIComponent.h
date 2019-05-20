#pragma once

#include <algorithm>
#include <string>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <json.hpp>
#include "../Core/Component.h"
#include "../Core/Entity.h"
#include "../Core/Rect.h"
#include "../Rendering/Color.h"
#include "../Loading/PrefabLoader.h"
using json = nlohmann::json;

class UIRoot;

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

/**
Basic UI panel that makes up all of the UI system.
UI systems are made by creating multiple of these nested within each other
*/
class UIComponent : public Component {
public:
    UIComponent(float width, float height, float x, float y);
    ~UIComponent();

	// String id used for uniquely identifying this UIComponent
	std::string			id;

	// Whether or not this UIComponent. Does not affect children.
    bool                visible;

	// UIComponent should be resized if Valid is set to false
	bool				valid;

	Color				color;

	glm::vec2           size;
	UnitType            widthType;
	UnitType            heightType;
	float				zForce;
	float				z;
    
	glm::vec2			anchorOffset;
	VerticalAnchor      vAnchor;
    HorizontalAnchor    hAnchor;
    AnchorType          anchorXType;
    AnchorType          anchorYType;

	// If using scaling type sizing on one side, defines the ratio by which to scale on
    float               aspectRatio;

	// Calculated screen coordinates, size and rotation in pixels
	Rect				screenBounds;
	glm::vec2			screenSize;
    float				screenRotation;

	// Name of UIComponent's associated action to take when clicked
	// Blank string if no action
    std::string         ClickAction;

// functions
public:
	// Recalculates screen position and size
	virtual void Resize();

	// Determine whether this panel uses transparency
	virtual bool IsTransparent();

	// Transformation matrix to get pivot into correct screen position (includes rotation and scale).
	virtual glm::mat4 GetTransform();

	// Transformation matrix specific to this component (does not affect children).
	virtual glm::mat4 GetIndividualTransform();

	// Sets this UIComponent's owner canvas 
	virtual void SetRoot(UIRoot* root);

	// Gets this UIComponent's owner canvas
	virtual UIRoot* GetRoot();

	virtual void OnInitialized() override;

	void InitalizeFromJson(json json);

protected:
	// Recalculates screen size
	virtual void CalculateScreenSize(const UIComponent* parent);

	static float ScreenWidth();

	static float ScreenHeight();

private:
	// Root canvas this UIComponent belongs to. 
	UIRoot* root;
	glm::mat4 _trans;
	glm::mat4 _indivTrans;

private:
	static Component* Create(json json);
	static ComponentRegistrar reg;
};