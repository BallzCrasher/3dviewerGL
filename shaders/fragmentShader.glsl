#version 330 core

out vec4 fragColor;

in vec2 TexCoord;

uniform float changer;
uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;

void main(){
    fragColor = mix(texture(ourTexture,TexCoord) , texture(ourTexture2,TexCoord),changer);
}
