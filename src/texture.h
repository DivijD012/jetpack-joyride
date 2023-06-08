#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <glad/glad.h>

class Texture2D
{
    public:
        //holds ID of the texture object
        unsigned int ID;
        //texture image dimensions
        unsigned int Width, Height;
        //texture Format
        unsigned int Internal_Format;
        unsigned int Image_Format;
        //texture config
        unsigned int Wrap_S;
        unsigned int Wrap_T;
        unsigned int Filter_Min;
        unsigned int Filter_Max;
        //constructor
        Texture2D();
        void Generate(unsigned int width, unsigned int height, unsigned char *data);
        void Bind() const;
};

#endif