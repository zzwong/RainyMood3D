#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D screen;

uniform bool gauss;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

uniform bool neon;
int rad = 5;

uniform bool half_tone;
uniform int pixelsPerRow;

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
    //Neon filter
    if(neon){
        int i = 0;
        int j= 0;
        vec4 sum = vec4(0.0);
        
        for( i=-5;i<5;i++) {
            for( j=-5;j<5;j++) {
                sum += texture(screen, TexCoords + vec2(j,i))*0.05;
            }
        }
        
        color = sum*sum+ vec4(texture(screen, TexCoords).rgb, 1.0);
    }
    //Half-tone filter
    if(half_tone){
        vec2 p = TexCoords;
        float pixelSize = 1.0 / float(pixelsPerRow);
        
        float dx = mod(p.x, pixelSize) - pixelSize*0.5;
        float dy = mod(p.y, pixelSize) - pixelSize*0.5;
        
        p.x -= dx;
        p.y -= dy;
        vec3 col = texture(screen, p).rgb;
        float bright = 0.3333*(col.r+col.g+col.b);
        
        float dist = sqrt(dx*dx + dy*dy);
        float rad = bright * pixelSize * 0.8;
        float m = step(dist, rad);
        
        vec3 col2 = mix(vec3(0.0), vec3(1.0), m);
        
        color = vec4(col2, 1.0);
    }
}
