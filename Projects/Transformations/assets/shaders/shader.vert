#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColour;
layout (location = 2) in vec2 aTexCoord;


out vec3 ourColour;
out vec2 TexCoord;
/* defines transformation matrix to apply rotation,
movement and other vector caculations*/
uniform mat4 transform;

void main()
{
    /* To apply trsnslations we simply multiply the gl_Position
        by the tranformation matrix*/
    gl_Position = vec4(aPos, 1.0) * transform;
    ourColour = aColour;
    TexCoord = aTexCoord;
}