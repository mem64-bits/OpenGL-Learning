
/* We know that a shader is a small program that runs on the 
GPU that is executed for every object the shader is appiled to*/ 

/*OpenGL uses a special C-like language called GLSL to define 
shader programs that run on the gpu*/

// GLSL Must declare the version of OpenGL the shader is run on
#version version_number

/*Unlike normal code ran on the cpu, shaders are limited, and must 
receive input data, process it e.g add transformations, math,
and send a finished result as output*/

in type in_variable_name; // 'in' gets input data given to the shader
in type in_variable_name;

/*'out' stands for the output attribute data the shader program will send */
out type out_variable_name;

/*a uniform variable is a variable passed from the CPU to the GPU,
e.g code in our main() function like the time*/
uniform type uniform_name;

/* like C/C++ the main function is the starting point 
of execution for the shader*/
void main()
{
  // process input(s) and do some weird graphics stuff
  ...
  // output processed stuff to output variable
  out_variable_name = weird_stuff_we_processed;
}