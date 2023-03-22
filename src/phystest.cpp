#include <box2d/box2d.h>
#include <iostream>

b2Body* createCircle(b2Vec2 pos, b2Vec2 vel, b2World& world, float radius) {
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position = pos;
	bodyDef.linearVelocity = vel;
	b2Body* body = world.CreateBody(&bodyDef);

	b2CircleShape circle;
	circle.m_radius = radius;
	body->CreateFixture(&circle, 1.0f/* density */);

	return body;
}

int main() {
    // box2d example code
	b2World world(b2Vec2(0.f, 0.f));

	b2Body* body = createCircle(b2Vec2(0.f, 0.f), b2Vec2(10.f, 0.f), world, 1.f);
	b2Body* body2 = createCircle(b2Vec2(10.f, 0.f), b2Vec2(0.f, 0.f), world, 1.f);

	float timeStep = 4.f / 60.f;
	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	for (int i = 0; i < 20; ++i) {
		world.Step(timeStep, velocityIterations, positionIterations);
		b2Vec2 pos = body->GetPosition();
		b2Vec2 pos2 = body2->GetPosition();
		std::cout << "pos1: " << pos.x  << ", " << pos.y  << std::endl;
		std::cout << "pos2: " << pos2.x << ", " << pos2.y << std::endl;
	}

	return 0;
}
