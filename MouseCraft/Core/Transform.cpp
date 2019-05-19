#include "Transform.h"

#include "../OmegaEngineDefines.h"

#include <iostream>
#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>

glm::vec3 Transform::getLocalPosition() const
{
	return _localPosition;
}

void Transform::setLocalPosition(glm::vec3 position)
{
	_localPosition = position;
	_invalidated = true;
}

glm::vec3 Transform::getLocalRotation() const
{
	return _localRotation;
}

void Transform::setLocalRotation(glm::vec3 rotation)
{
	_localRotation = rotation;
	_localQuat = glm::quat(rotation);
	_invalidated = true;
}

glm::quat Transform::getLocalQuaternion() const
{
	return _localQuat;
}

void Transform::setLocalRotation(glm::quat rotation)
{
	_localRotation = glm::eulerAngles(rotation);
	_localQuat = rotation;
	_invalidated = true;
}

glm::vec3 Transform::getLocalScale() const
{
	return _localScale;
}

void Transform::setLocalScale(glm::vec3 scale)
{
	_localScale = scale;
	_invalidated = true;
}

void Transform::setLocalTransformation(glm::mat4 transform)
{
	// warning: assuming matrix bottom-right value is 1. If not divide everything in matrix by that value. 
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(transform, _localScale, _localQuat, _localPosition, skew, perspective);
	_localRotation = glm::eulerAngles(_localQuat);
	_localTransformation = transform;
}

glm::vec3 Transform::getWorldPosition() const
{
	return glm::vec3(
		_worldTransformation[3][0],
		_worldTransformation[3][1],
		_worldTransformation[3][2]
	);
}

void Transform::translate(glm::vec3 amount)
{
	if (amount == glm::vec3()) return;
	setLocalPosition(getLocalPosition() + amount);
}

void Transform::scale(float amount)
{
	if (amount == 1) return;
	setLocalScale(getLocalScale() * amount);
}

void Transform::rotate(glm::vec3 amount)
{
	if (amount == glm::vec3()) return;
	setLocalRotation(getLocalRotation() + amount);
}

// WARNING: Entity::getWorldRotation() should be more performant (at the cost of memory locality?)
glm::vec3 Transform::getWorldRotation() const
{
	float y, x, z;
	glm::extractEulerAngleYXZ(_worldTransformation, y, x, z);
	return glm::vec3(x,y,z);
}

// WARNING: UNTESTED.
glm::vec3 Transform::getWorldScale() const
{
	return glm::vec3(
		glm::length(glm::vec3(_worldTransformation[0])),
		glm::length(glm::vec3(_worldTransformation[1])),
		glm::length(glm::vec3(_worldTransformation[2]))
	);
}

glm::vec3 Transform::getLocalForward() const
{
	return -glm::normalize(glm::vec3(
		_localTransformation[2][0],
		_localTransformation[2][1],
		_localTransformation[2][2]
	));
}

glm::vec3 Transform::getLocalRight() const
{
	return glm::normalize(glm::vec3(
		_localTransformation[0][0],
		_localTransformation[0][1],
		_localTransformation[0][2]
	));
}

glm::vec3 Transform::getLocalUp() const
{
	return glm::normalize(glm::vec3(
		_localTransformation[1][0],
		_localTransformation[1][1],
		_localTransformation[1][2]
	));
}

glm::vec3 Transform::getWorldForward() const
{
	return -glm::normalize(glm::vec3(
		_worldTransformation[2][0],
		_worldTransformation[2][1],
		_worldTransformation[2][2]
	));
}

glm::vec3 Transform::getWorldRight() const
{
	return glm::normalize(glm::vec3(
		_worldTransformation[0][0],
		_worldTransformation[0][1],
		_worldTransformation[0][2]
	));
}

glm::vec3 Transform::getWorldUp() const
{
	return glm::normalize(glm::vec3(
		_worldTransformation[1][0],
		_worldTransformation[1][1],
		_worldTransformation[1][2]
	));
}

glm::mat4 Transform::getLocalTransformation() const
{
	return _localTransformation;
}

glm::mat4 Transform::getWorldTransformation() const
{
	return _worldTransformation;
}

bool Transform::getLocalInvalidated() const
{
	return _invalidated;
}

void Transform::setLocalInvalidation(bool invalidate)
{
	_invalidated = invalidate;
}

void Transform::face2D(glm::vec2 dir)
{
	setLocalRotation(glm::vec3(0, getAngle2D(dir), 0));
}

