#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include <iostream>
#include <cmath>
#include <unistd.h>

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(49.4f, 69.4f);
// Initial velocity of the player paddle
const float PLAYER_VELOCITY(500.0f);
float PLAYER_ACCELERATION(0.0f);

float DIFFICULTY_ZAPPER[] = {3, 2, 1};
float COINS_SPAWN[] = {1, 1, 1};
float TIME_TAKEN[] = {30, 40, 60};
// float TIME_TAKEN[] = {10, 15, 20};  // for testing
float SPEED[] = {300, 400, 600};

SpriteRenderer *Renderer, *SmoothRenderer, *JetpackRenderer;
PlayerObject *Player;
// GameObject * Obstacle;

unsigned int VAO, VBO;
std::map<GLchar, Character> Characters;
Shader myTextShader;

int trackInput = 0;

char *curr_wd;

bool CheckCollision(PlayerObject &one, GameObject &two) // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    // collision only if on both axes
    return collisionX && collisionY;
}

bool check1Point(glm::vec2 pointPosition, GameObject &Obstacle, int number)
{
    glm::vec2 centerPosition = glm::vec2(Obstacle.Position.x + Obstacle.Size.x/2, Obstacle.Position.y + Obstacle.Size.y/2);
    glm::vec2 centerSidePosition = centerPosition + glm::vec2(sin(glm::radians(Obstacle.Rotation))*Obstacle.Size.y/2, -cos(glm::radians(Obstacle.Rotation))*Obstacle.Size.y/2);
    glm::vec2 obstaclePosition = centerSidePosition - glm::vec2(cos(glm::radians(Obstacle.Rotation))*Obstacle.Size.x/2, sin(glm::radians(Obstacle.Rotation))*Obstacle.Size.x/2);
    float rotationObstacle = Obstacle.Rotation;
    float calculateYPositionSide = obstaclePosition.y + ((obstaclePosition.x - pointPosition.x) / tan (glm::radians(rotationObstacle)));
    bool positionSide;
    if(calculateYPositionSide <= pointPosition.y)
        positionSide = true;
    else
        positionSide = false;
    glm::vec2 obstacleOtherPosition = glm::vec2(obstaclePosition.x + 30.0f*cos(glm::radians(rotationObstacle)), obstaclePosition.y + 30.0f*sin(glm::radians(rotationObstacle)));
    float calculateYOtherSide = obstacleOtherPosition.y + ((obstacleOtherPosition.x - pointPosition.x) / tan(glm::radians(rotationObstacle)));
    bool otherSide;
    if(calculateYOtherSide < pointPosition.y)
        otherSide = true;
    else
        otherSide = false;
    // std::cout<<"============================="<<std::endl;
    // std::cout<<"Number : "<<number<<std::endl;
    // std::cout<<"Rotation : "<<rotationObstacle<<std::endl;
    // std::cout<<"Point Y : "<<pointPosition.y<<", Point X : "<<pointPosition.x<<std::endl;
    // std::cout<<"Center Y : "<<centerPosition.y<<", Center X : "<<centerPosition.x<<std::endl;
    // std::cout<<"CenterSide Y : "<<centerSidePosition.y<<", Point X : "<<centerSidePosition.x<<std::endl;
    // std::cout<<"Calculate Y : "<<calculateYPositionSide<<", Calculate X : "<<pointPosition.x<<std::endl;
    // std::cout<<"Calculate Y Other : "<<calculateYOtherSide<<", Calculate X Other : "<<pointPosition.x<<std::endl;
    if(otherSide == !positionSide)
        return true;
    else
        return false;
}


