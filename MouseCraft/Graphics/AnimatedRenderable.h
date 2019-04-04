#pragma once

#include "../Core/Component.h"
#include "Model.h"
#include "BonedGeometry.h"
#include "BoneData.h"
#include "../AnimationData.h"
#include <map>
#include <string>

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
			counter += dt;
			if (counter > _currentAnim->duration)
				counter -= _currentAnim->duration;

			for (auto& banim : _currentAnim->frames)
			{
				float current = 0;
				glm::vec3 pos = banim.positions[0].second;
				glm::quat rot = banim.rotations[0].second;
				glm::vec3 scl = banim.scales[0].second;

				for (auto& p : banim.positions) {
					if (counter > p.first)
					{
						// start -> counter -> end 
						// counter - start / end - start;
						float percent = p.first - current; 
						break;
					}
					else
					{
						current = p.first;
						pos = p.second;
					}
				}
				for (auto& r : banim.rotations) {
					if (counter > r.first)
						break;
					else
						rot = r.second;
				}
				for (auto& s : banim.positions) {
					if (counter > s.first)
						break;
					else
						scl = s.second;
				}

				_boneMap[banim.name]->transform->transform.setLocalPosition(pos);
				_boneMap[banim.name]->transform->transform.setLocalScale(scl);
			}
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
		if (anims.size() > 0)
		{
			_currentAnim = &anims[0];
		}
	}

private:
	Model* _model;
	BonedGeometry* _geometry;
	BoneMap _boneMap;
	std::vector<BoneData> _bones;
	std::vector<Animation> _animations;
	Animation* _currentAnim;
	float counter = 0;
};

