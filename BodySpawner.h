#ifndef BODYSPAWNER_H
#define BODYSPAWNER_H

#include <iostream>
#include <vector>
#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
#include "physics.h"

using namespace std;
using namespace sf;

struct BodySpawner {

    public:

        // the spawner has to give an starting force or else the bodys will stack and not move

        /// rectangle shape initializer function
        /// MAKE SURE THAT THERE IS EITHER NO STARTING FORCE GIVEN OR A FORCE DO NOT GIVE NO FORCE
        BodySpawner(float SpawnCooldown, int SpawnAmount, Vector2f SpawnPosition, Vector2f ShapeSize, float Density = 1, Vector2f StartingForce = Vector2f(0.01,0.01));

        /// circle shape initializer function
        /// MAKE SURE THAT THERE IS EITHER NO STARTING FORCE GIVEN OR A FORCE DO NOT GIVE NO FORCE
        BodySpawner(float SpawnCooldown, int SpawnAmount, Vector2f SpawnPosition, float radius, float Density = 1, Vector2f StartingForce = Vector2f(0.01,0.01));

        ~BodySpawner();

        /// checking if the spawner needs to spawn any blocks/bodys
        void CheckSpawn(float deltaTime, b2World &world, vector<Block> &normalBodys);

        /// used to spawn bodys should only be used by the spawner struct
        Block SpawnBody(b2World &world);

        /// used to draw the visual part of the spawner
        void DrawSpawner(RenderWindow &window);


        // visual vars
        Shape* Spawner;

        // logical vars
        float SpawnCooldown = 0;
        float timer = 0;
        int SpawnAmount = 0;

        Vector2f SpawnPosition;
        b2Vec2 StartingForce;

        // body vars
        bool isCircle;
        Vector2f ShapeSize;
        float radius = 5;
        float Density = 1;

};

#endif // BODYSPAWNER_H
