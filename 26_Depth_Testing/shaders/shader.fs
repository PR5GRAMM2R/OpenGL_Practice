#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main(){
    FragColor = texture(texture_diffuse1, TexCoords);
}


// // diffuse 텍스쳐와 specular 텍스쳐
// struct Material {
//     sampler2D diffuse;
//     sampler2D specular;    
//     float shininess;
// };

// uniform Material material;

// // Directional Light
// struct DirLight {
//     vec3 direction;
	
//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;
// };

// uniform DirLight dirLight;

// // Point Light
// struct PointLight{
//     vec3 position;

//     float constant;     // 거리에 따른 빛의 감쇠
//     float linear;
//     float quadratic;

//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;
// };

// #define NR_POINT_LIGHTS 4
// uniform PointLight pointLights[NR_POINT_LIGHTS];

// // Spot Light
// struct SpotLight {
//     vec3 position;
//     vec3 direction;
//     float cutOff;       // 안쪽 원뿔 각도
//     float outerCutOff;  // 바깥쪽 원뿔 각도
  
//     float constant;     // 거리에 따른 빛의 감쇠
//     float linear;
//     float quadratic;
  
//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;       
// };

// uniform SpotLight spotLight;

// in vec3 FragPos;    // 위치 벡터들
// in vec3 Normal;     // Normal Vector들
// in vec2 TexCoords;  // 텍스쳐 좌표들

// uniform vec3 viewPos;

// vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
// vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
// vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

// void main()
// {
//     // 속성들
//     vec3 norm = normalize(Normal);
//     vec3 viewDir = normalize(viewPos - FragPos);

//     // 1 단계: Directional lighting
//     vec3 result = CalcDirLight(dirLight, norm, viewDir);

//     // 2 단계: Point lights
//     for(int i = 0; i < NR_POINT_LIGHTS; i++)
//         result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);

//     // 3 단계: Spot light
//     result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
    
//     FragColor = vec4(result, 1.0);
// }

// vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)    // Directional Light 적용
// {
//     vec3 lightDir = normalize(-light.direction);

//     // diffuse shading
//     float diff = max(dot(normal, lightDir), 0.0);

//     // specular shading
//     vec3 reflectDir = reflect(-lightDir, normal);
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

//     // 결과들을 결합
//     vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
//     vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
//     vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

//     return (ambient + diffuse + specular);
// }

// vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)  // Point Light 적용
// {
//     vec3 lightDir = normalize(light.position - fragPos);

//     // diffuse shading
//     float diff = max(dot(normal, lightDir), 0.0);

//     // specular shading
//     vec3 reflectDir = reflect(-lightDir, normal);
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

//     // attenuation
//     float distance    = length(light.position - fragPos);
//     float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
//     // 결과들을 결합
//     vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
//     vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
//     vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
//     ambient  *= attenuation;
//     diffuse  *= attenuation;
//     specular *= attenuation;

//     return (ambient + diffuse + specular);
// }

// vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)    // Spot Light 적용
// {
//     vec3 lightDir = normalize(light.position - fragPos);

//     // diffuse shading
//     float diff = max(dot(normal, lightDir), 0.0);

//     // specular shading
//     vec3 reflectDir = reflect(-lightDir, normal);
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

//     // attenuation
//     float distance = length(light.position - fragPos);
//     float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
//     // spotlight intensity
//     float theta = dot(lightDir, normalize(-light.direction)); 
//     float epsilon = light.cutOff - light.outerCutOff;
//     float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
//     // combine results
//     vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
//     vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
//     vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
//     ambient *= attenuation * intensity;
//     diffuse *= attenuation * intensity;
//     specular *= attenuation * intensity;

//     return (ambient + diffuse + specular);
// }