bool CheckCollisionSkew(PlayerObject &Player, GameObject &Obstacle)
{
    glm::vec2 point1 = glm::vec2(Player.Position.x, Player.Position.y);
    bool checkPoint1 = check1Point(point1, Obstacle, 1);
    glm::vec2 point2 = glm::vec2(Player.Position.x + Player.Size.x, Player.Position.y);
    bool checkPoint2 = check1Point(point2, Obstacle, 2);
    glm::vec2 point3 = glm::vec2(Player.Position.x + Player.Size.x, Player.Position.y + Player.Size.y);
    bool checkPoint3 = check1Point(point3, Obstacle, 3);
    glm::vec2 point4 = glm::vec2(Player.Position.x, Player.Position.y + Player.Size.y);
    bool checkPoint4 = check1Point(point4, Obstacle, 4);
    glm::vec2 point5 = glm::vec2(Player.Position.x, Player.Position.y + Player.Size.y/2);
    bool checkPoint5 = check1Point(point5, Obstacle, 5);
    glm::vec2 point6 = glm::vec2(Player.Position.x + Player.Size.x/2, Player.Position.y + Player.Size.y/2);
    bool checkPoint6 = check1Point(point6, Obstacle, 6);
    glm::vec2 point7 = glm::vec2(Player.Position.x + Player.Size.x/2, Player.Position.y);
    bool checkPoint7 = check1Point(point7, Obstacle, 7);
    glm::vec2 point8 = glm::vec2(Player.Position.x + Player.Size.x, Player.Position.y + Player.Size.y/2);
    bool checkPoint8 = check1Point(point8, Obstacle, 8);
    glm::vec2 point9 = glm::vec2(Player.Position.x + Player.Size.x/2, Player.Position.y + Player.Size.y);
    bool checkPoint9 = check1Point(point9, Obstacle, 9);
    return checkPoint1 || checkPoint2 || checkPoint3 || checkPoint4 || checkPoint5 || checkPoint6 || checkPoint7 || checkPoint8 || checkPoint9;
}

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height), Stuck(true), LastTime(), CoinsCount(0), StartTime(), Level(0), Distance(0), CoinsBeforeLevel(0)
{

}

Game::~Game()
{
    delete Renderer;
    delete Player;
    // delete Obstacle;
    for(int i=0;i<this->FlashingLights.size();i++)
    {
        delete this->FlashingLights[i];
    }
    for(int i =0;i<this->Zappers.size();i++)
    {
        delete this->Zappers[i];
    }
    for(int i =0;i<this->Coins.size();i++)
    {
        delete this->Coins[i];
    }
}

