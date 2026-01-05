#version 330 core
out vec4 FragColour;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 objectColour;
uniform vec3 lightColour;
uniform vec3 lightPos; // We need the light's position for specular lighting
uniform vec3 viewPos; // we need the viewpos to calculate specular
uniform int shineLevel; // easy modifer for light shine

void main()
{
    float ambientStrength = 0.1f;
    float specularStrength = 0.5f; // controls how bright the specular light is

/* This is ambient lighting (The global light level the object has
       regardless of any light source */
    vec3 ambient = ambientStrength * lightColour;

    vec3 norm = normalize(Normal); // we only need the direction of the normal vector
    vec3 lightDir = normalize(lightPos - FragPos); // calculates where the light is going

/* Diffused Lighting calculates lighting strength based on the angle
       the normal vector of the object fragments, and the lighting vector.
       The the closer the angle between the two vectors are parallel, the stronger
       intensity the light is.*/
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColour; // gives final diffused light value

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

/*Specular Lighting*/

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shineLevel);
    vec3 specular = specularStrength * spec * lightColour;

    vec3 result = (ambient + diffuse + specular);
    FragColour = vec4(result * objectColour, 1.0);
}