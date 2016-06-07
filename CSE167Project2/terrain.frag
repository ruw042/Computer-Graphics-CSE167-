//referenced: http://www.mbsoftworks.sk/index.php?page=tutorials&series=1&tutorial=24 for the shaders 

#version 330

smooth in vec2 vTexCoord;
smooth in vec3 vNormal;
//in vec3 vWorldPos;
in vec4 vEyeSpacePos;

uniform sampler2D gSampler;//[5];
uniform sampler2D secondSampler;//[5];
uniform sampler2D thirdSampler;//[5];
//uniform sampler2D shadowMap;

//uniform vec4 vColor;
//
//#include "dirLight.frag"
//uniform DirectionalLight sunLight;
//uniform float fRenderHeight;
//uniform float fMaxTextureU;
//uniform float fMaxTextureV;

out vec4 outputColor;

void main()
{
    vec3 vNormalized = normalize(vNormal);
    
    vec4 vTexColor = vec4(0.0);
    
//    float fScale = vWorldPos.y/fRenderHeight;
//    
//    const float fRange1 = 0.15f;
//    const float fRange2 = 0.3f;
//    const float fRange3 = 0.65f;
//    const float fRange4 = 0.85f;
    
//    if(fScale >= 0.0 && fScale <= fRange1)vTexColor = texture2D(gSampler[0], vTexCoord);
//    else if(fScale <= fRange2)
//    {
//        fScale -= fRange1;
//        fScale /= (fRange2-fRange1);
//        
//        float fScale2 = fScale;
//        fScale = 1.0-fScale;
//        
//        vTexColor += texture2D(gSampler[0], vTexCoord)*fScale;
//        vTexColor += texture2D(gSampler[1], vTexCoord)*fScale2;
//    }
//    else if(fScale <= fRange3)vTexColor = texture2D(gSampler[1], vTexCoord);
//    else if(fScale <= fRange4)
//    {
//        fScale -= fRange3;
//        fScale /= (fRange4-fRange3);
//        
//        float fScale2 = fScale;
//        fScale = 1.0-fScale;
//        
//        vTexColor += texture2D(gSampler[1], vTexCoord)*fScale;
//        vTexColor += texture2D(gSampler[2], vTexCoord)*fScale2;
//    }
//    else vTexColor = texture2D(gSampler[2], vTexCoord);
    
    if (vEyeSpacePos.y < -5)
        outputColor = texture(thirdSampler, vTexCoord);
    
    if (vEyeSpacePos.y < 10 && vEyeSpacePos.y >=-5)
        outputColor = texture(gSampler, vTexCoord);
    
    if (vEyeSpacePos.y < 12 && vEyeSpacePos.y >= 10)
    {
        float scale = (vEyeSpacePos.y - 12)/  12;
        float scaleInv = 1.0f - scale;
    
        vTexColor += texture(gSampler, vTexCoord)*scaleInv;
        vTexColor += texture(secondSampler, vTexCoord)*scale;
        outputColor = vTexColor;
    }
    if (vEyeSpacePos.y >= 12 )
    outputColor = texture(secondSampler, vTexCoord);
    
    
    
    
//    if (vEyeSpacePos.y < 2)
//        outputColor = texture(thirdSampler, vTexCoord);
//    else if (vEyeSpacePos.y < 4)
//    {
//        float scale = (vEyeSpacePos.y - 2)/  2;
//        float scaleInv = 1.0f - scale;
//
//        vTexColor += texture(gSampler, vTexCoord)*scaleInv;
//        vTexColor += texture(secondSampler, vTexCoord)*scale;
//        outputColor = vTexColor;
//    }
//    else
//        outputColor = texture(secondSampler, vTexCoord);
}

