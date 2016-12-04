#version 330 core
// This is a sample fragment shader.

//in float sampleExtraOutput;
//
//// You can output many things. The first vec4 type output determines the color of the fragment
//out vec4 color;
//
//void main()
//{
//    // Color everything a hot pink color. An alpha of 1.0f means it is not transparent.
//    color = vec4(1.0f, 0.41f, 0.7f, sampleExtraOutput);
//}

struct Material {
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    float shininess;
};

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    bool on;
};
//
struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float quadratic;
    bool on;
};
//
struct SpotLight {
    vec3 direction;
    vec3 position;
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    
    float cutOff;
    float quadratic;
    float exponent;
    bool on;
};
//
//
#define NR_POINT_LIGHTS 5
//
//// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
//// Note that you do not have access to the vertex shader's default output, gl_Position.
in vec3 Normal;
in vec3 FragPos;
in vec3 TexCoords;
//
//// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 color;
//

uniform samplerCube skybox;
uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform PointLight pointLight;
uniform SpotLight spotLight;
uniform Material material;
uniform bool normal_coloring;

uniform bool wire;
uniform bool water;

//
//// Function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
//
void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0f, 0.0f, 0.0f); // holds the result of all the light calculation
//    vec3 temp = CalcPointLight(pointLight, norm, FragPos , viewDir);
//    if (dirLight.on)
//        result += CalcDirLight(dirLight, norm, viewDir);
//    else if(pointLight.on){
//        
//        for (int i = 0; i < NR_POINT_LIGHTS; ++i){
//            result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
//        }
////        result += CalcPointLight(pointLight, norm, FragPos , viewDir);
//    }
//    else if(spotLight.on)
//        result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
//    
//    if (!normal_coloring)
//        color = vec4(result, sampleExtraOutput);
//    else
//        color = vec4(norm, sampleExtraOutput);
    
//    color = texture(skybox, TexCoords);
    if(wire)
        color = vec4(0.0f, .5f, .5f, 1.0f);
    else
        color = vec4(.87f, .3f, 0.0f, 1.0f);
    
    if (water)
        color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
    
    // Color everything a hot pink color. An alpha of 1.0f means it is not transparent.
//    color = vec4(1.0f, 0.41f, 0.7f, sampleExtraOutput);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 ambient = material.ambient * light.ambient;
    vec3 diffuse = (material.diffuse * light.diffuse) * max(dot(normal, lightDir), 0.0);
    vec3 specular = (material.specular * light.specular) * dot(material.specular,light.specular) * pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    return (ambient + diffuse + specular);
}
//
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{

    vec3 lightDir = normalize(light.position - fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // Attenuation
    float a_distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.quadratic * (a_distance * a_distance));
    
    // Combine results
    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}
//
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
 
    // check if lighting is inside the spotlight cone
    float theta = dot(lightDir, normalize(-light.direction));
//    
//    if (theta > light.cutOff)
//    {
        // Diffuse:
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * material.diffuse;

        // Specular
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.specular * spec * material.specular;
    
        // attenuation
        float distance = length(light.position - fragPos);
        float attenuation = 1.0f / (distance + light.quadratic * (distance * distance));
        
        // Ambient
        vec3 ambient = light.ambient * material.ambient;
        
        diffuse *= attenuation;
        specular *= attenuation;
    
//    vec3 result = (ambient + diffuse + specular);
//    float intensity = pow(theta, light.exponent);
//    result *= pow(theta, exp);
//    result = dot(result, intensity);
    return vec3(ambient + diffuse + specular)  ;
//    return result;
//    }
//    else {
//        return (light.ambient * material.diffuse);
//    }
//    return (light.ambient + light.diffuse + light.specular);

}
//    vec3 lightDir = normalize(light.position - fragPos);
//    
//    // Diffuse shading
//    float diff = max(dot(normal, lightDir), 0.0);
//    // Specular shading
//    vec3 reflectDir = reflect(-lightDir, normal);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//    // Attenuation
//    float distance = length(light.position - fragPos);
////    float attenuation = 1.0f / (light.constant + light.exponent * distance + light.quadratic * (distance * distance));
//    float attenuation = (pow(dot(normalize(light.direction), lightDir), light.exponent)) / (light.quadratic * (distance * distance));
//    
//    // Combine results
//    vec3 ambient = light.ambient * material.ambient;
//    vec3 diffuse = light.diffuse * diff * material.diffuse;
//    vec3 specular = light.specular * spec * material.specular;
//    ambient *= attenuation;
//    diffuse *= attenuation;
//    specular *= attenuation;
//    
//    if (acos(dot(light.direction, lightDir)) > light.cutOff)
//        return ambient;
//    
//    
//    return (ambient + diffuse + specular);
    


