#version 330 core

// This is a sample fragment shader.

// Directional light
struct DirLight {
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    int isOn;
};

struct PointLight {
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float quadratic;
    
    int isOn;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float cutOff;
    float exponent;
    
    float quadratic;
    
    int isOn;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;

uniform DirLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;
uniform Material material;

uniform bool isPoint;
uniform int pointType;


uniform vec3 lightColor;

out vec4 color;


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    result += CalcPointLight(pointLight, norm, FragPos, viewDir);
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

    color = vec4(result, 1.0f);
//    color = vec4(0.39f,0.19f,0.0f,1.0f);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
        vec3 lightDir = normalize(vec3(1.0f,1.0f,1.0f));
        // Diffuse shading
        float diff = max(dot(normal, lightDir), 0.0);
        
        // Specular shading
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        
        // Combine results
        vec3 ambient  = vec3(0.3,0.3,0.3)  * material.ambient;
        vec3 diffuse  = vec3(0.4,0.4,0.4)  * diff * material.diffuse;
        vec3 specular = vec3(0.8,0.8,0.8) * spec * material.specular;
        return (ambient + diffuse + specular);

}

// Calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    if (light.isOn == 1) {
        vec3 lightDir = normalize(light.position - fragPos);
        
        // Diffuse shading
        float diff = max(dot(normal, lightDir), 0.0);
        
        // Specular shading
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        
        // Attenuation
        float distance = length(light.position - fragPos);
        float attenuation = 1.0f / (light.quadratic * (distance * distance));
        
        // Combine results
        vec3 ambient = light.ambient * material.ambient;
        vec3 diffuse = light.diffuse * diff * material.diffuse;
        vec3 specular = light.specular * spec * material.specular;
        //        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;
        return (ambient + diffuse + specular);
    }
    return vec3(0,0,0);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    if (light.isOn == 1) {
        vec3 ambient = light.ambient * material.ambient;
        
        vec3 lightDir = normalize(light.position - fragPos);
        
        if (acos(dot(normalize(-light.direction),lightDir)) <= light.cutOff) {
            
            // Diffuse shading
            float diff = max(dot(normal, lightDir), 0.0);
            
            // Specular shading
            vec3 reflectDir = reflect(-lightDir, normal);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
            
            // Attenuation
            float distance = length(light.position - fragPos);
            float attenuation = pow(max(dot(-light.direction,lightDir),0),light.exponent) / (light.quadratic * (distance * distance));
            
            // Combine results
//            vec3 ambient = light.ambient * material.ambient;
            vec3 diffuse = light.diffuse * diff * material.diffuse;
            vec3 specular = light.specular * spec * material.specular;
            //        ambient *= attenuation;
            diffuse *= attenuation;
            specular *= attenuation;
            return (ambient + diffuse + specular);
        }
        return ambient;
    }
    return vec3(0,0,0);
}
