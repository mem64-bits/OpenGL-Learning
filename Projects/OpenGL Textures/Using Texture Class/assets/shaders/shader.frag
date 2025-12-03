#version 330 core
out vec4 FragColour;

in vec2 TexCoord;
in vec3 ourColour;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform bool negativeForm;
uniform bool greyscale;
uniform bool transition;
uniform float transitionProgress;


void main()
{
    vec4 bgTex = texture(texture1, TexCoord);
    vec4 fgTex = texture(texture2, TexCoord);
    vec4 baseColour = mix(bgTex, fgTex, fgTex.a);

    vec4 negBGColour = vec4(1.0f - bgTex.rgb, bgTex.a);
    vec4 negFGColour = vec4(1.0f - fgTex.rgb, fgTex.a);
    vec4 negColour = mix(negBGColour, negFGColour, negFGColour.a);
   

    FragColour = baseColour;
}