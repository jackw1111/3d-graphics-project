#ifndef _CULLING_H
#define _CULLING_H

#include "math_utils.h"
#include "animated_model.h"
/*! @brief helper class for culling objects behind the Camera */
class Frustum {
public:
    float fov;
    float nearDist;
    float farDist;
    float ar;
    vec3 Cnear;
    vec3 Cfar;
    vec3 topRightFar;
    vec3 topLeftFar;
    vec3 topRightNear;
    vec3 topLeftNear;
    vec3 bottomLeftNear;
    vec3 bottomLeftFar;
    vec3 bottomRightNear;
    vec3 bottomRightFar;
    vec3 rightNormal;
    vec3 leftNormal;
    vec3 topNormal;
    vec3 bottomNormal;
    vec3 backNormal;
    vec3 frontNormal;
    Frustum(){};
    Frustum(float _fov, float _nearDist, float _farDist, float _WIDTH, float _HEIGHT, Camera active_camera);
    void cull(StaticModel &statModel, const mat4 &m);
    void cullStaticObject(StaticObject &object);
    void cullAnimatedObject(AnimatedObject &object);
    void reset(AnimatedObject &object);

};

#endif