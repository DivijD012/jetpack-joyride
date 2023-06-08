#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec3 spriteColor;


float smoothStep(float outer_edge, float inner_edge, float x)
{
    float interpolated_value = smoothstep(outer_edge, inner_edge, x); // Compare with built-in
    // if(interpolated_value < 1.0)
    //     return interpolated_value/2;
    return interpolated_value*3/2;
}

void main()
{    
    if(TexCoords.x > 0.5 && TexCoords.y > 0.5)
    {
        color = vec4(1.0f,1.0f,0.0f,1.0f*smoothStep(0.0f,0.4f,1-TexCoords.x)*smoothStep(0.0f,0.4f,1-TexCoords.y));
    }
    else if(TexCoords.x<=0.5 && TexCoords.y<=0.5)
    {
        color = vec4(1.0f,1.0f,0.0f,1.0f*smoothStep(1.0f,0.6f,1-TexCoords.x)*smoothStep(1.0f,0.6f,1-TexCoords.y));
    }
    else if(TexCoords.x>0.5 && TexCoords.y<=0.5)
    {
        color = vec4(1.0f,1.0f,0.0f,1.0f*smoothStep(0.0f,0.4f,1-TexCoords.x)*smoothStep(1.0f,0.6f,1-TexCoords.y));
    }
    else
    {
        color = vec4(1.0f,1.0f,0.0f,1.0f*smoothStep(1.0f,0.6f,1-TexCoords.x)*smoothStep(0.0f,0.4f,1-TexCoords.y));
    }

}