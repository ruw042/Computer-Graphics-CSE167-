#version 330 core

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 vertNormal;

uniform mat4 MVP;
uniform mat4 m_viewModel;
uniform vec4 Light;
uniform mat3 NM;
uniform vec4 diffuse;
uniform vec4 ambient;
uniform vec4 specular;
uniform float shiness;


out vec4 outColor;



void main()
{
    
    vec4 spec = vec4(0.0);
    
    vec3 n = normalize(NM * vertNormal);
    vec4 pos = m_viewModel * vec4(position,1.0f);
    vec3 LightDir = normalize(vec3(Light - pos));
    vec3 eye = normalize(vec3(-pos));
    float length = length(vec3(Light-pos));
    float attenuation = 1/(0.5*length*length);
    
    float intensity = max(dot(n, LightDir), 0.0);
    
    // if the vertex is lit compute the specular term
    if (intensity > 0.0) {
        // compute eye vector and normalize it
        // compute the half vector
        vec3 h = normalize(LightDir + eye);
        
        // compute the specular term into spec
        float intSpec = max(dot(h,n), 0.0);
        spec = specular * pow(intSpec, shiness);
    }
    // add the specular term
    outColor = attenuation*(intensity *  diffuse + spec) + ambient;
    
    
    gl_Position = MVP * vec4(position.x, position.y, position.z, 1.0);
}
