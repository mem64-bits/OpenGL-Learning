#version 330 core
out vec4 FragColour;

in vec2 TexCoord;
in vec3 ourColour;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform int shaderState;

void main()
{
    vec4 bgTex = texture(texture1, TexCoord);
    vec4 fgTex = texture(texture2, TexCoord);
    vec4 baseColour = mix(bgTex, fgTex, fgTex.a);

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