void Transform::face2D(glm::vec3 dir)
{
	face2D(glm::vec2(dir.x, dir.z));
}

void Transform::computeLocalTransformation()
{
	_localTransformation = glm::mat4(1.0f);
	_localTransformation = glm::translate(_localTransformation, _localPosition);

	/*
	// we going super-sonic http://www.opengl-tutorial.org/assets/faq_quaternions/index.html#Q26
	// this is the precalculated y*x*z rotation matrix. (x2 faster)
	auto A = glm::cos(_localRotation.x);
	auto B = glm::sin(_localRotation.x);
	auto C = glm::cos(_localRotation.y);
	auto D = glm::sin(_localRotation.y);
	auto E = glm::cos(_localRotation.z);
	auto F = glm::sin(_localRotation.z);
	auto BC = B * C;
	auto BD = B * D;

	glm::mat4 rot = glm::mat4(0.0f);
	rot[0][0] = C * E + BD * F;
	rot[0][1] = A * F;
	rot[0][2] = -D * E + BC * F;
	rot[1][0] = -C * F + BD * E;
	rot[1][1] = A * E;
	rot[1][2] = D * F + BC * E;
	rot[2][0] = A * D;
	rot[2][1] = -B;
	rot[2][2] = A * C;
	rot[3][3] = 1.0f;
	_localTransformation = _localTransformation * rot;
	*/

	_localTransformation = _localTransformation * (glm::mat4)_localQuat;

	_localTransformation = glm::scale(_localTransformation, _localScale);

	_invalidated = false;
}

void Transform::computeWorldTransformation(glm::mat4 parent)
{
	_worldTransformation = parent * _localTransformation;
}

float Transform::getAngle2D(glm::vec2 dir)
{
	auto rotation = glm::atan(dir.x / dir.y);
	if (dir.y >= 0)
		rotation += glm::pi<float>();
	return rotation;
}

float Transform::getAngle2D(glm::vec3 dir)
{
	return getAngle2D(glm::vec2(dir.x, dir.z));
}

/*
#include <glm/gtx/matrix_decompose.hpp>
void Transform::setLocalTransform(glm::mat4 matrix)
{
	// warning: assuming matrix bottom-right value is 1. If not divide everything in matrix by that value. 
	glm::quat rot;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(matrix, scale, rot, _localPosition, skew, perspective);
	rotation = glm::eulerAngles(rot);
}
*/

#pragma region Vector2D Interfaces

glm::vec2 Transform::getLocalPosition2D() const
{
	auto pos = getLocalPosition();
	return glm::vec2(pos.x, pos.z);
}

float Transform::getLocalRotation2D() const
{
	return getAngle2D(getLocalForward());
}

glm::vec2 Transform::getLocalScale2D() const
{
	auto scl = getLocalScale();
	return glm::vec2(scl.x, scl.z);
}

glm::vec2 Transform::getLocalForward2D() const
{
	auto fwd = getLocalForward();
	return glm::vec2(fwd.x, fwd.z);
}

glm::vec2 Transform::getLocalRight2D() const
{
	auto rgt = getLocalRight();
	return glm::vec2(rgt.x, rgt.z);
}

glm::vec2 Transform::getWorldPosition2D() const
{
	auto pos = getWorldPosition();
	return glm::vec2(pos.x, pos.z);
}

float Transform::getWorldRotation2D() const
{
	return getAngle2D(getWorldForward());
}

glm::vec2 Transform::getWorldScale2D() const
{
	auto scl = getWorldScale();
	return glm::vec2(scl.x, scl.z);
}

glm::vec2 Transform::getWorldForward2D() const
{
	auto fwd = getWorldForward();
	return glm::vec2(fwd.x, fwd.z);
}

glm::vec2 Transform::getWorldRight2D() const
{
	auto rgt = getWorldRight();
	return glm::vec2(rgt.x, rgt.z);
}

void Transform::setLocalPosition2D(glm::vec2 pos)
{
	auto cur = getLocalPosition();
	cur.x = pos.x;
	cur.z = pos.y;
	setLocalPosition(cur);
}

void Transform::setLocalRotation2D(float angle)
{
	auto cur = getLocalRotation();
	cur.y = angle;
	setLocalRotation(cur);
}

void Transform::setLocalScale2D(glm::vec2 scl)
{
	auto cur = getLocalScale();
	cur.x = scl.x;
	cur.z = scl.y;
	setLocalScale(cur);
}

#pragma endregion
