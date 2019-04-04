#pragma once

#include "../Core/Component.h"
#include "Model.h"
#include "BonedGeometry.h"
#include "BoneData.h"
#include "../AnimationData.h"
#include <map>
#include <string>
#include <iostream>

// A special renderable for animated objects.
// Note: Does not inherit from Renderable as it needs an update call.
class AnimatedRenderable : public Component
{
	typedef std::map<std::string, BoneData*> BoneMap;
public:
	AnimatedRenderable();
	~AnimatedRenderable();
	void OnInitialized() override {};
	
	void Update(float dt) 
	{
		if (_currentAnim != nullptr)
		{
			counter += dt * 25.0f;
			if (counter > _currentAnim->duration)
				counter -= _currentAnim->duration;

			bool first = true;

			// go thru each bone animation 
			for (auto& banim : _currentAnim->frames)
			{
				glm::vec3 pos = banim.positions[0].second;
				glm::quat rot = banim.rotations[0].second;
				glm::vec3 scl = banim.scales[0].second;

				// calculate interpolated transform
				float current = 0;
				for (auto& p : banim.positions) {
					if (p.first > counter)
					{
						float percent = (counter - current) / (p.first - current);
						pos = glm::lerp(pos, p.second, percent);
						break;
					}
					else
					{
						current = p.first;
						pos = p.second;
					}
				}

				current = 0;
				for (auto& v : banim.rotations) {
					if (v.first > counter)
					{
						float percent = (counter - current) / (v.first - current);
						rot = glm::lerp(rot, v.second, percent);	// crashing here 
						break;
					}
					else
					{
						current = v.first;
						rot = v.second;
					}
				}
				
				current = 0;
				for (auto& v : banim.scales) {
					if (v.first > counter)
					{
						float percent = (counter - current) / (v.first - current);
						scl = glm::lerp(scl, v.second, percent);
						break;
					}
					else
					{
						current = v.first;
						scl = v.second;
					}
				}

				if (first)
				{
					// std::cout << pos.x << "," << pos.y << "," << pos.z << std::endl;
					first = false;
				}

				// update transform 
				auto boneEntity = _boneMap[banim.name];
				_boneMap[banim.name]->transform->transform.setLocalPosition(pos);
				_boneMap[banim.name]->transform->transform.setLocalRotation(rot);
				_boneMap[banim.name]->transform->transform.setLocalScale(scl);
			}

			auto debug = GetBones()[0].transform->transform.getWorldPosition();
		}
	};

	Model* GetModel() {
		return _model;
	}

	BonedGeometry* GetGeometry() {
		return _geometry;
	}

	void SetGeometry(BonedGeometry* g)
	{
		_geometry = g;
	}

	BoneMap GetBoneMap() {
		return _boneMap;
	}

	std::vector<BoneData>& GetBones()
	{
		return _bones;
	}

	void SetBones(std::vector<BoneData>& bones)
	{
		_bones = bones;
		_boneMap.clear();
		for (auto& b : _bones)
			_boneMap[b.name] = &b;
	}

	std::vector<Animation>& GetAnimations()
	{
		return _animations;
	}

	void SetAnimations(std::vector<Animation>& anims)
	{
		_animations = anims;
		if (_animations.size() > 0)
		{
			_currentAnim = &_animations[0];
		}
	}

private:
	Model* _model;
	BonedGeometry* _geometry;
	BoneMap _boneMap;
	std::vector<BoneData> _bones;
	std::vector<Animation> _animations;
	Animation* _currentAnim = nullptr;
	float counter = 0;
};

