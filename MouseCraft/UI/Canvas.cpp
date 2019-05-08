#include "Canvas.h"



Canvas::Canvas() : UIComponent(0,0,0,0)
{
	screenBounds = {
		0.0f,
		screenHeight(),
		screenWidth(),
		0.0f
	};

}

Canvas::~Canvas()
{
}

void Canvas::OnInitialized()
{
	GetEntity()->transform.setLocalPosition(glm::vec3(screenBounds.getCenter(), 0.0f));
	//GetEntity()->transform.computeLocalTransformation();
	//GetEntity()->transform.computeWorldTransformation();
}
