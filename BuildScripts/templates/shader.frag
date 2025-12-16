#version 330 core
out vec4 FragColor;
in vec3 ourColour;

void main()
{
    FragColor = ourColour;
}