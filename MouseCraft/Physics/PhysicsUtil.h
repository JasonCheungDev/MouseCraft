#pragma once

#include <Box2D/Box2D.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

glm::vec2 b2gl2(b2Vec2 vec);

glm::vec3 b2gl3(b2Vec2 vec);

b2Vec2 gl2b2(glm::vec2 vec);

b2Vec2 gl3b2(glm::vec3 vec);