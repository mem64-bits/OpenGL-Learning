#version 330 core
out vec4 FragColour;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 objectColour;
uniform vec3 lightColour;
uniform vec3 lightPositions[3]; // We need the light's position for specular lighting
uniform vec3 viewPos; // we need the viewpos to calculate specular
uniform int shineLevel; // easy modifer for light shine


void main()
{
    float lightIntensity = 1.0f;
    float ambientStrength = 0.05f;
    float specularStrength = 2.0f; // controls how bright the specular
    vec3 ambient = ambientStrength * (lightColour * lightIntensity);
    vec3 result = vec3(0.0f, 0.0f, 0.0f);

    vec3 norm = normalize(Normal); // we only need the direction of the normal vector

    for (int i = 0; i < lightPositions.length(); i++) {
        vec3 lightDir = normalize(lightPositions[i] - FragPos); // calculates where the light is going
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * (lightColour * lightIntensity); // gives final diffused light value

        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);

        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shineLevel);
        vec3 specular = specularStrength * spec * (lightColour * lightIntensity);
        result += (diffuse + specular);
        lightIntensity *= 0.75f;
    }

    FragColour = vec4((ambient + result) * objectColour, 1.0);
}