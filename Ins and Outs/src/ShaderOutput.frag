/*The role of a fragment shader is to control the colour of every
pixel of a shape, defined by vertex points of shown on the screen

To change the colour of a pixel we need a way to update the
fragment colour. 

To do this we output a new result using 
the keyword 'out'
*/

#version 330 core

// This is the output variable that will be 
out vec4 FragColor;

void main(){
    FragColor = vec4(1.0,1.0,1.0,1.0);
    
}