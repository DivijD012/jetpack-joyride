#include "game_object.h"

GameObject::GameObject()
    : Position(0.0f, 0.0f), Size(1.0f, 1.0f), Velocity(0.0f), Color(1.0f), Rotation(0.0f), Sprite(), IsSolid(false), Destroyed(false), IsRotating(0.0f) { }

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity, float rotation, float isRotating)
    : Position(pos), Size(size), Velocity(velocity), Color(color), Rotation(rotation), Sprite(sprite), IsSolid(false), Destroyed(false), IsRotating(isRotating) { }

glm::vec2 GameObject::Move(float dt)
{
    // if not stuck to player board
    this->Position -= this->Velocity * dt;
    return this->Position;
}

void GameObject::Draw(SpriteRenderer &renderer)
{
    if(this->IsRotating >= 0)
        this->Rotation += IsRotating;
    if(this->Destroyed == false)
        renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Color);
}