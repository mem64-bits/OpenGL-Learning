#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord);
}


// Greyscale
/*#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    vec4 texColor = texture(ourTexture, TexCoord);
    float luma = dot(texColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    FragColor = vec4(vec3(luma), texColor.a);
}*/