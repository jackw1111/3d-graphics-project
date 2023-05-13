#include "animated_model.h"

static inline glm::mat4 mat4_cast(const aiMatrix4x4 &m) { return glm::transpose(glm::make_mat4(&m.a1)); }
static inline glm::mat4 mat4_cast(const aiMatrix3x3 &m) { return glm::transpose(glm::make_mat3(&m.a1)); }


// a collection of objects all of the same filePath
vector<vector<AnimatedObject>> AnimatedObject::modelRegistry = {};
unsigned int AnimatedObject::uniqueObjectCount = 0;
AnimatedShader AnimatedModel::shader = AnimatedShader();

// start and end are in seconds
float playFrames(float start, float end, float current_frame) {
    if (start == 0.0f && end == 0.0f) {
        return current_frame;
    }
    return fmod(current_frame, end - start) + start;
}

AnimatedObject::AnimatedObject(const std::string &filePath) {

    // check if a registry for the filePath has been created
    bool found = false;
    bool newBatch = false;
    shared_ptr<AnimatedModel> modelReference;

    for (unsigned int i = 0; i < AnimatedObject::modelRegistry.size(); i++) {
        // get a reference to the first object of each unique filePath
        AnimatedObject* storedObject = &AnimatedObject::modelRegistry[i][0];
        // check if filePaths match
        if (storedObject->filePath == filePath && AnimatedObject::modelRegistry[i].size() < 20) {

            // reference underlying StaticModel data
            this->filePath = storedObject->filePath;
            this->model = storedObject->model;
            this->modelID = storedObject->modelID;
            this->modelMatrix = mat4(1.0);
            this->boneTransforms = {};
            this->toDraw = storedObject->toDraw;
            this->meshIsCulled = storedObject->meshIsCulled;
            this->uniqueID = AnimatedObject::modelRegistry[this->modelID].size();
            //this->color = vec4(-1,-1,-1,-1);
            // add it to the store of objects
            AnimatedObject::modelRegistry[this->modelID].push_back(*this);
            found = true;
            break;
        } else if (storedObject->filePath == filePath && AnimatedObject::modelRegistry[i].size() >= 20) {
            newBatch = true;
            modelReference = storedObject->model;
            break;
        }
    }
    if (!found || newBatch) {
        // object is unique, create an object store in the registry
        this->filePath = filePath;
        if (newBatch) {
            this->model = modelReference;
        } else {
            this->model = std::shared_ptr<AnimatedModel>(new AnimatedModel(filePath));
        }
        this->modelID = AnimatedObject::uniqueObjectCount++;
        this->modelMatrix = mat4(1.0);
        this->boneTransforms = {};
        this->meshIsCulled.resize(this->model.get()->meshes.size());
        this->uniqueID = 0;
        //this->color = vec4(-1,-1,-1,-1);
        this->setAABB();
        AnimatedObject::modelRegistry.push_back({*this});
    }

}

shared_ptr<AnimatedModel> AnimatedObject::getModel() {
    return AnimatedObject::modelRegistry.at(this->modelID).at(this->uniqueID).model;
}

mat4 AnimatedObject::getModelMatrix() {
    return AnimatedObject::modelRegistry.at(this->modelID).at(this->uniqueID).modelMatrix;
}

int AnimatedObject::setModelMatrix(mat4 m) {
    AnimatedObject::modelRegistry.at(this->modelID).at(this->uniqueID).modelMatrix = m;
    return 0;
}


bool AnimatedObject::getDrawBoundingBox() {
    return AnimatedObject::modelRegistry.at(this->modelID).at(this->uniqueID).drawBoundingBox;
}

int AnimatedObject::setDrawBoundingBox(bool value) {
    AnimatedObject::modelRegistry.at(this->modelID).at(this->uniqueID).drawBoundingBox = value;
    return 0;
}

vector<mat4> AnimatedObject::getBoneTransforms() {
    return AnimatedObject::modelRegistry.at(this->modelID).at(this->uniqueID).boneTransforms;
}

vec4 AnimatedObject::getColor() {
    return AnimatedObject::modelRegistry.at(this->modelID).at(this->uniqueID).color;
}

