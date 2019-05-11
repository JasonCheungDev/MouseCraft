#include "PhysicsManager.h"

#include "../Core/ComponentManager.h"
#include "../Core/ComponentFactory.h"
#include "PhysicsUtil.h"

PhysicsManager::PhysicsManager()
{
	b2Vec2 gravity(0, 0);

	cListener = new CContactListener();
	cListener->setup();

	world = new b2World(gravity);
	world->SetContactListener(cListener);

	profiler.InitializeTimers(4);
	profiler.LogOutput("Physics.log");	// optional
}

PhysicsManager::~PhysicsManager()
{
	delete(cListener);
	delete(world);
}

void PhysicsManager::FixedUpdate(float dt, int steps)
{
	profiler.StartTimer(0);

	// Resolve body status. This allows use to disable entities or components. 
	// Note: OmegaEngine guarantees that entity life/status will not change during system updates. 	
	auto physicComponents = ComponentManager<PhysicsComponent>::Instance().All();
	for (auto& pc : physicComponents)
	{
		bool active = pc->GetActive();

		// performance should be ok referring to the latest revision of b2body.cpp 
		// (there's fast return if this doesn't change active status)
		pc->body->SetActive(active);

		// also update velocities while we're here
		if (active)
		{
			// pc->body->SetLinearVelocity(b2Vec2(pc->velocity.x, pc->velocity.y));
		}
	}

	b2Body* b = world->GetBodyList(); //points to the first body

	// simulate physics world 
	world->Step(dt, 10, 10);

	// check for collisions
	checkCollisions();

	// update components to match with bodies 
	for (auto& pc : physicComponents)
	{
		if (pc->GetActive())
		{
			// retain existing y-height
			auto pos = glm::vec3(pc->body->GetPosition().x, pc->GetEntity()->t().pos().y, pc->body->GetPosition().y);
			pc->GetEntity()->transform.setLocalPosition(pos);
			auto rot = glm::vec3(pc->GetEntity()->t().rot().x, pc->body->GetAngle(), pc->GetEntity()->t().rot().z);
			pc->GetEntity()->transform.setLocalRotation(rot);
			pc->velocity = b2gl2(b->GetLinearVelocity());
		}
	}

	profiler.StopTimer(0);
	profiler.FrameFinish();
}

void PhysicsManager::checkCollisions()
{
	//If there are any unhandled collisions
	if (cListener->hasCollided() == 0)
		return;

	b2Body** collider1 = cListener->getColliders1();
	b2Body** collider2 = cListener->getColliders2();

	for (int c = 0; c < cListener->hasCollided(); c++)
	{
		b2Body* c1 = collider1[c];
		b2Body* c2 = collider2[c];

		PhysicsComponent* pComp1 = static_cast<PhysicsComponent*>(c1->GetUserData());
		PhysicsComponent* pComp2 = static_cast<PhysicsComponent*>(c2->GetUserData());
			
		pComp1->onCollide.Notify(pComp2);
		pComp2->onCollide.Notify(pComp1);
	}

	cListener->resetCollided();
}

//returns a list of the objects hit
std::vector<PhysicsComponent*> PhysicsManager::areaCheck(glm::vec2 bl, glm::vec2 tr, std::set<PhysObjectType::PhysObjectType>* toCheck)
{
	std::vector<PhysicsComponent*> foundObjects;

	if (toCheck && toCheck->size() == 0)
	{
		std::cerr << "WARNING: Physics - Area checking with explicit empty layers. What are you doing?" << std::endl;
		return foundObjects;
	}

	AreaQueryCallback callback;

	b2AABB boundingBox;
	boundingBox.lowerBound = gl2b2(bl);
	boundingBox.upperBound = gl2b2(tr);

	world->QueryAABB(&callback, boundingBox);

	for (int i = 0; i < callback.foundBodies.size(); i++)
	{
		PhysicsComponent* pComp = static_cast<PhysicsComponent*>(callback.foundBodies[i]->GetUserData());

		if (toCheck == nullptr || toCheck->find(pComp->pType) != toCheck->end())
			foundObjects.push_back(pComp);
	}

	return foundObjects;
}

PhysicsComponent * PhysicsManager::rayCheck(glm::vec2 start, glm::vec2 dest, glm::vec2& hit, std::set<PhysObjectType::PhysObjectType>* toCheck)
{
	if (toCheck && toCheck->size() == 0)
	{
		std::cerr << "WARNING: Physics - Ray checking with explicit empty layers. What are you doing?" << std::endl;
		return nullptr;
	}

	PhysicsComponent* bestMatch = nullptr;
	RayQueryCallback callback;

	b2Vec2 point1 = gl2b2(start);
	b2Vec2 point2 = gl2b2(dest);

	world->RayCast(&callback, point1, point2);

	//Find the closest object hit by the ray
	float32 frac = 1; //used to determine the closest object
	for (int i = 0; i < callback.hitBodies.size(); i++)
	{
		//if the object is further than the best match so far, move on
		if (callback.fractions[i] > frac)
			continue;

		PhysicsComponent* pComp = static_cast<PhysicsComponent*>(callback.hitBodies[i]->GetUserData());

		if (toCheck == nullptr || toCheck->find(pComp->pType) != toCheck->end())
		{
			frac = callback.fractions[i];
			bestMatch = pComp;
		}
	}

	if (bestMatch == nullptr)
		return nullptr;

	// return hit location
	b2Vec2 ray = point2 - point1;
	hit = b2gl2(point1 + (frac * ray));

	return bestMatch;
}

void PhysicsManager::InitializeBody(PhysicsComponent * physicsComponent, float x, float y)
{
	if (physicsComponent->body != nullptr)
	{
		std::cerr << "ERROR: Physics - Trying to rebind a physics body to a component with one already!" << std::endl;
		return;
	}

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(x, y);
	bodyDef.active = false;	// wait for component to be active (valid state)
	// bodyDef.angle = r;

	b2PolygonShape shape;
	shape.SetAsBox(physicsComponent->width / 2, physicsComponent->height / 2);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = 1;
	fixtureDef.userData = physicsComponent;

	b2Body* body;
	body = world->CreateBody(&bodyDef);
	body->CreateFixture(&fixtureDef);
	body->SetUserData(physicsComponent);

	physicsComponent->body = body;
}
