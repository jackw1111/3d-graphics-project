#version 330                                                                    
                                                                                    
layout (location = 0) in vec3 Position;                                             
layout (location = 2) in vec2 TexCoord;                                             
layout (location = 1) in vec3 Normal;                                               
layout (location = 5) in ivec4 BoneIDs;
layout (location = 6) in vec4 Weights;

out vec2 TexCoord0;
out vec3 Normal0;                                                                   
out vec3 WorldPos0;                                                                 

const int MAX_BONES = 100;

uniform mat4 MVP;
uniform mat4 gWorld;
uniform mat4 gView;
uniform mat4 gProjection;
uniform mat4 gBones[MAX_BONES];

invariant gl_Position;

void main()
{       
    mat4 BoneTransform = gBones[BoneIDs[0]] * Weights[0];
    BoneTransform     += gBones[BoneIDs[1]] * Weights[1];
    BoneTransform     += gBones[BoneIDs[2]] * Weights[2];
    BoneTransform     += gBones[BoneIDs[3]] * Weights[3];

    vec4 PosL    = BoneTransform * vec4(Position, 1.0);
    gl_Position  = MVP * PosL;
    TexCoord0    = TexCoord;
    vec4 NormalL = BoneTransform * vec4(Normal, 0.0);
    Normal0      = (gWorld * NormalL).xyz;
    WorldPos0    = (gWorld * PosL).xyz;                                
}