int AnimatedObject::setColor(vec4 c) {
    AnimatedObject::modelRegistry.at(this->modelID).at(this->uniqueID).color = c;
    return 0;
}

vector<mat4> AnimatedObject::getObjectTransforms(vector<AnimatedObject> &currentObjects, bool shadowPass) {
    vector<mat4> modelTransforms = {};
    for (unsigned int i = 0; i < currentObjects.size(); i++) {
        if (!shadowPass) {
            if (currentObjects[i].getToDraw()) {
                modelTransforms.push_back(currentObjects[i].getModelMatrix());
            }
        } else {
            if (currentObjects[i].getToDrawShadow()) {
                modelTransforms.push_back(currentObjects[i].getModelMatrix());
            }
        }
    }
    return modelTransforms;
}
vector<unsigned int> AnimatedObject::getObjectIndices(vector<AnimatedObject> &currentObjects, bool shadowPass) {
    vector<unsigned int> modelIndices = {};
    for (unsigned int i = 0; i < currentObjects.size(); i++) {

        if (!shadowPass) {
            if (currentObjects[i].getToDraw()) {
                modelIndices.push_back(i);
            }
        } else {
            if (currentObjects[i].getToDrawShadow()) {
                modelIndices.push_back(i);
            }
        }
    }
    return modelIndices;
}

vector<vector<vec3>> AnimatedObject::getAnimatedVertices() {
    AnimatedObject *object = &AnimatedObject::modelRegistry.at(this->modelID).at(this->uniqueID);
    object->animatedVertices = {};

    for (unsigned int i = 0; i < object->model.get()->meshes.size(); i++) {
        AnimatedMesh *currentMesh = &(object->model.get()->meshes.at(i));
        currentMesh->setVertices(this->boneTransforms);
        object->animatedVertices.push_back(currentMesh->animatedVertices);
    }
    return object->animatedVertices;
}


