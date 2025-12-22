#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

/* defines the 3 essential matrices to apply rotation,
movement to object in 3D space, and to displayed correctly
by the 3D viewport*/

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoord = aTexCoord;
    // Applies our new matrices to create proper viewing in 3D space
    gl_Position = projection * view * model * vec4(aPos, 1.0);

}