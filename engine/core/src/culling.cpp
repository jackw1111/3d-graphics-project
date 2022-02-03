#include "culling.h"

Frustum::Frustum(float _fov, float _nearDist, float _farDist, float _WIDTH, float _HEIGHT, Camera camera) {
    fov = _fov;
    nearDist = _nearDist;
    farDist = _farDist;
    ar = (float)_WIDTH / (float)_HEIGHT;


    // float fov = 45.0f;
    // float nearDist = 0.1f;
    // float farDist = 1000.0f;
    // float ar = (float)WIDTH / (float)HEIGHT;

    float Hnear;
    float Wnear;
    float Hfar;
    float Wfar;

    // ortho
    if (fov == 0.0f) {
        Wnear = _WIDTH;
        Hnear = _HEIGHT;
        Hfar = _HEIGHT;
        Wfar = _WIDTH;

    // perspective
    } else {
        Hnear = 2* tan(fov/2) * nearDist;
        Wnear = Hnear * ar;
        Hfar = 2* tan(fov/2) * farDist;
        Wfar = Hfar * ar; 
    }

    // calculate frustum data per frame
    Cnear = camera.Position + glm::normalize(camera.Front) * nearDist;
    Cfar = camera.Position + glm::normalize(camera.Front) * farDist;

    topRightFar = Cfar + (camera.Up * (Hfar / 2)) + (camera.Right * (Wfar / 2));
    bottomRightFar = Cfar - (camera.Up * (Hfar / 2)) + (camera.Right * (Wfar / 2));

    topLeftFar =  Cfar + (camera.Up * (Hfar / 2)) - (camera.Right * (Wfar / 2));
    bottomLeftFar =  Cfar - (camera.Up * (Hfar / 2)) - (camera.Right * (Wfar / 2));

    topRightNear = Cnear + (camera.Up * (Hnear / 2)) + (camera.Right * (Wnear / 2));
    topLeftNear =  Cnear + (camera.Up * (Hnear / 2)) - (camera.Right * (Wnear / 2));

    bottomLeftNear = Cnear - (camera.Up * (Hnear /2)) - (camera.Right * (Wnear / 2));
    bottomRightNear = Cnear - (camera.Up * (Hnear /2)) + (camera.Right * (Wnear / 2));

    vec3 aux = glm::normalize((Cnear + camera.Right * (float)(Wnear / 2)) - camera.Position);
    rightNormal = glm::normalize(glm::cross(aux, camera.Up));
    aux = glm::normalize((Cnear - camera.Right * (float)(Wnear / 2)) - camera.Position);
    leftNormal = glm::normalize(glm::cross(aux, camera.Up));

    aux = glm::normalize((Cnear + camera.Up * (float)(Hnear / 2)) - camera.Position);
    topNormal =  glm::normalize(glm::cross(aux, camera.Right));

    aux = glm::normalize((Cnear - camera.Up * (float)(Hnear / 2)) - camera.Position);
    bottomNormal =  glm::normalize(glm::cross(aux, camera.Right));

    backNormal = camera.Front;
    frontNormal = -1.0f * camera.Front;
}