void Game::Init()
{   
    srand(glfwGetTime());
    curr_wd = (char *) malloc(4096*sizeof(char));
    chdir("..");
    getcwd(curr_wd, 4096);
    std::string current_wd(curr_wd);
    char *sprite_vertex, *sprite_fragment, *glow_vertex, *glow_fragment, *jet_vertex, *jet_fragment, *text_vertex, *text_fragment;
    sprite_vertex = (char *) malloc(4096*sizeof(char));
    sprite_fragment = (char *) malloc(4096*sizeof(char));
    glow_vertex = (char *) malloc(4096*sizeof(char));
    glow_fragment = (char *) malloc(4096*sizeof(char));
    jet_vertex = (char *) malloc(4096*sizeof(char));
    jet_fragment = (char *) malloc(4096*sizeof(char));
    text_vertex = (char *) malloc(4096*sizeof(char));
    text_fragment = (char *) malloc(4096*sizeof(char));
    strcpy(sprite_vertex, curr_wd);
    strcpy(sprite_fragment, curr_wd);
    strcpy(glow_vertex, curr_wd);
    strcpy(glow_fragment, curr_wd);
    strcpy(jet_vertex, curr_wd);
    strcpy(jet_fragment, curr_wd);
    strcpy(text_vertex, curr_wd);
    strcpy(text_fragment, curr_wd);
    std::strcat(sprite_vertex, "/src/shaders/sprite.vs");
    std::strcat(sprite_fragment, "/src/shaders/sprite.frag");
    std::strcat(glow_vertex, "/src/shaders/glow.vs");
    std::strcat(glow_fragment, "/src/shaders/glow.frag");
    std::strcat(jet_vertex, "/src/shaders/jetpack.vs");
    std::strcat(jet_fragment, "/src/shaders/jetpack.frag");
    std::strcat(text_vertex, "/src/shaders/text.vs");
    std::strcat(text_fragment, "/src/shaders/text.fs");
    ResourceManager::LoadShader(sprite_vertex, sprite_fragment, nullptr, "sprite");
    ResourceManager::LoadShader(glow_vertex, glow_fragment, nullptr, "smooth");
    ResourceManager::LoadShader(jet_vertex, jet_fragment, nullptr, "smoothJetpack");
    //configure shader
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    glm::mat4 textProjection = glm::ortho(0.0f, static_cast<float>(this->Width), 0.0f,static_cast<float>(this->Height));
    ResourceManager::LoadShader(text_vertex, text_fragment, nullptr, "text");
    ResourceManager::GetShader("text").Use();
    ResourceManager::GetShader("text").SetMatrix4("projection", textProjection);
    ResourceManager::GetShader("smooth").Use().SetMatrix4("projection", projection);
    ResourceManager::GetShader("smoothJetpack").Use().SetMatrix4("projection", projection);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    this->TextInit();
    myTextShader = ResourceManager::GetShader("text");
    //set renderer specific controls
    Shader myShader;
    myShader = ResourceManager::GetShader("sprite");
    Renderer = new SpriteRenderer(myShader);
    Shader smoothShader = ResourceManager::GetShader("smooth");
    SmoothRenderer = new SpriteRenderer(smoothShader);
    Shader jetpackShader = ResourceManager::GetShader("smoothJetpack");
    JetpackRenderer = new SpriteRenderer(jetpackShader);
    //load textures
    // ResourceManager::LoadTexture("/home/divijd/Desktop/Semester_4/Computer Graphics/Breakout/src/textures/sonic.png", true, "sonic");
    char *background = (char *) malloc(4096*sizeof(char));
    char *block = (char *) malloc(4096*sizeof(char));
    char *sonic = (char *) malloc(4096*sizeof(char));
    char *face = (char *) malloc(4096*sizeof(char));
    strcpy(background, curr_wd);
    strcpy(block, curr_wd);
    strcpy(sonic, curr_wd);
    strcpy(face, curr_wd);
    std::strcat(background, "/src/textures/background.jpg");
    std::strcat(block, "/src/textures/block.png");
    std::strcat(sonic, "/src/textures/sonic.png");
    std::strcat(face, "/src/textures/awesomeface.png");
    ResourceManager::LoadTexture(background, false, "background");
    ResourceManager::LoadTexture(block, false, "block");
    ResourceManager::LoadTexture(sonic, true, "sonic");
    ResourceManager::LoadTexture(face, true, "face");
    // configure game objects
    glm::vec2 playerPos = glm::vec2(10.0f, this->Height - PLAYER_SIZE.y);
    Player = new PlayerObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("sonic"));
    GameObject *temp;
    for(int i = 1;i<=300;i++)
    {
        glm::vec2 tempPos = glm::vec2(400.0f * i, 20.0f);
        glm::vec2 tempSize = glm::vec2(30.0f, 30.0f);
        temp = new GameObject(tempPos, tempSize, ResourceManager::GetTexture("block"), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(SPEED[this->Level], 0.0f));
        this->FlashingLights.push_back(temp);
    }
    // Obstacle = new GameObject(glm::vec2(105.324f, 40.0f), glm::vec2(20.0f, 20.0f), ResourceManager::GetTexture("block"), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f), 45.0f);
}

int Game::TextInit()
{
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

	// find path to font
    std::string pathing(curr_wd);
    std::string font_name = pathing + "/src/fonts/Antonio-Bold.ttf";
    if (font_name.empty())
    {
        std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
        return -1;
    }
	
	// load font as face
    FT_Face face;
    if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }
    else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            Characters.insert(std::pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return 0;
}

