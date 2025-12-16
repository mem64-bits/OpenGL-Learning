/*Just like C/C++ GLSL has different datatypes that we can use 
e.g int float double. But GLSL also has special datatypes that are 
helpful for doing linear algebra operations, on graphics*/

/*Vectors are very important as they can hold data
like the colour of a pixel (R,G,B,A), vertex postions
e.g (x,y,z), and they allow us to easily manipulate,
huge amounts of data at a time*/

/* in GLSL vectors come in the form vecn, where every element 
of the vector is a float

Vectors can also be types other than float

vecn: the default vector of n floats.
bvecn: a vector of n booleans.
ivecn: a vector of n integers.
uvecn: a vector of n unsigned integers.
dvecn: a vector of n double components.

vecn is what is used most of the time as floats are sufficient
*/

vec2 pos = (0.4, 0.9);
vec3 vect = (0.3, 0.2, 0.7);
vec4 matrix = (0.2, 0.5, 0.6, 0.4);

/*We can also combine smaller vectors to
 become bigger vec2 + vec2 = vec4*/

vec4 result = (pos + 0.0, 0.0);