void Frustum::cull(StaticModel &statModel, const mat4 &model) {

    for (unsigned int j = 0; j < statModel.meshes.size(); j++) {
        StaticMesh *mesh = &statModel.meshes[j];
        mesh->frustumCulled = false;                

        vector<float> aabb = mesh->getAABB();
        vec3 point1 = vec3(model * vec4(aabb[0], aabb[1], aabb[2], 1.0));
        vec3 point2 = vec3(model * vec4(aabb[3], aabb[4], aabb[5], 1.0));
        vec3 point3 = vec3(point1.x, point2.y, point1.z);
        vec3 point4 = vec3(point2.x, point1.y, point1.z);
        vec3 point5 = vec3(point2.x, point2.y, point1.z);
        vec3 point6 = vec3(point2.x, point1.y, point2.z);
        vec3 point7 = vec3(point1.x, point2.y, point2.z);
        vec3 point8 = vec3(point1.x, point1.y, point2.z);

        // check if box is outside the left plane
        float outsideFrustum1 = glm::dot(leftNormal, glm::normalize(point1 - topLeftNear));
        float outsideFrustum2 = glm::dot(leftNormal, glm::normalize(point2 - topLeftNear));
        float outsideFrustum3 = glm::dot(leftNormal, glm::normalize(point3 - topLeftNear));
        float outsideFrustum4 = glm::dot(leftNormal, glm::normalize(point4 - topLeftNear));
        float outsideFrustum5 = glm::dot(leftNormal, glm::normalize(point5 - topLeftNear));
        float outsideFrustum6 = glm::dot(leftNormal, glm::normalize(point6 - topLeftNear));
        float outsideFrustum7 = glm::dot(leftNormal, glm::normalize(point7 - topLeftNear));
        float outsideFrustum8 = glm::dot(leftNormal, glm::normalize(point8 - topLeftNear));

        if (outsideFrustum1 < 0 && outsideFrustum2 < 0 && outsideFrustum3 < 0 && outsideFrustum4  < 0  && 
            outsideFrustum5 < 0 && outsideFrustum6 < 0  && outsideFrustum7  < 0  && outsideFrustum8  < 0 ) {
            mesh->frustumCulled = true;
            continue;
        }

        // check if box is outside the right plane
        outsideFrustum1 = glm::dot(rightNormal, glm::normalize(point1 - topRightNear));
        outsideFrustum2 = glm::dot(rightNormal, glm::normalize(point2 - topRightNear));
        outsideFrustum3 = glm::dot(rightNormal, glm::normalize(point3 - topRightNear));
        outsideFrustum4 = glm::dot(rightNormal, glm::normalize(point4 - topRightNear));
        outsideFrustum5 = glm::dot(rightNormal, glm::normalize(point5 - topRightNear));
        outsideFrustum6 = glm::dot(rightNormal, glm::normalize(point6 - topRightNear));
        outsideFrustum7 = glm::dot(rightNormal, glm::normalize(point7 - topRightNear));
        outsideFrustum8 = glm::dot(rightNormal, glm::normalize(point8 - topRightNear));

        if (outsideFrustum1 > 0 && outsideFrustum2 > 0 && outsideFrustum3 > 0 && outsideFrustum4  > 0  && 
            outsideFrustum5 > 0 && outsideFrustum6 > 0  && outsideFrustum7  > 0  && outsideFrustum8  > 0 ) {
            mesh->frustumCulled = true;
            continue;
        }

        // check if box is outside the bottom plane
        outsideFrustum1 = glm::dot(bottomNormal, glm::normalize(point1 - bottomLeftNear));
        outsideFrustum2 = glm::dot(bottomNormal, glm::normalize(point2 - bottomLeftNear));
        outsideFrustum3 = glm::dot(bottomNormal, glm::normalize(point3 - bottomLeftNear));
        outsideFrustum4 = glm::dot(bottomNormal, glm::normalize(point4 - bottomLeftNear));
        outsideFrustum5 = glm::dot(bottomNormal, glm::normalize(point5 - bottomLeftNear));
        outsideFrustum6 = glm::dot(bottomNormal, glm::normalize(point6 - bottomLeftNear));
        outsideFrustum7 = glm::dot(bottomNormal, glm::normalize(point7 - bottomLeftNear));
        outsideFrustum8 = glm::dot(bottomNormal, glm::normalize(point8 - bottomLeftNear));

        if (outsideFrustum1 > 0 && outsideFrustum2 > 0 && outsideFrustum3 > 0 && outsideFrustum4  > 0  && 
            outsideFrustum5 > 0 && outsideFrustum6 > 0  && outsideFrustum7  > 0  && outsideFrustum8  > 0 ) {
            mesh->frustumCulled = true;
            continue;
        }

        // check if box is outside the top plane
        outsideFrustum1 = glm::dot(topNormal, glm::normalize(point1 - topLeftNear));
        outsideFrustum2 = glm::dot(topNormal, glm::normalize(point2 - topLeftNear));
        outsideFrustum3 = glm::dot(topNormal, glm::normalize(point3 - topLeftNear));
        outsideFrustum4 = glm::dot(topNormal, glm::normalize(point4 - topLeftNear));
        outsideFrustum5 = glm::dot(topNormal, glm::normalize(point5 - topLeftNear));
        outsideFrustum6 = glm::dot(topNormal, glm::normalize(point6 - topLeftNear));
        outsideFrustum7 = glm::dot(topNormal, glm::normalize(point7 - topLeftNear));
        outsideFrustum8 = glm::dot(topNormal, glm::normalize(point8 - topLeftNear));

        if (outsideFrustum1 < 0 && outsideFrustum2 < 0 && outsideFrustum3 < 0 && outsideFrustum4  < 0  && 
            outsideFrustum5 < 0 && outsideFrustum6 < 0  && outsideFrustum7  < 0  && outsideFrustum8  < 0 ) {
            mesh->frustumCulled = true;
            continue;
        }

        // check if box is outside the back plane

        outsideFrustum1 = glm::dot(backNormal, glm::normalize(point1 - topLeftFar));
        outsideFrustum2 = glm::dot(backNormal, glm::normalize(point2 - topLeftFar));
        outsideFrustum3 = glm::dot(backNormal, glm::normalize(point3 - topLeftFar));
        outsideFrustum4 = glm::dot(backNormal, glm::normalize(point4 - topLeftFar));
        outsideFrustum5 = glm::dot(backNormal, glm::normalize(point5 - topLeftFar));
        outsideFrustum6 = glm::dot(backNormal, glm::normalize(point6 - topLeftFar));
        outsideFrustum7 = glm::dot(backNormal, glm::normalize(point7 - topLeftFar));
        outsideFrustum8 = glm::dot(backNormal, glm::normalize(point8 - topLeftFar));

        if (outsideFrustum1 > 0 && outsideFrustum2 > 0 && outsideFrustum3 > 0 && outsideFrustum4  > 0  && 
            outsideFrustum5 > 0 && outsideFrustum6 > 0  && outsideFrustum7  > 0  && outsideFrustum8  > 0 ) {
            mesh->frustumCulled = true;
            continue;
        }

        // check if box is outside the front plane

        outsideFrustum1 = glm::dot(frontNormal, glm::normalize(point1 - topLeftNear));
        outsideFrustum2 = glm::dot(frontNormal, glm::normalize(point2 - topLeftNear));
        outsideFrustum3 = glm::dot(frontNormal, glm::normalize(point3 - topLeftNear));
        outsideFrustum4 = glm::dot(frontNormal, glm::normalize(point4 - topLeftNear));
        outsideFrustum5 = glm::dot(frontNormal, glm::normalize(point5 - topLeftNear));
        outsideFrustum6 = glm::dot(frontNormal, glm::normalize(point6 - topLeftNear));
        outsideFrustum7 = glm::dot(frontNormal, glm::normalize(point7 - topLeftNear));
        outsideFrustum8 = glm::dot(frontNormal, glm::normalize(point8 - topLeftNear));

        if (outsideFrustum1 > 0 && outsideFrustum2 > 0 && outsideFrustum3 > 0 && outsideFrustum4  > 0  && 
            outsideFrustum5 > 0 && outsideFrustum6 > 0  && outsideFrustum7  > 0  && outsideFrustum8  > 0 ) {
            mesh->frustumCulled = true;
            continue;
        }
    }
}
// @todo implement polymorphic behaviour to cull static and animated objects
void Frustum::cullStaticObject(StaticObject &object) {
	StaticModel *statModel = object.model.get();

	mat4 model = object.modelMatrix;

    for (unsigned int j = 0; j < statModel->meshes.size(); j++) {

        StaticMesh *mesh = &statModel->meshes[j];

        object.meshIsCulled[j] = 0;

        vector<float> aabb = mesh->getAABB();
        vec3 point1 = vec3(model * vec4(aabb[0], aabb[1], aabb[2], 1.0));
        vec3 point2 = vec3(model * vec4(aabb[3], aabb[4], aabb[5], 1.0));
        vec3 point3 = vec3(point1.x, point2.y, point1.z);
        vec3 point4 = vec3(point2.x, point1.y, point1.z);
        vec3 point5 = vec3(point2.x, point2.y, point1.z);
        vec3 point6 = vec3(point2.x, point1.y, point2.z);
        vec3 point7 = vec3(point1.x, point2.y, point2.z);
        vec3 point8 = vec3(point1.x, point1.y, point2.z);

        // check if box is outside the left plane
        float outsideFrustum1 = glm::dot(leftNormal, glm::normalize(point1 - topLeftNear));
        float outsideFrustum2 = glm::dot(leftNormal, glm::normalize(point2 - topLeftNear));
        float outsideFrustum3 = glm::dot(leftNormal, glm::normalize(point3 - topLeftNear));
        float outsideFrustum4 = glm::dot(leftNormal, glm::normalize(point4 - topLeftNear));
        float outsideFrustum5 = glm::dot(leftNormal, glm::normalize(point5 - topLeftNear));
        float outsideFrustum6 = glm::dot(leftNormal, glm::normalize(point6 - topLeftNear));
        float outsideFrustum7 = glm::dot(leftNormal, glm::normalize(point7 - topLeftNear));
        float outsideFrustum8 = glm::dot(leftNormal, glm::normalize(point8 - topLeftNear));

        if (outsideFrustum1 < 0 && outsideFrustum2 < 0 && outsideFrustum3 < 0 && outsideFrustum4  < 0  && 
            outsideFrustum5 < 0 && outsideFrustum6 < 0  && outsideFrustum7  < 0  && outsideFrustum8  < 0 ) {
            object.meshIsCulled[j] = 1;
            continue;
        }

        // check if box is outside the right plane
        outsideFrustum1 = glm::dot(rightNormal, glm::normalize(point1 - topRightNear));
        outsideFrustum2 = glm::dot(rightNormal, glm::normalize(point2 - topRightNear));
        outsideFrustum3 = glm::dot(rightNormal, glm::normalize(point3 - topRightNear));
        outsideFrustum4 = glm::dot(rightNormal, glm::normalize(point4 - topRightNear));
        outsideFrustum5 = glm::dot(rightNormal, glm::normalize(point5 - topRightNear));
        outsideFrustum6 = glm::dot(rightNormal, glm::normalize(point6 - topRightNear));
        outsideFrustum7 = glm::dot(rightNormal, glm::normalize(point7 - topRightNear));
        outsideFrustum8 = glm::dot(rightNormal, glm::normalize(point8 - topRightNear));

        if (outsideFrustum1 > 0 && outsideFrustum2 > 0 && outsideFrustum3 > 0 && outsideFrustum4  > 0  && 
            outsideFrustum5 > 0 && outsideFrustum6 > 0  && outsideFrustum7  > 0  && outsideFrustum8  > 0 ) {
            object.meshIsCulled[j] = 1;
            continue;
        }

        // check if box is outside the bottom plane
        outsideFrustum1 = glm::dot(bottomNormal, glm::normalize(point1 - bottomLeftNear));
        outsideFrustum2 = glm::dot(bottomNormal, glm::normalize(point2 - bottomLeftNear));
        outsideFrustum3 = glm::dot(bottomNormal, glm::normalize(point3 - bottomLeftNear));
        outsideFrustum4 = glm::dot(bottomNormal, glm::normalize(point4 - bottomLeftNear));
        outsideFrustum5 = glm::dot(bottomNormal, glm::normalize(point5 - bottomLeftNear));
        outsideFrustum6 = glm::dot(bottomNormal, glm::normalize(point6 - bottomLeftNear));
        outsideFrustum7 = glm::dot(bottomNormal, glm::normalize(point7 - bottomLeftNear));
        outsideFrustum8 = glm::dot(bottomNormal, glm::normalize(point8 - bottomLeftNear));

        if (outsideFrustum1 > 0 && outsideFrustum2 > 0 && outsideFrustum3 > 0 && outsideFrustum4  > 0  && 
            outsideFrustum5 > 0 && outsideFrustum6 > 0  && outsideFrustum7  > 0  && outsideFrustum8  > 0 ) {
            object.meshIsCulled[j] = 1;
            continue;
        }

        // check if box is outside the top plane
        outsideFrustum1 = glm::dot(topNormal, glm::normalize(point1 - topLeftNear));
        outsideFrustum2 = glm::dot(topNormal, glm::normalize(point2 - topLeftNear));
        outsideFrustum3 = glm::dot(topNormal, glm::normalize(point3 - topLeftNear));
        outsideFrustum4 = glm::dot(topNormal, glm::normalize(point4 - topLeftNear));
        outsideFrustum5 = glm::dot(topNormal, glm::normalize(point5 - topLeftNear));
        outsideFrustum6 = glm::dot(topNormal, glm::normalize(point6 - topLeftNear));
        outsideFrustum7 = glm::dot(topNormal, glm::normalize(point7 - topLeftNear));
        outsideFrustum8 = glm::dot(topNormal, glm::normalize(point8 - topLeftNear));

        if (outsideFrustum1 < 0 && outsideFrustum2 < 0 && outsideFrustum3 < 0 && outsideFrustum4  < 0  && 
            outsideFrustum5 < 0 && outsideFrustum6 < 0  && outsideFrustum7  < 0  && outsideFrustum8  < 0 ) {
            object.meshIsCulled[j]= 1;
            continue;
        }

        // check if box is outside the back plane

        outsideFrustum1 = glm::dot(backNormal, glm::normalize(point1 - topLeftFar));
        outsideFrustum2 = glm::dot(backNormal, glm::normalize(point2 - topLeftFar));
        outsideFrustum3 = glm::dot(backNormal, glm::normalize(point3 - topLeftFar));
        outsideFrustum4 = glm::dot(backNormal, glm::normalize(point4 - topLeftFar));
        outsideFrustum5 = glm::dot(backNormal, glm::normalize(point5 - topLeftFar));
        outsideFrustum6 = glm::dot(backNormal, glm::normalize(point6 - topLeftFar));
        outsideFrustum7 = glm::dot(backNormal, glm::normalize(point7 - topLeftFar));
        outsideFrustum8 = glm::dot(backNormal, glm::normalize(point8 - topLeftFar));

        if (outsideFrustum1 > 0 && outsideFrustum2 > 0 && outsideFrustum3 > 0 && outsideFrustum4  > 0  && 
            outsideFrustum5 > 0 && outsideFrustum6 > 0  && outsideFrustum7  > 0  && outsideFrustum8  > 0 ) {
            object.meshIsCulled[j] = 1;
            continue;
        }

        // check if box is outside the front plane

        outsideFrustum1 = glm::dot(frontNormal, glm::normalize(point1 - topLeftNear));
        outsideFrustum2 = glm::dot(frontNormal, glm::normalize(point2 - topLeftNear));
        outsideFrustum3 = glm::dot(frontNormal, glm::normalize(point3 - topLeftNear));
        outsideFrustum4 = glm::dot(frontNormal, glm::normalize(point4 - topLeftNear));
        outsideFrustum5 = glm::dot(frontNormal, glm::normalize(point5 - topLeftNear));
        outsideFrustum6 = glm::dot(frontNormal, glm::normalize(point6 - topLeftNear));
        outsideFrustum7 = glm::dot(frontNormal, glm::normalize(point7 - topLeftNear));
        outsideFrustum8 = glm::dot(frontNormal, glm::normalize(point8 - topLeftNear));

        if (outsideFrustum1 > 0 && outsideFrustum2 > 0 && outsideFrustum3 > 0 && outsideFrustum4  > 0  && 
            outsideFrustum5 > 0 && outsideFrustum6 > 0  && outsideFrustum7  > 0  && outsideFrustum8  > 0 ) {
            object.meshIsCulled[j] = 1;
            continue;
        }
    }
}

