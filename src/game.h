#ifndef _GAME_H_
#define _GAME_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H 

#include "player_object.h"
#include "game_object.h"
#include <vector>
#include <map>
#include <string>

struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN,
    GAME_LOSE
};


class Game {
    public:
        GameState State;
        bool Keys[1024];
        unsigned int Width, Height;
        std::vector<GameObject *> FlashingLights;
        std::vector<GameObject *> Zappers;
        std::vector<GameObject *> Coins;
        unsigned int Level;
        bool Stuck;
        unsigned int LastTime;
        unsigned int LastTimeCoins;
        unsigned int StartTime;
        unsigned int CoinsCount;
        unsigned int Distance;
        unsigned int CoinsBeforeLevel;
        Game(unsigned int width, unsigned int height);
        ~Game();

        void Init();
        int TextInit();
        void ProcessInput(float dt);
        void Update(float dt);
        void Render();
        void DoCollisions();
        void Reset();
        void GoToNextLevel();
        void RenderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color);
};

#endif