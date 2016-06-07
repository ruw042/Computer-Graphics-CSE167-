//referenced: http://www.mbsoftworks.sk/index.php?page=tutorials&series=1&tutorial=24 for the shaders

#version 330

//uniform struct Matrices
//{
//    mat4 projMatrix;
//    mat4 modelMatrix;
//    mat4 viewMatrix;
//    mat4 normalMatrix;
//} matrices;


layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inCoord;

smooth out vec2 vTexCoord;
smooth out vec3 vNormal;
//out vec3 vWorldPos;
out vec4 vEyeSpacePos;

//uniform mat4 MV;
uniform mat4 MVP;
//uniform mat4 HeightmapScaleMatrix;

void main()
{
//    vec4 inPositionScaled = HeightmapScaleMatrix*vec4(inPosition, 1.0);
//    mat4 mMVP = matrices.projMatrix*matrices.viewMatrix*matrices.modelMatrix;
//    
    gl_Position = MVP * vec4(inPosition, 1.0);
//    vEyeSpacePos = matrices.viewMatrix*matrices.modelMatrix*vec4(inPosition, 1.0);
    vEyeSpacePos = vec4(inPosition, 1.0);
    
    vTexCoord = inCoord;
    vNormal = inNormal;
    
//    vec4 vWorldPosLocal = matrices.modelMatrix*inPositionScaled;
//    vWorldPos = vWorldPosLocal.xyz;
    
}