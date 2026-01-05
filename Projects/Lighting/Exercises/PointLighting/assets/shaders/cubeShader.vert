#version 330 core
layout (location = 0) in vec3 aPos;

/*For diffuse lighting we need to calculate the normal vector
of each vertex of the object.

A normal vector is a vector that is perpendicular to the
surface of a vertex.*/
layout (location = 1) in vec3 aNormal;

/* defines the 3 essential matrices to apply rotation,
movement to object in 3D space, and to displayed correctly
by the 3D viewport.*/

/*uniforms for controlling the objects transformations, camera location,
and world view.*/
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos; // we need the actual position of the fragment
out vec3 Normal; // outputs normal vector to be used in frag shader
uniform vec3 GlobalNormal;
void main()
{
    // Applies our new matrices to create proper viewing in 3D space
    // Outputs positions needed to calculate diffuse lighting
    FragPos = vec3(model * vec4(aPos, 1.0));

    // This is fine however it doesn't account for lighting in global space
    // Normal = aNormal
    
    // transposing in a shader can be slow, but for learning this is fine
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}