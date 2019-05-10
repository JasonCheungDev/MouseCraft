#pragma once

class System
{
public:
	System();
	~System();
	
	virtual void Update(float dt) {};
	virtual void FixedUpdate(float dt, int steps) {};
};