int AnimatedObject::drawAllObjects(Camera &active_camera, float currentFrame, bool shadowPass) {
    
    bool recalculateBones = shadowPass;

    unsigned int totalMeshes = 0;
    unsigned int drawnMeshes = 0;

    // for every model in registry
    for (unsigned int index = 0; index <  AnimatedObject::modelRegistry.size(); index++) {
        vector<AnimatedObject> *currentObjects = &AnimatedObject::modelRegistry[index];
        // get the model matrices for current objects
        vector<mat4> modelTransforms = AnimatedObject::getObjectTransforms(*currentObjects, shadowPass);
        vector<unsigned int> objectIndices = AnimatedObject::getObjectIndices(*currentObjects, shadowPass);

        // get a reference to the underlying model data
        AnimatedModel *pModel = (*currentObjects)[0].model.get();

        // for every mesh
        for (unsigned int i = 0; i < pModel->meshes.size(); i++) {
            // reset transforms
            vector<mat4> visibleModelTransforms = {};
            vector<mat4> coloredVisibleModelTransforms = {};
            vector<vec4> colors = {};
            //for each transform
            unsigned int id = 0;
            for (unsigned int j = 0; j < objectIndices.size(); j++) {  
                // get the current object
                AnimatedObject *object = &(*currentObjects).at(objectIndices.at(j));
                AnimatedMesh *currentMesh = &(object->model.get()->meshes.at(i));

                vector<float> AABB = currentMesh->getAABB();
                glm::vec3 min = glm::vec3(AABB[0], AABB[1], AABB[2]);
                glm::vec3 max = glm::vec3(AABB[3], AABB[4], AABB[5]);
                currentMesh->boundingBox.setBoundary(min, max);
                // since recalculateBones is also the variable shadowPass,
                // ignore drawing to shadow map if drawShadow == false
                if (shadowPass && !object->getToDrawShadow()) {
                    continue;
                }

                if (object->drawBoundingBox) {

                    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
                    glDisable(GL_CULL_FACE);
                    currentMesh->boundingBox.modelMatrix = object->getModelMatrix();
                    currentMesh->boundingBox.setMatrix("model", object->getModelMatrix());
                    currentMesh->boundingBox.setMatrix("view", active_camera.view_matrix);
                    currentMesh->boundingBox.setMatrix("projection", active_camera.projection_matrix);

                    currentMesh->boundingBox.draw();
                    AnimatedModel::shader.use();
                    //glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
                    glEnable(GL_CULL_FACE);
                }
                        
                if (!object->useCustomShader) {
                    // check if object mesh is either not culled or not occluded
                    if (!object->meshIsCulled[i]) {
                        //std::cout << "drawing AABB" << std::endl;


                        if (recalculateBones) {
                            object->boneTransforms = {};
                        }


                        pModel->getFrame(id, playFrames(object->start_frame, object->end_frame, object->offset), object->boneTransforms);
                        // create animated vertices for the frame
                        // @todo speed up with octree, raycast
                        // vector<mat4> _gBones = object->boneTransforms;
                        // object->animatedVertices = {};
                        // for (unsigned int j  = 0; j < object->model.get()->meshes.size(); j++) {
                        //     AnimatedMesh *currentMesh = &(object->model.get()->meshes.at(j));
                        //     currentMesh->setVertices(_gBones);
                        //     object->animatedVertices.push_back(currentMesh->animatedVertices);
                        // }
                        visibleModelTransforms.push_back(modelTransforms[j]);
                        id++;
                        ///@note indexing the bone matrices with j wasn't working and was only
                        /// sparsely filling the bone matrix vector to send to the shader
                        /// packing the index tightly (with id) fixes the alignment of
                        /// bone matrices (for now) but this needs to be reevaluated when
                        /// consider how the indexing system works when objects are added/removed
                        /// but for now "it just werks"                    
                        colors.push_back(object->color);
                        // if (object->color != vec4(-1,-1,-1,-1)) {
                        //     coloredVisibleModelTransforms.push_back(modelTransforms[j]);
                        // }
                    }    
                }
                               
            } 
            drawnMeshes += visibleModelTransforms.size();
            totalMeshes += modelTransforms.size();

            // std::sort( modelTransforms.begin( ), modelTransforms.end( ), [&active_camera]( mat4 lhs, mat4 rhs )
            // {
            //    return distance(active_camera.Position, vec3(lhs[3])) > distance(active_camera.Position, vec3(rhs[3]));
            // });

            for (unsigned int j = 0; j < colors.size(); j++) {
                stringstream Name;
                Name << "colors[" << j << "]";
                AnimatedModel::shader.setVec4(Name.str().c_str(), colors[j]);                 
            }   
            // glDepthMask(GL_FALSE);
            // glDisable(GL_DEPTH_TEST);
            
            // // draw highlights with depth writing disabled
            // pModel->meshes[i].DrawInstanced(pModel->shader, coloredVisibleModelTransforms);

            // glDepthMask(GL_TRUE);
            // glEnable(GL_DEPTH_TEST);

            AnimatedModel::shader.setFloat("shininess", (*currentObjects)[0].shininess);


            // draw only those that arent culled
            pModel->meshes[i].DrawInstanced(AnimatedModel::shader, visibleModelTransforms);

            
            // for (unsigned int j = 0; j < visibleModelTransforms.size(); j++) {
            //     stringstream Name;
            //     Name << "colors[" << j << "]";
            //     AnimatedModel::shader.setVec4(Name.str().c_str(), vec4(-1,-1,-1,-1));                 
            // }   
        }
    }

    // std::cout << drawnMeshes << " of " << totalMeshes << " AnimatedModels drawn." << std::endl;
    return 0;
}



int AnimatedObject::setAABB() {

    float duration = model.get()->m_pScene->mAnimations[0]->mDuration;
    float dt = 0.1f;

    for (float t = dt; t < duration; t+=0.1f) {

        boneTransforms = {};
        model.get()->getFrame(0, playFrames(0.0f, duration, t), boneTransforms);
        vector<mat4> _gBones = boneTransforms;
        for (unsigned int j  = 0; j < model.get()->meshes.size(); j++) {
            AnimatedMesh *currentMesh = &(model.get()->meshes.at(j));
            currentMesh->setVertices(_gBones);
            vector<float> AABB = currentMesh->getAABB();

        }
    }


    return 0;
}

