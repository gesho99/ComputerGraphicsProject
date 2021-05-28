#version 330 core

struct Material {
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;  
  
uniform vec3 viewPos;

uniform Material material;
uniform Light light;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

void main()
{
    vec3 diffuseColor = vec3(texture(texture_diffuse1, TexCoords));

    // ambient
    vec3 ambient = light.ambient * diffuseColor;

    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * diffuseColor;
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));  
        
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}