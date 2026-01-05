#version 330 core
out vec4 FragColour;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 objectColour;
uniform vec3 lightColour;
uniform vec3 lightPos; // We need the light's position
void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColour;

    // diffuse
    vec3 norm = normalize(Normal); // we only need the direction of the normal vector
    vec3 lightDir = normalize(lightPos - FragPos); // calculates where the light is going
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColour;

    vec3 result = (ambient + diffuse) * objectColour;
    FragColour = vec4(result, 1.0);


}