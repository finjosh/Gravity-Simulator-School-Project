#include "BodySpawner.h"

/// rectangle shape initializer function
BodySpawner::BodySpawner(float SpawnCooldown, int SpawnAmount, Vector2f SpawnPosition, Vector2f ShapeSize, float Density, Vector2f StartingForce)
:   SpawnCooldown(SpawnCooldown),
    SpawnAmount(SpawnAmount),
    SpawnPosition(SpawnPosition),
    StartingForce(b2Vec2(StartingForce.x, StartingForce.y)),
    ShapeSize(ShapeSize),
    Density(Density),
    isCircle(false)
{
    BodySpawner::Spawner = new RectangleShape(Vector2f(ShapeSize.x*1.5, ShapeSize.y*1.5));
    BodySpawner::Spawner->setOrigin(ShapeSize.x*0.75, ShapeSize.y*0.75);
    BodySpawner::Spawner->setPosition(SpawnPosition);
    BodySpawner::Spawner->setFillColor(Color(255,255,255,100));
}

/// circle shape initializer function
BodySpawner::BodySpawner(float SpawnCooldown, int SpawnAmount, Vector2f SpawnPosition, float radius, float Density, Vector2f StartingForce)
:   SpawnCooldown(SpawnCooldown),
    SpawnAmount(SpawnAmount),
    SpawnPosition(SpawnPosition),
    StartingForce(b2Vec2(StartingForce.x, StartingForce.y)),
    radius(radius),
    Density(Density),
    isCircle(true)
{
    BodySpawner::Spawner = new CircleShape(radius*1.5);
    BodySpawner::Spawner->setOrigin(radius*1.5, radius*1.5);
    BodySpawner::Spawner->setPosition(SpawnPosition);
    BodySpawner::Spawner->setFillColor(Color(255,255,255,100));
}

BodySpawner::~BodySpawner() {
    delete(BodySpawner::Spawner);
}

/// checking if the spawner needs to spawn any blocks/bodys
void BodySpawner::CheckSpawn(float deltaTime, b2World &world, vector<Block> &normalBodys)
{
    timer += deltaTime;
    if (timer >= BodySpawner::SpawnCooldown) {
        timer = 0;
        for (int i = 0; i < SpawnAmount; i++) {
            normalBodys.push_back(SpawnBody(world));
            normalBodys.back()->ApplyForceToCenter(BodySpawner::StartingForce, true);
            // applying a second random small force to stop the bodys from stacking as they do if there is no force applied
            normalBodys.back()->ApplyForceToCenter(b2Vec2((float)(rand()%10)/100, (float)(rand()%10)/100), true);
        }
    }
}

/// used to spawn bodys should only be used by the spawner struct
Block BodySpawner::SpawnBody(b2World &world)
{
    if (isCircle)
    {
        b2BodyDef bodyDef;
        bodyDef.position.Set(SpawnPosition.x/pixels_per_meter, SpawnPosition.y/pixels_per_meter);
        bodyDef.type = b2_dynamicBody;
        bodyDef.linearDamping = 0; // making linearDamping 0 as there should be no deleceration without gravity force
        b2CircleShape b2shape;

        b2shape.m_radius = radius/pixels_per_meter;

        b2FixtureDef fixtureDef;
        fixtureDef.density = Density;
        fixtureDef.friction = 0.4;
        fixtureDef.restitution = 0.5;
        fixtureDef.shape = &b2shape;

        b2Body* res = world.CreateBody(&bodyDef);
        res->CreateFixture(&fixtureDef);

        sf::CircleShape* shape1 = new sf::CircleShape(radius);
        shape1->setOrigin(radius,radius);
        shape1->setPosition(SpawnPosition);
        shape1->setFillColor(Color::White);

        res->GetUserData().pointer = (uintptr_t)shape1;

        return res;
    }
    else
    {
        b2BodyDef bodyDef;
        bodyDef.position.Set(SpawnPosition.x/pixels_per_meter, SpawnPosition.y/pixels_per_meter);
        bodyDef.type = b2_dynamicBody;
        bodyDef.linearDamping = 0; // making linearDamping 0 as there should be no deleceration without gravity force
        b2PolygonShape b2shape;
        b2shape.SetAsBox(ShapeSize.x/pixels_per_meter/2.0, ShapeSize.y/pixels_per_meter/2.0);
        b2FixtureDef fixtureDef;
        fixtureDef.density = Density;
        fixtureDef.friction = 0.4;
        fixtureDef.restitution = 0.5;
        fixtureDef.shape = &b2shape;

        b2Body* res = world.CreateBody(&bodyDef);
        res->CreateFixture(&fixtureDef);

        sf::RectangleShape* shape = new sf::RectangleShape(ShapeSize);
        shape->setOrigin(ShapeSize.x/2, ShapeSize.y/2);
        shape->setPosition(SpawnPosition);
        shape->setFillColor(Color::White);

        res->GetUserData().pointer = (uintptr_t)shape;

        return res;
    }
}

/// used to draw the visual part of the spawner
void BodySpawner::DrawSpawner(RenderWindow &window)
{
    window.draw(*Spawner);
}
