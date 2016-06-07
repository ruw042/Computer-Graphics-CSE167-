#version 330 core

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 vertNormal;

uniform mat4 MVP;
uniform mat4 m_viewModel;
uniform vec3 Light;
uniform mat3 NM;
uniform vec4 diffuse;
uniform vec4 ambient;
uniform vec4 specular;
uniform float shiness;

//in vec2 vertTexCoord;

out vec4 outColor;



void main()
{
    
    vec4 spec = vec4(0.0);
    
    vec3 n = normalize(NM * vertNormal);
    
    float intensity = max(dot(n, Light), 0.0);
    
    // if the vertex is lit compute the specular term
    if (intensity > 0.0) {
        // compute position in camera space
        vec3 pos = vec3(m_viewModel * vec4(position,1.0f));
        // compute eye vector and normalize it
        vec3 eye = normalize(-pos);
        // compute the half vector
        vec3 h = normalize(Light + eye);
        
        // compute the specular term into spec
        float intSpec = max(dot(h,n), 0.0);
        spec = specular * pow(intSpec, shiness);
    }
    // add the specular term
    outColor = intensity *  diffuse + spec+ ambient;
    
    
    gl_Position = MVP * vec4(position.x, position.y, position.z, 1.0);
}