void AnimatedObject::setFrames(float start, float end, float offset) {
    AnimatedObject::modelRegistry.at(this->modelID).at(this->uniqueID).start_frame = start;
    AnimatedObject::modelRegistry.at(this->modelID).at(this->uniqueID).end_frame = end;
    AnimatedObject::modelRegistry.at(this->modelID).at(this->uniqueID).offset = offset;

}

int AnimatedObject::getToDraw() {
    return AnimatedObject::modelRegistry.at(this->modelID).at(this->uniqueID).toDraw;
}
void AnimatedObject::setToDraw(int b) {
    AnimatedObject::modelRegistry.at(this->modelID).at(this->uniqueID).toDraw = b;
}

int AnimatedObject::getToDrawShadow() {
    return AnimatedObject::modelRegistry.at(this->modelID).at(this->uniqueID).toDrawShadow;
}
void AnimatedObject::setToDrawShadow(int b) {
    AnimatedObject::modelRegistry.at(this->modelID).at(this->uniqueID).toDrawShadow = b;
}


float AnimatedObject::getShininess() {
    return AnimatedObject::modelRegistry.at(this->modelID).at(this->uniqueID).shininess;
}
void AnimatedObject::setShininess(float b) {
    AnimatedObject::modelRegistry.at(this->modelID).at(this->uniqueID).shininess = b;
}

int AnimatedObject::getUseCustomShader() {
    return AnimatedObject::modelRegistry.at(this->modelID).at(this->uniqueID).useCustomShader;
}


void AnimatedObject::setUseCustomShader(int b) {
    AnimatedObject::modelRegistry.at(this->modelID).at(this->uniqueID).useCustomShader = b;
}

int AnimatedObject::remove() {
    AnimatedObject::modelRegistry.at(this->modelID).erase(AnimatedObject::modelRegistry.at(this->modelID).begin() + this->uniqueID);
    for (unsigned int i = this->uniqueID; i < AnimatedObject::modelRegistry.at(this->modelID).size(); i++) {
        AnimatedObject::modelRegistry.at(this->modelID).at(i).uniqueID--;
    }
    return -1;
}


AnimatedModel::AnimatedModel(string path) {
    loadModel(path);    
}

/**
 * processNode
 * 
 *
 * The expander is very very nasty in that it responds to an isolated
 * "release" event, which I'm apparently unable to block.
 * If the node is contained in a #GtkNodesNodeView, it will therefore
 * expand/contract if someone clicks the label when executing a node "drag"
 * motion.
 *
 * Note: gtk_nodes_node_block_expander() repeatedly will affect the state only once
 */
void AnimatedModel::processNode(aiNode *node, const aiScene *scene)
{
    m_pScene = scene;
    globalInverseTransform = inverse(mat4_cast(scene->mRootNode->mTransformation));
    meshes.reserve(scene->mNumMeshes);
    // process each mesh located at the current node
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene.
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));

    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

AnimatedMesh AnimatedModel::processMesh(aiMesh *mesh, const aiScene* pScene)
{
    vector<Texture> textures;
    vector<VertexTransform> vertices;
    vector<unsigned int> indices;

    // Populate the vertex attribute vectors
   // Walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        VertexTransform vertex;
        vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        //Positions.push_back(vector);
        // normals
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;
        //Normals.push_back(vector);
        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
            //TexCoords.push_back(vec);
        }
        else
            vertex.TexCoords = vec2(0.0f, 0.0f);
            //TexCoords.push_back(vec2(0.0f, 0.0f));
        // tangent
        vector.x = mesh->mTangents[i].x;
        vector.y = mesh->mTangents[i].y;
        vector.z = mesh->mTangents[i].z;
        //Tangent.push_back(vector);
        // bitangent
        vector.x = mesh->mBitangents[i].x;
        vector.y = mesh->mBitangents[i].y;
        vector.z = mesh->mBitangents[i].z;
        //Bitangent.push_back(vector);
        vertices.push_back(vertex);
    }

    // Populate the index buffer
    for (unsigned int j = 0 ; j < mesh->mNumFaces ; j++) {
        aiFace face = mesh->mFaces[j];
        for (unsigned int k = 0; k < face.mNumIndices; k++) {
            indices.push_back(face.mIndices[k]);
        }
    }

    aiMaterial* material = pScene->mMaterials[mesh->mMaterialIndex];

    // 1. diffuse maps
    vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    AnimatedMesh m(vertices, indices, textures);

    // load bones for mesh
    loadBones(m);

    return m;
}

