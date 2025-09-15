/*For shaders to really be effective we need a method 
of inputing and outputing data, to modify different
elements rendered to the screen.

To do this in GLSL we use 'in' and 'out' respectively.
A vertex shader needs input data to be useful, we get 
the vertex data defined on the cpu. 

But to use this data we first need to link the shader 
to the location the VBO is.
*/

# version 330 core
/*
this tells the vertex shader to look for a 3D vector at location 0,
where the VBO will be defined at later in the code
*/
layout(location = 0) in vec3 aPos;


// Vertex Shaders control how vertices get placed on the screen
void main()
{
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}


/*
Theres also another way to link the location to the shader on the 
CPU code, see glGetAtribLocation() example
*/
