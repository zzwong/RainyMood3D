#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D screen;

uniform bool gauss;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

uniform bool neon;

void main(){
    color = texture(screen,TexCoords);
    
    //Gaussian blur
    if(gauss){
        vec2 tex_offset = 1.0 / textureSize(screen, 0); // gets size of single texel
        vec3 result = texture(screen, TexCoords).rgb * weight[0]; // current fragment's contribution
        for(int i = 1; i < 5; ++i)
        {
            result += texture(screen, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(screen, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
        
        color = vec4(result, 1.0f);
    }
}