int AnimatedModel::getFrame(unsigned int uniqueID, float currentFrame, vector<mat4> &objectBoneTransforms) {
    AnimatedModel::shader.use();
    if (objectBoneTransforms.size() == 0) {
        BoneTransform(currentFrame, objectBoneTransforms);
    }

    for (unsigned int i = 0 ; i < objectBoneTransforms.size() ; i++) {
        AnimatedModel::shader.SetBoneTransform(uniqueID, i, objectBoneTransforms[i]);
    }
    return 0;
}

int AnimatedModel::Draw() {
      for (unsigned int i = 0; i < meshes.size(); i++) {
          meshes[i].Draw(shader);
      }
      return 0;
}

// draws the model, and thus all its meshes
int AnimatedModel::DrawInstanced(std::vector<mat4> modelTransforms)
{
    for(unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].DrawInstanced(shader, modelTransforms);
    }
    return 0;
}


int AnimatedModel::loadModel(string path) {

    m_Importer = new Assimp::Importer();

    // read file via ASSIMP
    const aiScene* scene = m_Importer->ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    // check for errors
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        cout << "ERROR::ASSIMP:: " << m_Importer->GetErrorString() << endl;
        return 0;
    }
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);


    const aiAnimation* pAnimation = m_pScene->mAnimations[animationIndex];
    storeNodeAnim(pAnimation);

    const char *_vShader = "#version 300 es\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aNormal;\n"
    "layout (location = 2) in vec2 aTexCoord;\n"
    "layout (location = 5) in ivec4 BoneIDs;\n"
    "layout (location = 6) in vec4 Weights;\n"
    "layout (location = 7) in mat4 instanceMatrix;\n"
    "out vec2 TexCoord;\n"
    "out float diffuse;\n"
    "out vec4 color;\n"
    "out vec3 FragPos;\n"
    "out vec3 normal;\n"
    "flat out int _shadowPass;\n"
    "uniform mat4 proj_view;\n"
    "uniform mat4 gBones[20*50];\n"
    "uniform vec4 colors[20];\n"
    "uniform mat4 lightSpaceMatrix;\n"
    "uniform int shadowPass;\n"
    "uniform vec3 lightPos;\n"
    "uniform vec3 viewPos;\n"
    "out vec3 _viewPos;\n"
    "out vec3 _lightPos;\n"
    "uniform float nearPlane;\n"
    "uniform float farPlane;\n"
    "out float alpha;\n"
    "out float alpha2;\n"
    "float getFogFactor(float d, float nearPlane, float farPlane)\n"
    "{\n"
        "float FogMax = 1.0f * farPlane;\n"
        "float FogMin = 0.5f * farPlane;\n"
        "if (d>=FogMax) return 1.0f;\n"
        "if (d<=FogMin) return 0.0f;\n"
        "return 1.0f - (FogMax - d) / (FogMax - FogMin);\n"
    "}\n"
    "float getFogFactorAlpha(float d, float nearPlane, float farPlane)\n"
    "{\n"
        "float FogMax = 1.0f * farPlane;\n"
        "float FogMin = 0.7f * farPlane;\n"
        "if (d>=FogMax) return 1.0f;\n"
        "if (d<=FogMin) return 0.0f;\n"
        "return 1.0f - (FogMax - d) / (FogMax - FogMin);\n"
    "}\n"
    "void main()\n"
    "{\n"   
        "mat4 BoneTransform = gBones[gl_InstanceID * 50 + BoneIDs[0]] * Weights[0];\n"
        "BoneTransform     += gBones[gl_InstanceID * 50 + BoneIDs[1]] * Weights[1];\n"
        "BoneTransform     += gBones[gl_InstanceID * 50 + BoneIDs[2]] * Weights[2];\n"
        "BoneTransform     += gBones[gl_InstanceID * 50 + BoneIDs[3]] * Weights[3];\n"
        "vec4 fragPos = instanceMatrix * BoneTransform * vec4(aPos, 1.0);\n"
        "if (shadowPass == 1) {\n"
            "gl_Position = lightSpaceMatrix * fragPos;\n"
            "return;\n"
        "}\n"
        "mat3 normalMatrix = transpose(inverse(mat3(instanceMatrix * BoneTransform)));\n"
        "vec3 Normal = normalize(normalMatrix * aNormal);\n"
        "color = colors[gl_InstanceID];\n"
        "gl_Position = proj_view * fragPos;\n"
        "TexCoord = aTexCoord;\n"
        "vec3 lightDir = normalize(lightPos);\n"  
        "diffuse =  max(dot(Normal, lightDir), 0.0);\n"
        "normal = Normal;\n"
        "FragPos = fragPos.xyz;\n"
        "float d = distance(viewPos, FragPos);\n"
        "alpha = getFogFactor(d, nearPlane, farPlane);\n"
        "alpha2 = getFogFactorAlpha(d, nearPlane, farPlane);\n"
        "_shadowPass = shadowPass;\n"
        "_viewPos = viewPos;\n"
        "_lightPos = lightPos;\n"
    "}\n";