void Game::GoToNextLevel()
{
    this->Level++;
    this->CoinsBeforeLevel = this->CoinsCount;
    this->State = GAME_MENU;
    this->Reset();
}

void Game::Reset()
{   
    this->StartTime = glfwGetTime();
    this->Distance = 0;
    this->Stuck = true;
    for(int i=0;i<this->FlashingLights.size();i++)
    {
        delete this->FlashingLights[i];
    }
    for(int i =0;i<this->Zappers.size();i++)
    {
        delete this->Zappers[i];
    }
    for(int i =0;i<this->Coins.size();i++)
    {
        delete this->Coins[i];
    }
    this->FlashingLights.clear();
    this->Zappers.clear();
    this->Coins.clear();
    this->LastTime = glfwGetTime();
    this->LastTimeCoins = glfwGetTime();
    GameObject *temp;
    for(int i = 1;i<=300;i++)
    {
        glm::vec2 tempPos = glm::vec2(400.0f * i, 20.0f);
        glm::vec2 tempSize = glm::vec2(30.0f, 30.0f);
        temp = new GameObject(tempPos, tempSize, ResourceManager::GetTexture("block"), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(SPEED[this->Level], 0.0f));
        this->FlashingLights.push_back(temp);
    }
    Player->Position = glm::vec2(10.0f, this->Height - PLAYER_SIZE.y);
    PLAYER_ACCELERATION = 0.0f;
}

void Game::DoCollisions()
{
    for (int i=0;i<this->Zappers.size();i++)
    {
        if(CheckCollision(*Player, *(this->Zappers[i])))
        {
            float angle = ((int)this->Zappers[i]->Rotation) % 90;
            if(angle >= 3 && angle <= 87)
            {
                if(CheckCollisionSkew(*Player, *(this->Zappers[i])))
                {
                    this->State = GAME_LOSE;
                }
            }
            else
            {
                this->State = GAME_LOSE;
            }
            // if(CheckCollisionSkew(*Player, *(this->Zappers[i])))
            //     this->Reset();
        }
    }
    for (int i=0;i<this->Coins.size();i++)
    {
        if(CheckCollision(*Player, *(this->Coins[i])))
        {
            if(this->Coins[i]->Destroyed == false)
                this->CoinsCount++;
            this->Coins[i]->Destroyed = true;
        }
    }
} 

