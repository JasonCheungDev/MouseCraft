#pragma once

#include "../OmegaEngineDefines.h"

#include <map>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>

#include <json.hpp>
using json = nlohmann::json;

struct Vec3Keyframe 
{
	float time;
	glm::vec3 value;
	
	bool operator<(const Vec3Keyframe& other) const 
	{
		return time < other.time;
	}
};

struct QuatKeyframe
{
	float time;
	glm::quat value;

	bool operator<(const QuatKeyframe& other) const
	{
		return time < other.time;
	}
};

class LinearConverter
{
public:
	virtual float Convert(float t) { return t; };
protected:
	static float Expand(float v) { return v * 2 - 1; }
	static float Linearize(float v) { return v * 0.5f + 0.5f; }
};

class SineConverter : public LinearConverter
{
public:
	float Convert(float t) override 
	{
		auto input = LinearConverter::Expand(t) * glm::pi<float>() / 2.0f;	// convert 0...1 to -pi/2...pi/2
		return LinearConverter::Linearize( glm::sin(input) );				// return 0...1
	};
};

class X2Converter : public LinearConverter
{
public:
	float Convert(float t) override
	{
		return t * t;
	}
};

class X3Converter : public LinearConverter
{
public:
	float Convert(float t) override
	{
		return t * t * t;
	}
};

enum RotationPolicy { QUATERNION, EULER };

class Animation
{
public:
	std::string name;

	float duration;

	void AddPosition(float time, glm::vec3 position);

	void AddRotation(float time, glm::quat rotation);
	
	void AddRotation(float time, glm::vec3 rotation);

	void AddScale(float time, glm::vec3 scale);

	void SetPositionKeyFrames(std::vector<Vec3Keyframe> keyframes);

	void SetRotationKeyFrames(std::vector<QuatKeyframe> keyframes);

	void SetScaleKeyFrames(std::vector<Vec3Keyframe> keyframes);

	glm::vec3 GetPosition(float time) const;

	glm::quat GetRotation(float time) const;

	glm::vec3 GetScale(float time) const;

	int GetPositionsCount() const;

	int GetRotationCount() const;

	int GetScalesCount() const;

	void SetCurve(LinearConverter* converter);

	RotationPolicy GetRotationPolicy();

	// If the rotations don't look as you intended try using the other policy.
	// WARNING: Animating between euler angles is not as robust.
	void SetRotationPolicy(RotationPolicy policy);

	static Animation* CreateFromJson(json json);

private:
	// helper function to get a rotation from quaternion keyframes 
	glm::quat GetRotationQuat(float time) const;
	// helper function to get a rotation from vec3 keyframes 
	glm::quat GetRotationVec3(float time) const;
	
	std::vector<Vec3Keyframe> _keyframesPos;
	std::vector<QuatKeyframe> _keyframesQuat;
	std::vector<Vec3Keyframe> _keyframesRot;
	std::vector<Vec3Keyframe> _keyframesScl;
	RotationPolicy _rotationPolicy;
	LinearConverter* _converter = defaultConverter;
	static LinearConverter* defaultConverter;
};