const char *_fShader =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 TexCoord;\n"
    "in float diffuse;\n"
    "in vec4 color;\n"
    "in float alpha;\n"
    "in float alpha2;\n"
    "in vec3 FragPos;\n"
    "in vec3 normal;\n"
    "flat in int _shadowPass;\n"  
    "out vec4 FragColor;\n"
    "uniform sampler2D texture_diffuse1;\n"
    "uniform vec3 backgroundColor;\n"
    "in vec3 _viewPos;\n"
    "in vec3 _lightPos;\n"
    "uniform float shininess;\n"
    "uniform samplerCube skybox;\n"
    "float ambient = 0.55;\n"           
    "vec3 fresnelSchlick(float cosTheta, vec3 F0)\n"
    "{\n"
        "return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);\n"
    "}\n"                                                
    "void main()\n"
    "{\n"    
        "vec4 tex = texture(texture_diffuse1, TexCoord);\n"
        "FragColor = (_shadowPass == 1 ? vec4(0,0,0,1) :  vec4(tex.rgb * (diffuse + ambient), tex.a));\n"
        "FragColor = mix(FragColor, vec4(backgroundColor, 1.0), alpha);\n"
        "FragColor.a = 1.0-alpha2;\n"
        "vec3 F0 = vec3(0.04);\n"
        "F0      = mix(F0, FragColor.rgb, 0.0f);\n"
        "vec3 lightDir   = normalize(_lightPos - FragPos);\n"
        "vec3 viewDir    = normalize(_viewPos - FragPos);\n"
        "vec3 halfwayDir = normalize(lightDir + viewDir);\n"
        "FragColor.rgb =  (color != vec4(-1.0,-1.0,-1.0,-1.0) ? FragColor.rgb + fresnelSchlick(dot(normal, viewDir), F0) * color.rgb : FragColor.rgb);\n"
        "if (color.a != -1.0) {\n"
            "FragColor.a = color.a;\n"
        "}\n"
        "vec3 I = normalize(FragPos - _viewPos);\n"
        "vec3 R = reflect(I, normalize(normal));\n"
        "FragColor = (shininess == 1.0f ? FragColor * vec4(texture(skybox, R).rgb,1.0) : FragColor);\n"
    "}\n";

    AnimatedModel::shader.setup(_vShader, _fShader);

    model = mat4(1.0);
    start_frame = 0.0f;
    end_frame = 0.0f;


    return 0;
}

