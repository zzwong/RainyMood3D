#version 330 core
// This is a sample fragment shader.
//in vec3 fragPos;
// You can output many things. The first vec4 type output determines the color of the fragment
layout (location = 0) out vec4 color;

uniform sampler2D reflectionTex;
uniform sampler2D refractionTex;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;


uniform vec3 lightColor = vec3(1.f,1.f,1.f);

in vec4 clipSpace;
in vec2 texCoord;
in vec3 toCameraVector;
in vec3 fromLightVector;

uniform float moveFactor;

const float waveStrength = 0.00375f*2.0f*2.0f;
const float shineDamper = 20.0f; //shinedamper
const float reflectivity = 1.0f; //reflectivity

void main()
{
    //Normalize the coordinates (normalized device coordinates)
    vec2 ndc = (clipSpace.xy/clipSpace.w)/2.0f + 0.5f;
    vec2 refractTexCoord = vec2(ndc.x,ndc.y);
    vec2 reflectTexCoord = vec2(ndc.x,-ndc.y);
    
    float near = 0.1f;
    float far = 1000.0f;
    float depth = texture(depthMap, refractTexCoord).r;
    float floorDistance = 2.0f*near*far/(far+near-(2.0f*depth-1.0f)*(far-near));
    
    depth = gl_FragCoord.z;
    float waterDistance = 2.0f*near*far/(far+near-(2.0f*depth-1.0f)*(far-near));
    
    float waterDepth = floorDistance - waterDistance;
    
    //Distortion from dudv map
    vec2 distortedTexCoords = texture(dudvMap, vec2(ndc.x + moveFactor, ndc.y)).rg * 0.1f;
    distortedTexCoords = ndc + vec2(distortedTexCoords.x, distortedTexCoords.y + moveFactor);
    vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0f - 1.0f) * waveStrength * clamp(waterDepth/5.0f,0.0,1.0f);
    
    refractTexCoord += totalDistortion;
    refractTexCoord = clamp(refractTexCoord, 0.001f, 0.999f);
    
    reflectTexCoord += totalDistortion;
    reflectTexCoord.x = clamp(reflectTexCoord.x, 0.001f, 0.999f);
    reflectTexCoord.y = clamp(reflectTexCoord.y, -0.999f, -0.001f);

    vec4 reflectColor = texture(reflectionTex, reflectTexCoord);
    vec4 refractionColor = texture(refractionTex, refractTexCoord);
    
    vec4 normalMapColor = texture(normalMap,distortedTexCoords);
    vec3 normal = vec3(normalMapColor.r * 2.0f - 1.0f, normalMapColor.b * 3.0f, normalMapColor.g * 2.0f - 1.0f);
    normal = normalize(normal);
    
    vec3 viewVector = normalize(toCameraVector);
    float refractiveFactor = dot(viewVector, normal);
    refractiveFactor = pow(refractiveFactor, 2.0f);
    
    vec3 reflectedLight = reflect(normalize(fromLightVector),normal);
    float spec = max(dot(reflectedLight,viewVector),0.0f);
    spec = pow(spec,shineDamper);
    vec3 specHighlight = lightColor * spec * reflectivity * clamp(waterDepth/5.0f,0.0,1.0f);;

    color = mix(reflectColor, refractionColor, refractiveFactor);
    color = mix(color, vec4(0.0f,0.3f,0.5f,1.0f),0.2f) + vec4(specHighlight,1.0f);
    color.a = clamp(waterDepth/10.0f,0.0,1.0f);
//    color = vec4(waterDepth)/50.0f;
//    color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
//    color = reflectColor;
//    color = refractionColor;
//    color = dudvColor;
//    color = normalMapColor;
}
