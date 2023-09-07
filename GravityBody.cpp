#include "GravityBody.h"

GravityBody::GravityBody(Block body, b2Vec2 position, float radius, float gravitationalForce, bool DeleteOnCollsion, float rotation, Color color) {
    GravityBody::body = body;

    GravityBody::gravitationalForce = gravitationalForce;

    GravityBody::position = b2Vec2(position.x / pixels_per_meter, position.y / pixels_per_meter);

    GravityBody::DeleteOnCollsion = DeleteOnCollsion;

    GravityBody::radius = radius / pixels_per_meter;

    GravityBody::body->SetTransform(GravityBody::position, rotation*PI/180);

    // setting the color of the Gravity Body
    sf::Shape* shape = static_cast<sf::Shape*>((void*)body->GetUserData().pointer);
    shape->setFillColor(color);
}

void GravityBody::ApplyGravityForce(vector<Block> &blocksAffected, b2World &world) {
    for (int i = 0; i < blocksAffected.size(); i++) {
        // checking if the block being moved is touching the gravity block if it is remove it
        if (DeleteOnCollsion && physics::checkCollision(body, blocksAffected[i])) {
            physics::deleteBlock(world, blocksAffected[i]);
            blocksAffected.erase(blocksAffected.begin()+i);
            i--;
            continue;
        }

        b2Vec2 x_y_comp = b2Vec2(position.x - blocksAffected[i]->GetPosition().x,
                                     position.y - blocksAffected[i]->GetPosition().y);

        // in case that the graviy body does not remove normal blocks on collision
        if (x_y_comp.Length() <= 0) {
            physics::deleteBlock(world, blocksAffected[i]);
            blocksAffected.erase(blocksAffected.begin()+i);
            i--;
            continue;
        }

        float force = gravitationalForce / x_y_comp.LengthSquared();

        x_y_comp.Normalize();

        // applying calculated force to the body
        blocksAffected[i]->ApplyForceToCenter(b2Vec2(force*x_y_comp.x, force*x_y_comp.y), true);
    }
}

void GravityBody::DestoryGravityBody(b2World &world)  {
    physics::deleteBlock(world, body);
    delete(&body);
}