void AnimatedModel::loadBones(AnimatedMesh &mesh)
{
        aiMesh* pMesh = m_pScene->mMeshes[meshes.size()];
        for (unsigned int i = 0 ; i < pMesh->mNumBones ; i++) {
          unsigned int BoneIndex = 0;
          string BoneName(pMesh->mBones[i]->mName.data);

          if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {
              // Allocate an index for a new bone
              BoneIndex = m_NumBones;
              m_NumBones++;
              BoneInfo bi;
              m_BoneInfo.push_back(bi);
              m_BoneInfo[BoneIndex].BoneOffset = mat4_cast(pMesh->mBones[i]->mOffsetMatrix);
              m_BoneMapping[BoneName] = BoneIndex;
          }
          else {
              BoneIndex = m_BoneMapping[BoneName];
          }
          for (unsigned int j = 0 ; j < pMesh->mBones[i]->mNumWeights ; j++) {
              unsigned int VertexID = pMesh->mBones[i]->mWeights[j].mVertexId;
              float Weight  = pMesh->mBones[i]->mWeights[j].mWeight;
              mesh.vertices[VertexID].Bones.AddBoneData(BoneIndex, Weight);
          }
        }
}

void AnimatedModel::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, mat4 ParentTransform)
{
    string NodeName(pNode->mName.data);

    const aiAnimation* pAnimation = m_pScene->mAnimations[animationIndex];

    mat4 NodeTransformation =  mat4_cast(pNode->mTransformation);

    const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

    if (pNodeAnim) {
        // Interpolate scaling and generate scaling transformation matrix
        aiVector3D Scaling;
        CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
        mat4 ScalingM(1.0f);
        scale(ScalingM, vec3(Scaling.x, Scaling.y, Scaling.z));


        // Interpolate rotation and generate rotation transformation matrix
        aiQuaternion RotationQ;
        CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
        mat4 RotationM = mat4_cast(RotationQ.GetMatrix());

        // Interpolate translation and generate translation transformation matrix
        aiVector3D Translation;
        CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
        mat4 TranslationM(1.0f);
        TranslationM = glm::translate(TranslationM, vec3(Translation.x, Translation.y, Translation.z));

        NodeTransformation = TranslationM * RotationM * ScalingM;
    }
    mat4 GlobalTransformation = ParentTransform * NodeTransformation;

    if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
        unsigned int BoneIndex = m_BoneMapping[NodeName];
        mat4 BoneOffset, FinalTransformation;
        BoneOffset = m_BoneInfo[BoneIndex].BoneOffset;
        FinalTransformation = globalInverseTransform * GlobalTransformation * BoneOffset;
        m_BoneInfo[BoneIndex].FinalTransformation = FinalTransformation;
    }

    for (unsigned int i = 0 ; i < pNode->mNumChildren ; i++) {
        ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
    }
}
// start and end frames in seconds, spliced from the animation file
void AnimatedModel::setFrames(float start, float end) {
    this->start_frame = start;
    this->end_frame = end;
}

void AnimatedModel::BoneTransform(float TimeInSeconds, vector<mat4>& Transforms)
{
    mat4 Identity(1.0f);
    //std::cout << m_pScene->mAnimations[animationIndex]->mTicksPerSecond << std::endl;
    float TicksPerSecond = (float)(m_pScene->mAnimations[animationIndex]->mTicksPerSecond != 0 ? m_pScene->mAnimations[animationIndex]->mTicksPerSecond : 25.0f);
    float TimeInTicks = TimeInSeconds * TicksPerSecond;
    float AnimationTime = fmod(TimeInTicks, (float)m_pScene->mAnimations[animationIndex]->mDuration);
    ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity);

    Transforms.resize(m_NumBones);

    for (unsigned int i = 0 ; i < m_NumBones ; i++) {
        Transforms[i] = m_BoneInfo[i].FinalTransformation;
    }
}

// You can store the aiNodeAnim's in a map and use that to look them up instead of your FindNodeAnim function for every node every frame. I don't have an FPS counter set up but my profiler said that function was consuming most of my CPU and by using a map the animation went from mildly choppy to silky smooth. I do realize this is just meant to be illustrative and educational code.

