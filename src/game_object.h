#ifndef _GAME_OBJECT_H_
#define _GAME_OBJECT_H_

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "sprite_renderer.h"

class GameObject
{
    public:
        glm::vec2 Position, Size, Velocity;
        glm::vec3 Color;
        float Rotation;
        bool IsSolid;
        bool Destroyed;
        float IsRotating;
        Texture2D Sprite;
        //constructor
        GameObject();
        GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 veloctiy = glm::vec2(0.0f, 0.0f), float rotation = 0.0f, float isRotating = 0.0f);
        //draw Sprite
        glm::vec2 Move(float dt);
        virtual void Draw(SpriteRenderer &renderer);
};

#endif