// #include "ball_object.h"


// PlayerObject::PlayerObject() 
//     : GameObject(), Stuck(true) { }

// PlayerObject::PlayerObject(glm::vec2 pos, glm::vec2 velocity, Texture2D sprite)
//     : GameObject(pos, GameObject::Size(size), sprite, glm::vec3(1.0f), velocity), Radius(radius), Stuck(true) { }

// glm::vec2 BallObject::Move(float dt, unsigned int window_width)
// {
//     // if not stuck to player board
//     if (!this->Stuck)
//     {
//         // move the ball
//         this->Position += this->Velocity * dt;
//         // then check if outside window bounds and if so, reverse velocity and restore at correct position
//         if (this->Position.x <= 0.0f)
//         {
//             this->Velocity.x = -this->Velocity.x;
//             this->Position.x = 0.0f;
//         }
//         else if (this->Position.x + this->Size.x >= window_width)
//         {
//             this->Velocity.x = -this->Velocity.x;
//             this->Position.x = window_width - this->Size.x;
//         }
//         if (this->Position.y <= 0.0f)
//         {
//             this->Velocity.y = -this->Velocity.y;
//             this->Position.y = 0.0f;
//         }
//     }
//     return this->Position;
// }

#include "player_object.h"

PlayerObject::PlayerObject()
    : Position(0.0f, 0.0f), Size(1.0f, 1.0f), Velocity(0.0f), Color(1.0f), Rotation(0.0f), Sprite(), IsStuck(true) { }

PlayerObject::PlayerObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity)
    : Position(pos), Size(size), Velocity(velocity), Color(color), Rotation(0.0f), Sprite(sprite), IsStuck(true) { }

void PlayerObject::Draw(SpriteRenderer &renderer)
{
    renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Color);
}

// resets the ball to initial Stuck Position (if ball is outside window bounds)
void PlayerObject::Reset(glm::vec2 position, glm::vec2 velocity)
{
    this->Position = position;
    this->Velocity = velocity;
    this->IsStuck = true;
}