// @todo implement polymorphic behaviour to cull static and animated objects
void Frustum::cullAnimatedObject(AnimatedObject &object) {
    AnimatedModel *animModel = object.model.get();

    mat4 model = object.modelMatrix;

    for (unsigned int j = 0; j < animModel->meshes.size(); j++) {

        StaticMesh *mesh = &animModel->meshes[j];

        object.meshIsCulled[j] = 0;

        vector<float> aabb = mesh->getAABB();
        vec3 point1 = vec3(model * vec4(aabb[0], aabb[1], aabb[2], 1.0));
        vec3 point2 = vec3(model * vec4(aabb[3], aabb[4], aabb[5], 1.0));
        vec3 point3 = vec3(point1.x, point2.y, point1.z);
        vec3 point4 = vec3(point2.x, point1.y, point1.z);
        vec3 point5 = vec3(point2.x, point2.y, point1.z);
        vec3 point6 = vec3(point2.x, point1.y, point2.z);
        vec3 point7 = vec3(point1.x, point2.y, point2.z);
        vec3 point8 = vec3(point1.x, point1.y, point2.z);

        // check if box is outside the left plane
        float outsideFrustum1 = glm::dot(leftNormal, glm::normalize(point1 - topLeftNear));
        float outsideFrustum2 = glm::dot(leftNormal, glm::normalize(point2 - topLeftNear));
        float outsideFrustum3 = glm::dot(leftNormal, glm::normalize(point3 - topLeftNear));
        float outsideFrustum4 = glm::dot(leftNormal, glm::normalize(point4 - topLeftNear));
        float outsideFrustum5 = glm::dot(leftNormal, glm::normalize(point5 - topLeftNear));
        float outsideFrustum6 = glm::dot(leftNormal, glm::normalize(point6 - topLeftNear));
        float outsideFrustum7 = glm::dot(leftNormal, glm::normalize(point7 - topLeftNear));
        float outsideFrustum8 = glm::dot(leftNormal, glm::normalize(point8 - topLeftNear));

        if (outsideFrustum1 < 0 && outsideFrustum2 < 0 && outsideFrustum3 < 0 && outsideFrustum4  < 0  && 
            outsideFrustum5 < 0 && outsideFrustum6 < 0  && outsideFrustum7  < 0  && outsideFrustum8  < 0 ) {
            object.meshIsCulled[j] = 1;
            continue;
        }

        // check if box is outside the right plane
        outsideFrustum1 = glm::dot(rightNormal, glm::normalize(point1 - topRightNear));
        outsideFrustum2 = glm::dot(rightNormal, glm::normalize(point2 - topRightNear));
        outsideFrustum3 = glm::dot(rightNormal, glm::normalize(point3 - topRightNear));
        outsideFrustum4 = glm::dot(rightNormal, glm::normalize(point4 - topRightNear));
        outsideFrustum5 = glm::dot(rightNormal, glm::normalize(point5 - topRightNear));
        outsideFrustum6 = glm::dot(rightNormal, glm::normalize(point6 - topRightNear));
        outsideFrustum7 = glm::dot(rightNormal, glm::normalize(point7 - topRightNear));
        outsideFrustum8 = glm::dot(rightNormal, glm::normalize(point8 - topRightNear));

        if (outsideFrustum1 > 0 && outsideFrustum2 > 0 && outsideFrustum3 > 0 && outsideFrustum4  > 0  && 
            outsideFrustum5 > 0 && outsideFrustum6 > 0  && outsideFrustum7  > 0  && outsideFrustum8  > 0 ) {
            object.meshIsCulled[j] = 1;
            continue;
        }

        // check if box is outside the bottom plane
        outsideFrustum1 = glm::dot(bottomNormal, glm::normalize(point1 - bottomLeftNear));
        outsideFrustum2 = glm::dot(bottomNormal, glm::normalize(point2 - bottomLeftNear));
        outsideFrustum3 = glm::dot(bottomNormal, glm::normalize(point3 - bottomLeftNear));
        outsideFrustum4 = glm::dot(bottomNormal, glm::normalize(point4 - bottomLeftNear));
        outsideFrustum5 = glm::dot(bottomNormal, glm::normalize(point5 - bottomLeftNear));
        outsideFrustum6 = glm::dot(bottomNormal, glm::normalize(point6 - bottomLeftNear));
        outsideFrustum7 = glm::dot(bottomNormal, glm::normalize(point7 - bottomLeftNear));
        outsideFrustum8 = glm::dot(bottomNormal, glm::normalize(point8 - bottomLeftNear));

        if (outsideFrustum1 > 0 && outsideFrustum2 > 0 && outsideFrustum3 > 0 && outsideFrustum4  > 0  && 
            outsideFrustum5 > 0 && outsideFrustum6 > 0  && outsideFrustum7  > 0  && outsideFrustum8  > 0 ) {
            object.meshIsCulled[j] = 1;
            continue;
        }

        // check if box is outside the top plane
        outsideFrustum1 = glm::dot(topNormal, glm::normalize(point1 - topLeftNear));
        outsideFrustum2 = glm::dot(topNormal, glm::normalize(point2 - topLeftNear));
        outsideFrustum3 = glm::dot(topNormal, glm::normalize(point3 - topLeftNear));
        outsideFrustum4 = glm::dot(topNormal, glm::normalize(point4 - topLeftNear));
        outsideFrustum5 = glm::dot(topNormal, glm::normalize(point5 - topLeftNear));
        outsideFrustum6 = glm::dot(topNormal, glm::normalize(point6 - topLeftNear));
        outsideFrustum7 = glm::dot(topNormal, glm::normalize(point7 - topLeftNear));
        outsideFrustum8 = glm::dot(topNormal, glm::normalize(point8 - topLeftNear));

        if (outsideFrustum1 < 0 && outsideFrustum2 < 0 && outsideFrustum3 < 0 && outsideFrustum4  < 0  && 
            outsideFrustum5 < 0 && outsideFrustum6 < 0  && outsideFrustum7  < 0  && outsideFrustum8  < 0 ) {
            object.meshIsCulled[j]= 1;
            continue;
        }

        // check if box is outside the back plane

        outsideFrustum1 = glm::dot(backNormal, glm::normalize(point1 - topLeftFar));
        outsideFrustum2 = glm::dot(backNormal, glm::normalize(point2 - topLeftFar));
        outsideFrustum3 = glm::dot(backNormal, glm::normalize(point3 - topLeftFar));
        outsideFrustum4 = glm::dot(backNormal, glm::normalize(point4 - topLeftFar));
        outsideFrustum5 = glm::dot(backNormal, glm::normalize(point5 - topLeftFar));
        outsideFrustum6 = glm::dot(backNormal, glm::normalize(point6 - topLeftFar));
        outsideFrustum7 = glm::dot(backNormal, glm::normalize(point7 - topLeftFar));
        outsideFrustum8 = glm::dot(backNormal, glm::normalize(point8 - topLeftFar));

        if (outsideFrustum1 > 0 && outsideFrustum2 > 0 && outsideFrustum3 > 0 && outsideFrustum4  > 0  && 
            outsideFrustum5 > 0 && outsideFrustum6 > 0  && outsideFrustum7  > 0  && outsideFrustum8  > 0 ) {
            object.meshIsCulled[j] = 1;
            continue;
        }

        // check if box is outside the front plane

        outsideFrustum1 = glm::dot(frontNormal, glm::normalize(point1 - topLeftNear));
        outsideFrustum2 = glm::dot(frontNormal, glm::normalize(point2 - topLeftNear));
        outsideFrustum3 = glm::dot(frontNormal, glm::normalize(point3 - topLeftNear));
        outsideFrustum4 = glm::dot(frontNormal, glm::normalize(point4 - topLeftNear));
        outsideFrustum5 = glm::dot(frontNormal, glm::normalize(point5 - topLeftNear));
        outsideFrustum6 = glm::dot(frontNormal, glm::normalize(point6 - topLeftNear));
        outsideFrustum7 = glm::dot(frontNormal, glm::normalize(point7 - topLeftNear));
        outsideFrustum8 = glm::dot(frontNormal, glm::normalize(point8 - topLeftNear));

        if (outsideFrustum1 > 0 && outsideFrustum2 > 0 && outsideFrustum3 > 0 && outsideFrustum4  > 0  && 
            outsideFrustum5 > 0 && outsideFrustum6 > 0  && outsideFrustum7  > 0  && outsideFrustum8  > 0 ) {
            object.meshIsCulled[j] = 1;
            continue;
        }
    }
}

void Frustum::reset(AnimatedObject &object) {
    AnimatedModel *animModel = object.model.get();

    for (unsigned int j = 0; j < animModel->meshes.size(); j++) {

        object.meshIsCulled[j] = 0;
    }
}