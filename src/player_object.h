#ifndef _PLAYER_OBJECT_H_
#define _PLAYER_OBJECT_H_

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "sprite_renderer.h"

class PlayerObject
{
    public:
        glm::vec2 Position, Size, Velocity;
        glm::vec3 Color;
        float Rotation;
        bool IsStuck;
        Texture2D Sprite;
        //constructor
        PlayerObject();
        PlayerObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 veloctiy = glm::vec2(0.0f, 0.0f));
        //draw Sprite
        virtual void Draw(SpriteRenderer &renderer);
        void Reset(glm::vec2 position, glm::vec2 velocity);
};

#endif