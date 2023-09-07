#ifndef GRAVITYBODY_H
#define GRAVITYBODY_H

#include "physics.h"
#include <iostream>

using namespace std;
using namespace sf;

struct GravityBody {

    public:

        GravityBody(Block body, b2Vec2 position, float radius, float gravitationalForce, bool DeleteOnCollsion, float rotation = 0.f, Color color = Color(150,0,0,150));
        void ApplyGravityForce(vector<Block> &blocksAffected, b2World &world);
        void DestoryGravityBody(b2World &world);

        Block body;
        float radius;

        b2Vec2 position; // using a position var in the struct to make it easier to find position later as it cant move
        float gravitationalForce;
        bool DeleteOnCollsion;

};

#endif // GRAVITYBODY_H
