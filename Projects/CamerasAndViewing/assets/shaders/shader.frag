#version 330 core
out vec4 FragColour;
in vec2 TexCoord;

uniform sampler2D texture;
uniform int shaderState;

void main()
{
    vec4 baseColour = texture(texture, TexCoord);
    switch(shaderState)
    {
        case 0:
            FragColour = baseColour;
            break;

        case 1:
            vec4 negColour = vec4(1.0 - baseColour.rgb, baseColour.a);
            FragColour = negColour;
            break;

        case 2:
            float averageGrey = (0.299 * baseColour.r) + (0.587 * baseColour.g) + (0.114 * baseColour.b);
            vec4 greyColour = vec4(averageGrey,averageGrey,averageGrey,baseColour.a);
            FragColour = greyColour;
            break;

        default:
            FragColour = baseColour;
    }
}