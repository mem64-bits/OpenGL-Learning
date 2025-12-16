/*
This is the shader isolated in it's own file. The real shader is 
loaded as a string in the code, we will be setting up a shader class
to handle .glsl / .vert / .frag in t a different section

Uniforms are another way to share data between the CPU and GPU
Unlike shader inputs and outputs, uniforms are global in scope,
meaning that they can be accessed in any shader, at any point in
in the program.

In our OpenGL CPU code we can set the value of the uniform variable,
effectively transfering data from the CPU to the GPU
*/

#version 330 core

out vec4 FragColour;

// Declares a uniform variable 
uniform vec4 ourColour;

void main(){
	FragColour = ourColour;
}