void AnimatedModel::storeNodeAnim(const aiAnimation* pAnimation) {

    for (unsigned int i = 0 ; i < pAnimation->mNumChannels ; i++) {
        const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

        nodeAnimMap.insert(std::make_pair(string(pNodeAnim->mNodeName.data), pNodeAnim));
    }
}
const aiNodeAnim* AnimatedModel::FindNodeAnim(const aiAnimation* pAnimation, const string NodeName)
{
    if (nodeAnimMap.find(NodeName) != nodeAnimMap.end()) {
        return nodeAnimMap[NodeName];
    }
    // for (unsigned int i = 0 ; i < pAnimation->mNumChannels ; i++) {
    //     const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

    //     if (string(pNodeAnim->mNodeName.data) == NodeName) {
    //         return pNodeAnim;
    //     }
    // }

    return NULL;
}
// const aiNodeAnim* AnimatedModel::FindNodeAnim(const aiAnimation* pAnimation, const string NodeName)
// {
//     for (unsigned int i = 0 ; i < pAnimation->mNumChannels ; i++) {
//         const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

//         if (string(pNodeAnim->mNodeName.data) == NodeName) {
//             return pNodeAnim;
//         }
//     }

//     return NULL;
// }

void AnimatedModel::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumPositionKeys == 1) {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }

    unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
    unsigned int NextPositionIndex = (PositionIndex + 1);
    assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
    float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}


void AnimatedModel::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumRotationKeys == 1) {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }

    unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
    unsigned int NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& EndRotationQ   = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
    Out = Out.Normalize();
}


void AnimatedModel::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumScalingKeys == 1) {
        Out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

    unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
    unsigned int NextScalingIndex = (ScalingIndex + 1);
    assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
    float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D& End   = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}

unsigned int AnimatedModel::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    for (unsigned int i = 0 ; i < pNodeAnim->mNumPositionKeys - 1 ; i++) {
        if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }

    //assert(0);

    return 0;
}


unsigned int AnimatedModel::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumRotationKeys > 0);

    for (unsigned int i = 0 ; i < pNodeAnim->mNumRotationKeys - 1 ; i++) {
        if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
            return i;
        }
    }

    //assert(0);

    return 0;
}


unsigned int AnimatedModel::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumScalingKeys > 0);

    for (unsigned int i = 0 ; i < pNodeAnim->mNumScalingKeys - 1 ; i++) {
        if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }

    //assert(0);

    return 0;
}

void AnimatedModel::remove() {
}


AnimatedModel::~AnimatedModel() {
    // @todo deleting causes a segfault in some cases
    // find a way to make the importer statically allocated
        //delete m_Importer;
    std::cout << "deleting animated model..." << std::endl;
}


float rayAnimatedObjectIntersect(glm::vec3 rayOrigin, glm::vec3 rayDirection, AnimatedObject object)
{   
  std::vector<float> intersect_values = {};
  for (unsigned int j = 0; j < object.model.get()->meshes.size(); j++) {
    std::vector<vec3> vertices = object.getAnimatedVertices().at(j);
    for (unsigned int i = 0; i < vertices.size()-3; i+=3) {
      vec3 v1 = glm::vec3(object.getModelMatrix() * glm::vec4(vertices.at(i+0), 1.0));
      vec3 v2 = glm::vec3(object.getModelMatrix() * glm::vec4(vertices.at(i+1), 1.0));
      vec3 v3 = glm::vec3(object.getModelMatrix() * glm::vec4(vertices.at(i+2), 1.0));
      float t = rayTriangleIntersect(rayOrigin, rayDirection, v1, v2, v3);
      if (t) {
        intersect_values.push_back(t);
      }
    }
  }
  if (intersect_values.size()) {
    float shortest_distance = FLT_MAX;
    for (unsigned int i = 0; i < intersect_values.size(); i++) {
        if (intersect_values.at(i) < shortest_distance) {
          shortest_distance = intersect_values.at(i);
        }
    }
    return shortest_distance;
  } else {
    return 0.0f;
  }
  return 0.0f;
}