void Game::Update(float dt)
{   
    if(this->State == GAME_MENU)
    {
        sleep(3);
        this->State = GAME_ACTIVE;
    }
    if(!this->Stuck && this->State == GAME_ACTIVE)
    {   
        Distance = (glfwGetTime() - this->StartTime) * SPEED[this->Level];
        this->DoCollisions();
        if((float)((float) glfwGetTime() - (float) this->StartTime) > (float) TIME_TAKEN[this->Level] && !this->Stuck)
        {
            if(this->Level == 2)
            {
                this->State = GAME_WIN;
            }
            else
            {
                GoToNextLevel();
            }
        }
        for(int i=0;i<this->FlashingLights.size();i++)
        {
            if(!this->Stuck)
                this->FlashingLights[i]->Move(dt);
        }
        for(int i=0;i<this->Zappers.size();i++)
        {
            if(!this->Stuck)
                this->Zappers[i]->Move(dt);
        }
        for(int i=0;i<this->Coins.size();i++)
        {
            if(!this->Stuck)
                this->Coins[i]->Move(dt);
        }
        float velocity = dt * PLAYER_VELOCITY/3;
        velocity += PLAYER_ACCELERATION;
        if(Player->Position.y >= this->Height - Player->Size.y - 4.0f && !this->Stuck)
                    PLAYER_ACCELERATION = 0;
        if (Player->Position.y <= this->Height - Player->Size.y && !this->Stuck)
        {
            if(trackInput == 0)
            {   
                PLAYER_ACCELERATION += 10.0f * dt;
                if(PLAYER_ACCELERATION < 0)
                    PLAYER_ACCELERATION += 15.0f * dt;
            }
            Player->Position.y += velocity;
        }
        GameObject *temp;
        if((float) ((float) glfwGetTime() - (float) this->LastTime) > (float) DIFFICULTY_ZAPPER[this->Level] && !this->Stuck)
        {
            this->LastTime = glfwGetTime();
            float yPos;
            int yPre = rand()%5;
            float ySize = 150.0f +  rand()%150;
            glm::vec2 tempSize = glm::vec2(30.0f, ySize);
            if(yPre == 0)
                yPos = 2.0f;
            else if(yPre == 1)
                yPos = 598.0f - ySize;
            else
                yPos = rand()%(598 - (int) ySize);
            glm::vec2 tempPos = glm::vec2(900.0f, yPos);
            int rota = rand()%7;
            float rotation;
            float isRotating = 0.0f;
            if(rota == 0)
            {
                rotation = 0.0f;
            }
            else if(rota == 1 || rota == 2)
            {
                rotation = 30.0f + rand()%150;
                // rotation = glm::radians(angle);
            }
            else
            {
                rotation = rand()%180;
                isRotating = 0.5 + rand()/RAND_MAX;
            }
            temp = new GameObject(tempPos, tempSize, ResourceManager::GetTexture("block"), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(SPEED[this->Level] + this->Level*rand()%150, 0.0f), rotation, isRotating);
            this->Zappers.push_back(temp);
        }
        int coin_chance = rand()%2;
        if(coin_chance == 0 && (float) glfwGetTime() - (float) LastTimeCoins > COINS_SPAWN[this->Level] && !this->Stuck)
        {
            LastTimeCoins = glfwGetTime();
            float xcSize = 30.0f;
            float ycSize = 30.0f;
            float xcPos = 800.0f;
            float ycPos = rand()%(598 - (int) ycSize);
            temp = new GameObject(glm::vec2(xcPos, ycPos), glm::vec2(xcSize, ycSize), ResourceManager::GetTexture("face"), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(SPEED[this->Level] + 200*this->Level + this->Level*rand()%100, 0.0f));
            this->Coins.push_back(temp);
        }
    }
}

void Game::ProcessInput(float dt)
{
    if (this->State == GAME_ACTIVE)
    {
        // std::cout<<trackInput<<std::endl;
        float velocity = PLAYER_VELOCITY * dt;
        if (this->Keys[GLFW_KEY_W])
        {   
            
            if(trackInput == 0)
            {
                trackInput = 1;
            }
            else
            {
                PLAYER_ACCELERATION -= 5.0f * dt;
                if(PLAYER_ACCELERATION > 0)
                    PLAYER_ACCELERATION -= 15.0f * dt;
            }
            if(Player->Position.y <= 4.0f)
            {
                
                PLAYER_ACCELERATION = 0;
                velocity = PLAYER_VELOCITY * dt;
            }
            if (Player->Position.y >= 0.0f && !this->Stuck)
            {   
                 Player->Position.y -= velocity;
            }
        }
        else if(trackInput == 1)
        {
            trackInput = 0;
        }
        if (this->Keys[GLFW_KEY_SPACE])
        {   
            if(this->Stuck)
            {
                this->StartTime = glfwGetTime();
                this->LastTime = glfwGetTime();
                this->LastTimeCoins = glfwGetTime();
            }
            this->Stuck = false;
        }
    }

    if(this->State == GAME_WIN)
    {
        if (this->Keys[GLFW_KEY_SPACE])
        {   
            sleep(0.1);
            this->CoinsCount = 0;
            this->CoinsBeforeLevel = 0;
            this->Level = 0;
            this->State = GAME_ACTIVE;
            this->Reset();
        }
    }
    if(this->State == GAME_LOSE)
    {
        if (this->Keys[GLFW_KEY_SPACE])
        {   
            sleep(0.1);
            this->CoinsCount = this->CoinsBeforeLevel;
            this->State = GAME_ACTIVE;
            this->Reset();
        }
    }
}

