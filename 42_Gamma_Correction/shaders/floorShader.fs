#version 330 core

out vec4 FragColor;

in VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform vec3 viewPos;
uniform bool blinn;
uniform bool gamma;

uniform sampler2D texture1;

vec3 BlinnPhong(vec3 normal, vec3 fragPos, vec3 lightPos, vec3 lightColor)
{
    // Ambient
    vec3 ambient = 0.05 * lightColor;

    // Diffuse
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    vec3 viewDir = normalize(viewPos - fragPos);
    float spec = 0.0;

    if(blinn){
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), 8.0);
    }
    else{
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }

    vec3 specular = spec * lightColor;

    // simple attenuation
    float max_distance = 1.5;
    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (gamma ? distance * distance : distance);
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}


void main()
{           
    vec3 color = texture(texture1, fs_in.TexCoords).rgb;
    vec3 lighting = vec3(0.0);
    for(int i = 0; i < 4; ++i)
        lighting += BlinnPhong(normalize(fs_in.Normal), fs_in.FragPos, lightPositions[i], lightColors[i]);
    color *= lighting;
    if(gamma)
        color = pow(color, vec3(1.0/2.2));
    FragColor = vec4(color, 1.0);
}


// void main()
// {
//     vec3 color = texture(texture1, fs_in.TexCoords).rgb;

//     // Ambient
//     vec3 ambient = 0.05 * color;

//     // Diffuse
//     vec3 lightDir = normalize(lightPos - fs_in.FragPos);
//     vec3 normal = normalize(fs_in.Normal);
//     float diff = max(dot(lightDir, normal), 0.0);
//     vec3 diffuse = diff * color;

//     // Specular
//     vec3 viewDir = normalize(viewPos - fs_in.FragPos);
//     float spec = 0.0;

//     if(blinn){
//         vec3 halfwayDir = normalize(lightDir + viewDir);
//         spec = pow(max(dot(normal, halfwayDir), 0.0), 8.0);
//     }
//     else{
//         vec3 reflectDir = reflect(-lightDir, normal);
//         spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
//     }

//     vec3 specular = vec3(1.0) * spec;

//     FragColor = vec4(ambient + diffuse + specular, 1.0);

//     // float gamma = 2.2;
//     // FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / gamma));
// }