#version 330 core

in vec4 vColor;    
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos; 
out vec4 FragColor;

uniform mat4 transform;
uniform sampler2D texture1;

struct Light {
    vec3 position;  
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
};

uniform Light light;
uniform vec3 viewPos;
uniform float shininess;
void main()
{
    
   // 环境光处理
    vec3 ambient = light.ambient * texture(texture1, TexCoords).rgb;
    
    // 漫反射光处理 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(texture1, TexCoords).rgb;  
    
    // 镜面光处理
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = light.specular * spec * texture(texture1, TexCoords).rgb;  
    
    
    //光的衰减
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    ambient  *= attenuation; 
    diffuse   *= attenuation;
    specular *= attenuation;   
        
    vec3 result = min(ambient + diffuse + specular,vec3(1.0));
    FragColor = vec4(result, 1.0);
   
}