void Game::Render()
{
    if(this->State == GAME_ACTIVE)
    {
        //draw background
        Texture2D myTexture;
        myTexture = ResourceManager::GetTexture("background");
        Renderer->DrawSprite(myTexture, glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f, glm::vec3(0.5f, 0.5f, 0.5f));
        //draw Level
        // this->Levels[this->Level].Draw(*Renderer);
        // Ball->Draw(*Renderer);
        for(GameObject* &iterator : this->FlashingLights)
        {
            iterator->Draw(*Renderer);
        }
        for(GameObject* &iterator : this->Zappers)
        {
            iterator->Draw(*SmoothRenderer);
        }
        for(GameObject* &iterator : this->Coins)
        {
            iterator->Draw(*Renderer);
        }
        if(trackInput == 0)
            Player->Draw(*Renderer);
        else if(trackInput == 1)
            Player->Draw(*JetpackRenderer);
        else
            Player->Draw(*Renderer);
        std::string coinsString = "Coins : " + std::to_string(this->CoinsCount);
        std::string distanceString = "Distance : " + std::to_string(this->Distance) + "/" + std::to_string((int)(TIME_TAKEN[this->Level]*SPEED[this->Level]));
        this->RenderText(myTextShader, coinsString, 0.0f, this->Height - 50.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
        this->RenderText(myTextShader, distanceString, 0.0f, this->Height - 90.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
        std::string levelString = "Level : " + std::to_string(this->Level + 1);
        this->RenderText(myTextShader, levelString, 0.0f, this->Height - 130.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
        // this->RenderText(myTextShader, "Welcome to CG Course", 200.0f,200.0f,1.0f, glm::vec3(0.5, 0.8f, 0.2f));
        // Obstacle->Draw(*Renderer);

    }

    else if(this->State == GAME_MENU)
    {
        std::string print_message = "WELL DONE!! You completed Level " + std::to_string(this->Level);
        std::string second_message = "Now proceeding to next level...";
        this->RenderText(myTextShader, print_message, 60.0f, this->Height - 150.0f, 1.0f, glm::vec3(0.3, 0.7f, 0.9f));
        this->RenderText(myTextShader, second_message, 60.0f, this->Height - 250.0f, 1.0f, glm::vec3(0.3f, 0.7f, 0.9f));
    }

    else if(this->State == GAME_WIN)
    {
        std::string win_message = "CONGRATS! YOU HAVE FINISHED THE GAME!!!";
        this->RenderText(myTextShader, win_message, 20.0f, this->Height - 250.0f, 1.0f, glm::vec3(0.3f, 0.7f, 0.9f));
        std::string message = "Press Spacebar to start again";
        this->RenderText(myTextShader, message, 0.0f, 60.0f, 0.5f, glm::vec3(0.3f, 0.7f, 0.9f));
        std::string score_message = "SCORE : " + std::to_string(this->CoinsCount);
        this->RenderText(myTextShader, score_message, 0.0f, 20.0f, 0.5f, glm::vec3(0.3f, 0.7f, 0.9f));
    }
    else if(this->State == GAME_LOSE)
    {
        std::string lose_message = "Uh oh! YOU HAVE LOST. PRESS SPACEBAR TO RETRY";
        this->RenderText(myTextShader, lose_message, 20.0f, this->Height - 250.0f, 0.8f, glm::vec3(0.3f, 0.7f, 0.9f));
        std::string score_message = "SCORE : " + std::to_string(this->CoinsCount);
        this->RenderText(myTextShader, score_message, 0.0f, 20.0f, 0.5f, glm::vec3(0.3f, 0.7f, 0.9f));
    }
}

void Game::RenderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
    shader.Use();
    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    // shader.SetVector3f("textColor", color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    // std::cout<<text<<std::endl;
    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) 
    {
        Character ch = Characters[*c];
        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}