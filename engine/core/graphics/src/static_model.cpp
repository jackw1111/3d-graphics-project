#include "static_model.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


// a collection of objects all of the same filePath
std::vector<std::vector<StaticObject>> StaticObject::modelRegistry = {};
unsigned int StaticObject::uniqueObjectCount = 0;
StaticShader StaticModel::shader = StaticShader();

StaticObject::StaticObject(const std::string &filePath) {
    std::cout << "got here0" << std::endl;
    load(filePath);
}

int StaticObject::load(const std::string &filePath) {
    std::cout << "got here1" << std::endl;
    // check if a registry for the filePath has been created
    bool found = false;
    for (unsigned int i = 0; i < StaticObject::modelRegistry.size(); i++) {
        // get a reference to the first object of each unique filePath
        StaticObject* storedObject = &StaticObject::modelRegistry[i][0];
        // check if filePaths match
        if (storedObject->filePath == filePath) {

            // reference underlying StaticModel data
            this->filePath = storedObject->filePath;
            this->model = storedObject->model;
            this->modelID = storedObject->modelID;
            this->meshIsCulled = storedObject->meshIsCulled;
            this->uniqueID = StaticObject::modelRegistry[this->modelID].size();
            this->color = vec4(-1,-1,-1,-1);
            this->drawBoundingBox = false;

            // // add it to the store of objects
            StaticObject::modelRegistry[this->modelID].push_back(*this);
            std::cout << "copying object" << std::endl;
            found = true;
            vector<float> AABB =  this->model.get()->meshes.at(0).getAABB();
            glm::vec3 min = glm::vec3(AABB[0], AABB[1], AABB[2]);
            glm::vec3 max = glm::vec3(AABB[3], AABB[4], AABB[5]);
            this->boundingBox.setup(min, max);

            break;
        }
    }
    if (!found) {
        std::cout << "got here4" << std::endl;

        // object is unique, create an object store in the registry
        this->filePath = filePath;
        this->model = std::shared_ptr<StaticModel>(new StaticModel(filePath));
        this->modelID = StaticObject::uniqueObjectCount++;
        this->uniqueID = 0;
        this->meshIsCulled.resize(this->model.get()->meshes.size());
        this->color = vec4(-1,-1,-1,-1);
        this->drawBoundingBox =  false;
        vector<float> AABB =  this->model.get()->meshes.at(0).getAABB();
        glm::vec3 min = glm::vec3(AABB[0], AABB[1], AABB[2]);
        glm::vec3 max = glm::vec3(AABB[3], AABB[4], AABB[5]);
        this->boundingBox.setup(min, max);

        StaticObject::modelRegistry.push_back({*this});
        std::cout << "got here3" << std::endl;

    }
    return 0;
}


shared_ptr<StaticModel> StaticObject::getModel() {
    return StaticObject::modelRegistry.at(this->modelID).at(this->uniqueID).model;
}

vec4 StaticObject::getColor() {
    return StaticObject::modelRegistry.at(this->modelID).at(this->uniqueID).color;
}

int StaticObject::setColor(vec4 c) {
    StaticObject::modelRegistry.at(this->modelID).at(this->uniqueID).color = c;
    return 0;
}

bool StaticObject::getDrawBoundingBox() {
    return StaticObject::modelRegistry.at(this->modelID).at(this->uniqueID).drawBoundingBox;
}

int StaticObject::setDrawBoundingBox(bool value) {
    StaticObject::modelRegistry.at(this->modelID).at(this->uniqueID).drawBoundingBox = value;
    return 0;
}

vec3 StaticObject::getAABBMin() {
    return StaticObject::modelRegistry.at(this->modelID).at(this->uniqueID).getModel().get()->meshes[0].boundingBox.min;
}

int StaticObject::setAABBMin(vec3 value) {
    StaticObject::modelRegistry.at(this->modelID).at(this->uniqueID).getModel().get()->meshes[0].boundingBox.min = value;
    return 0;
}

vec3 StaticObject::getAABBMax() {
    return StaticObject::modelRegistry.at(this->modelID).at(this->uniqueID).boundingBox.max;
}

int StaticObject::setAABBMax(vec3 value) {
    StaticObject::modelRegistry.at(this->modelID).at(this->uniqueID).boundingBox.max = value;
    return 0;
}

mat4 StaticObject::getModelMatrix() {
    return StaticObject::modelRegistry.at(this->modelID).at(this->uniqueID).modelMatrix;
}

int StaticObject::setModelMatrix(mat4 m) {
    StaticObject::modelRegistry.at(this->modelID).at(this->uniqueID).modelMatrix = m;
    return 0;
}

int StaticObject::getToDraw() {
    return StaticObject::modelRegistry.at(this->modelID).at(this->uniqueID).toDraw;
}

void StaticObject::setToDraw(int b) {
    StaticObject::modelRegistry.at(this->modelID).at(this->uniqueID).toDraw = b;
}

vector<mat4> StaticObject::getObjectTransforms(vector<StaticObject> &currentObjects) {
    vector<mat4> modelTransforms = {};
    for (unsigned int i = 0; i < currentObjects.size(); i++) {
        if (currentObjects[i].getToDraw()) {
            modelTransforms.push_back(currentObjects[i].getModelMatrix());
        }
    }
    return modelTransforms;
}


vector<unsigned int> StaticObject::getObjectIndices(vector<StaticObject> &currentObjects) {
    vector<unsigned int> modelIndices = {};
    for (unsigned int i = 0; i < currentObjects.size(); i++) {
        if (currentObjects[i].getToDraw()) {
            modelIndices.push_back(i);
        }
    }
    return modelIndices;
}

int StaticObject::drawAllObjects(Camera &active_camera, StaticShader &shader) {

    unsigned int totalMeshes = 0;
    unsigned int drawnMeshes = 0;

    shader.use();
    glBindAttribLocation(shader.ID, 0, "aPos");
    glBindAttribLocation(shader.ID, 1, "aNormal");
    glBindAttribLocation(shader.ID, 2, "aTexCoords");
    glBindAttribLocation(shader.ID, 5, "instanceMatrix");

    // for every model in registry
    for (unsigned int index = 0; index <  StaticObject::modelRegistry.size(); index++) {
        vector<StaticObject> *currentObjects = &StaticObject::modelRegistry[index];

        // get the modelMatrices for current objects
        vector<mat4> modelTransforms = StaticObject::getObjectTransforms(*currentObjects);
        vector<unsigned int> objectIndices = StaticObject::getObjectIndices(*currentObjects);

        // get a reference to the underlying model data
        StaticModel *pModel = (*currentObjects)[0].model.get();

        // for every mesh
        for (unsigned int i = 0; i < pModel->meshes.size(); i++) {
            // reset transforms
            vector<mat4> visibleModelTransforms = {};
            vector<mat4> coloredVisibleModelTransforms = {};
            vector<vec4> colors = {};
            //for each transform
            for (unsigned int j = 0; j < objectIndices.size(); j++) {  
                // get the current object
                StaticObject *object = &(*currentObjects).at(objectIndices.at(j));
                StaticMesh *currentMesh = &(object->model.get()->meshes[i]);

                vector<float> AABB = currentMesh->getAABB();
                glm::vec3 min = glm::vec3(AABB[0], AABB[1], AABB[2]);
                glm::vec3 max = glm::vec3(AABB[3], AABB[4], AABB[5]);
                currentMesh->boundingBox.setBoundary(min, max);
                object->setAABBMin(min);
                object->setAABBMax(max);
                //std::cout << "c:" << glm::to_string(currentMesh->boundingBox.min)<< ", "<< glm::to_string(currentMesh->boundingBox.max) << std::endl;
                if (object->drawBoundingBox) {
                    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
                    glDisable(GL_CULL_FACE);
                    currentMesh->boundingBox.modelMatrix = object->getModelMatrix();
                    currentMesh->boundingBox.setMatrix("model", object->getModelMatrix());
                    currentMesh->boundingBox.setMatrix("view", active_camera.view_matrix);
                    currentMesh->boundingBox.setMatrix("projection", active_camera.projection_matrix);
                    currentMesh->boundingBox.draw();
                    StaticModel::shader.use();
                    //glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
                    glEnable(GL_CULL_FACE);
                }

                // check if object mesh is either not culled or not occluded
                if (!object->meshIsCulled[i]) {

                    visibleModelTransforms.push_back(modelTransforms[j]);

                    colors.push_back(object->color);
                    if (object->color != vec4(-1,-1,-1,-1)) {
                        coloredVisibleModelTransforms.push_back(modelTransforms[j]);
                    }
                }
            } 
            drawnMeshes += visibleModelTransforms.size();
            totalMeshes += modelTransforms.size();

            for (unsigned int j = 0; j < colors.size(); j++) {
                stringstream Name;
                Name << "colors[" << j << "]";
                StaticModel::shader.setVec4(Name.str().c_str(), colors[j]);                 
            }   

            // Sorting doesn't seem to make sense when the fragment shader is so cheap
            
            // std::map<float, glm::mat4> sorted;
            // for (unsigned int i = 0; i < visibleModelTransforms.size(); i++)
            // {
            //     float distance = glm::length2(active_camera.Position - vec3(visibleModelTransforms[i][3]));
            //     sorted[distance] = visibleModelTransforms[i];
            // }
            // std::vector<glm::mat4> sortedModelTransforms = {};
            // for (auto& x: sorted) {
            //     sortedModelTransforms.push_back(x.second);
            // }
            
            // TO DO
            // maybe export shininess into a field that can then
            // be parsed from ASSIMP into a material field
            // materials such as metallic, shininess, etc...
            // ie. PBR
            //shader.setFloat("shininess", pModel->meshes[i].mats.shininess);
            shader.setFloat("shininess", (*currentObjects)[0].shininess);

            // draw only those that arent culled
            pModel->meshes[i].DrawInstanced(shader, visibleModelTransforms);
        }
    }

    //std::cout << drawnMeshes << " of " << totalMeshes << " StaticModels drawn." << std::endl;
    return 0;
}

float StaticObject::getShininess() {
    return StaticObject::modelRegistry.at(this->modelID).at(0).shininess;
}
void StaticObject::setShininess(float b) {
    StaticObject::modelRegistry.at(this->modelID).at(0).shininess = b;
}

int StaticObject::remove() {
    //std::cout << StaticObject::modelRegistry.at(this->modelID).size() << std::endl;
    std::cout << "removing staticobject" << std::endl;
    StaticObject::modelRegistry.at(this->modelID).erase(StaticObject::modelRegistry.at(this->modelID).begin() + this->uniqueID);
    for (unsigned int i = this->uniqueID; i < StaticObject::modelRegistry.at(this->modelID).size(); i++) {
        StaticObject::modelRegistry.at(this->modelID).at(i).uniqueID--;
    }

    // std::cout << StaticObject::modelRegistry.at(this->modelID).size() << std::endl;

    // //std::cout << "size: " << StaticObject::modelRegistry.at(this->modelID).size() << std::endl;
    // //std::cout << this->uniqueID << ", " << StaticObject::modelRegistry.at(this->modelID).size() << std::endl;

    return -1;
}


// void StaticObject::translate(glm::vec3 pos) {
//     model = glm::translate(model, pos);
// }

std::vector<float> StaticModel::getAABB() {

    float minX = numeric_limits<float>::max();
    float maxX = numeric_limits<float>::min();
    float minY = numeric_limits<float>::max();
    float maxY = numeric_limits<float>::min();
    float minZ = numeric_limits<float>::max();
    float maxZ = numeric_limits<float>::min();

    for (unsigned int i = 0; i < meshes.size(); i++) {
        vector<float> aabb = meshes[i].getAABB();
        if (aabb[0] < minX) {
            minX = aabb[0];
        }
        if (aabb[1] < minY) {
            minY = aabb[1];
        }
        if (aabb[2] < minZ) {
            minZ = aabb[2];
        }
        if (aabb[3] > maxX) {
            maxX = aabb[3];
        }
        if (aabb[4] > maxY) {
            maxY = aabb[4];
        }
        if (aabb[5] > maxZ) {
            maxZ = aabb[5];
        }
    }
    glm::vec3 point1 = glm::vec3(model * glm::vec4(minX, minY, minZ, 1.0));
    glm::vec3 point2 = glm::vec3(model * glm::vec4(maxX, maxY, maxZ, 1.0));

    vector<float> limits = {point1.x, point1.y, point1.z, point2.x, point2.y, point2.z};

    return limits;
}

StaticModel::StaticModel(string path)
{
    loadModel(path);  
}

// draws the model, and thus all its meshes
int StaticModel::Draw()
{
    for(unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].Draw(shader);
    }
    return 1;
}


// draws the model, and thus all its meshes
void StaticModel::DrawInstanced(std::vector<mat4> modelTransforms)
{

    for (unsigned int i = 0; i < modelTransforms.size(); i++) {
        for (unsigned int j = 0; j < meshes.size(); j++) {
            if (meshes[j].frustumCulled == false) {
                meshes[j].DrawInstanced(shader, {modelTransforms[i]});
            }
        }
    }
}


/*  Functions   */
// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
int StaticModel::loadModel(string path)
{
    static Assimp::Importer m_Importer;
    // read file via ASSIMP
    const aiScene* scene = m_Importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    std::cout << "scene: " << scene << std::endl;
    // check for errors
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        cout << "ERROR::ASSIMP:: " << m_Importer.GetErrorString() << endl;
        return 0;
    }
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);

    const char* _vShader = 
    "#version 300 es\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aNormal;\n"
    "layout (location = 2) in vec2 aTexCoords;\n"
    "layout (location = 5) in mat4 instanceMatrix;\n"
    "out vec2 TexCoord;\n"
    "out float diffuse;\n"
    "out vec4 FragPosLightSpace;\n"
    "out vec3 FragPos;\n"
    "out vec3 Normal;\n"
    "out vec4 color;\n"
    "uniform mat4 proj_view;\n"
    "uniform vec4 colors[1000];\n"
    "uniform mat4 lightSpaceMatrix;\n"
    "uniform vec3 viewPos;\n"
    "uniform vec3 lightPos;\n"
    "uniform float nearPlane;\n"
    "uniform float farPlane;\n"
    "out float alpha;\n"
    "out float alpha2;\n"
    "out vec3 _viewPos;\n"
    "out vec3 _lightPos;\n"
    "float getFogFactor(float d, float nearPlane, float farPlane)\n"
    "{\n"
    "    float FogMax = 1.0f * farPlane;\n"
    "    float FogMin = 0.5f * farPlane;\n"
    "    if (d>=FogMax) return 1.0f;\n"
    "    if (d<=FogMin) return 0.0f;\n"
    "    return 1.0f - (FogMax - d) / (FogMax - FogMin);\n"
    "}\n"
    "float getFogFactorAlpha(float d, float nearPlane, float farPlane)\n"
    "{\n"
    "    float FogMax = 1.0f * farPlane;\n"
    "    float FogMin = 0.7f * farPlane;\n"
    "    if (d>=FogMax) return 1.0f;\n"
    "    if (d<=FogMin) return 0.0f;\n"
    "    return 1.0f - (FogMax - d) / (FogMax - FogMin);\n"
    "}\n"
    "void main()\n"
    "{\n"
    "    vec4 fragPos = instanceMatrix * vec4(aPos, 1.0);\n"
    "    TexCoord = aTexCoords;\n"
    "    color = colors[gl_InstanceID];\n"
    "    mat3 normalMatrix = transpose(inverse(mat3(instanceMatrix)));\n"
    "    Normal = normalize(normalMatrix * aNormal);\n"
    "    vec3 lightDir = normalize(lightPos);\n"  
    "    diffuse =  max(dot(Normal, lightDir), 0.0);\n"
    "    gl_Position = proj_view * fragPos;\n"
    "    FragPosLightSpace = lightSpaceMatrix * fragPos;\n"
    "    FragPos = fragPos.xyz;\n"
    "    float d = distance(viewPos, FragPos);\n"
    "    alpha = getFogFactor(d, nearPlane, farPlane);\n"
    "    alpha2 = getFogFactorAlpha(d, nearPlane, farPlane);\n"
    "    _viewPos = viewPos;\n"
    "    _lightPos = lightPos;\n"
    "}\n";

    const char* _fShader =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 TexCoord;\n"
    "in vec4 color;\n"
    "in vec4 FragPosLightSpace;\n"
    "in vec4 farFragPosLightSpace;\n"
    "in vec3 FragPos;\n"
    "in float alpha;\n"
    "in float alpha2;\n"
    "in vec3 Normal;\n"
    "in vec3 _viewPos;\n"
    "in vec3 _lightPos;\n"
    "out vec4 FragColor;\n"

    "uniform sampler2D texture_diffuse1;\n"
    "uniform sampler2D depthMap;\n"
    "uniform sampler2D ditherMap;\n"
    "uniform vec3 backgroundColor;\n"
    "uniform samplerCube skybox;\n"
    "float ambient = 0.55;\n"
    "in float diffuse;\n"
    "uniform float shininess;\n"
    "vec3 fresnelSchlick(float cosTheta, vec3 F0)\n"
    "{\n"
    "    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);\n"
    "}\n"                             
    "float ShadowCalculation(vec4 fragPosLightSpace)\n"
    "{\n"
    "    // perform perspective divide\n"
    "    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;\n"
    "    // transform to [0,1] range\n"
    "    projCoords = projCoords * 0.5 + 0.5;\n"
    "    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)\n"
    "    float closestDepth = texture(depthMap, projCoords.xy).r;\n" 
    "    // get depth of current fragment from light's perspective\n"
    "    float currentDepth = projCoords.z;\n"
    "    // check whether current frag pos is in shadow\n"
    "    float shadow = currentDepth > closestDepth  ? currentDepth - closestDepth: 0.0;\n"
    "    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.\n"
    "    if(projCoords.z > 1.0)\n"
    "        shadow = 0.0;\n"
    "    return shadow;\n"
    "}\n"
    "void main()\n"
    "{\n"    
    "    //float dist = distance(FragPos, _viewPos);\n"
    "    float shadow = ShadowCalculation(FragPosLightSpace);\n"
    "    shadow = (shadow == 0.0 ? 1.0 : max(min(shadow, 1.0),0.35));\n"      
    "    FragColor = texture(texture_diffuse1, TexCoord);\n"
    "    if (FragColor.a < 0.5f) {\n"
    "        discard;\n"
    "    }\n"
    "    //vec4 dither = vec4(texture2D(ditherMap, gl_FragCoord.xy / 8.0).r / 32.0 - (1.0 / 128.0));\n"
    "    //vec3 lightDir = normalize(_lightPos - FragPos);\n"  
    "    vec3 viewDir = normalize(_viewPos - FragPos);\n"
    "    //vec3 reflectDir = reflect(-lightDir, Normal);\n"  
    "    //float specular = pow(max(dot(viewDir, reflectDir), 0.0), 2);\n"
    "    FragColor *= shadow;\n"
    "    FragColor.rgb *= (diffuse + ambient); // + specular\n"
    "    FragColor = mix(FragColor, vec4(backgroundColor, 1.0), alpha);\n"
    "    FragColor.a = 1.0f-alpha2;\n"
    "    //FragColor.rgb += dither.rgb*0.05;\n"
    "    vec3 F0 = vec3(0.04);\n"
    "    F0      = mix(F0, FragColor.rgb, 0.0f);\n"
    "    FragColor.rgb =  (color != vec4(-1,-1,-1,-1) ? FragColor.rgb + fresnelSchlick(dot(Normal, viewDir), F0) * color.rgb : FragColor.rgb);\n"
    "    if (color.a != -1.0f) {\n"
    "        FragColor.a = color.a;\n"
    "    }\n"
    "    vec3 I = normalize(FragPos - _viewPos);\n"
    "    vec3 R = reflect(I, normalize(Normal));\n"
    "    FragColor = (shininess == 1.0f ? FragColor * vec4(texture(skybox, R).rgb,1.0) : FragColor);\n"
    "}\n";
    

    StaticModel::shader.setup(_vShader, _fShader);
    model = mat4(1.0);

    isInitialized = true;

    for (unsigned int i = 0; i < this->meshes.size(); i++) {
        vector<float> AABB =  this->meshes[i].getAABB();
        glm::vec3 min = glm::vec3(AABB[0], AABB[1], AABB[2]);
        glm::vec3 max = glm::vec3(AABB[3], AABB[4], AABB[5]);
        this->meshes[i].boundingBox.setup(min, max);
        this->meshes[i].boundingBox.toDraw = false;
    }

    return 0;
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void StaticModel::processNode(aiNode *node, const aiScene *scene)
{
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

StaticMesh StaticModel::processMesh(aiMesh *mesh, const aiScene *scene)
{
    // data to fill
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    // Walk through each of the mesh's vertices
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // normals
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;
        // texture coordinates
        if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        // tangent
        vector.x = mesh->mTangents[i].x;
        vector.y = mesh->mTangents[i].y;
        vector.z = mesh->mTangents[i].z;
        vertex.Tangent = vector;
        // bitangent
        vector.x = mesh->mBitangents[i].x;
        vector.y = mesh->mBitangents[i].y;
        vector.z = mesh->mBitangents[i].z;
        vertex.Bitangent = vector;
        vertices.push_back(vertex);
    }
    // now walk through each of the mesh's faces (a face is triangle if it is a triangle mesh) and retrieve the corresponding vertex indices.
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    std::cout << "number of materials: " << scene->mNumMaterials << std::endl;

    Material mat;
    aiColor3D color;
    float shininess;
    // Read mtl file vertex data
    material->Get(AI_MATKEY_COLOR_SPECULAR, color);
    material->Get(AI_MATKEY_SHININESS, shininess);

    mat.Ks = glm::vec4(color.r, color.g, color.b, 1.0);
    mat.shininess = shininess;
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

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

    std::cout << "textures size: " << textures.size() << std::endl;
    // return a mesh object created from the extracted mesh data
    return StaticMesh(vertices, indices, textures, mat);
}

void StaticModel::remove() {
    
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
vector<Texture> StaticModel::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
    vector<Texture> textures;
    std::cout << "no of textures: " << mat->GetTextureCount(type) << std::endl;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for(unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if(!skip)
        {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            std::cout << "path: " << texture.path << std::endl;
            std::cout << "id: " << texture.id << std::endl;
            textures.push_back(texture);
            textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        }
    }
    return textures;
}

unsigned int TextureFromFile(const char *path, const string &directory)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(false);
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = 0;
        GLenum format2 = 0;

        if (nrComponents == 1){
            format = GL_RED;
        }
        else if (nrComponents == 3){
            format = GL_RGB;
        }
        else if (nrComponents == 4){
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);


        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << filename << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int TextureFromData(unsigned int width, unsigned int height, std::vector<glm::vec3> data)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGB, GL_FLOAT, &data[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    

    return textureID;
}

float rayObjectIntersect(glm::vec3 rayOrigin, glm::vec3 rayDirection, StaticObject object)
{   
  std::vector<float> intersect_values = {};
  for (unsigned int j = 0; j < object.model.get()->meshes.size(); j++) {
    std::vector<Vertex> vertices = object.model.get()->meshes.at(j).vertices;
    for (unsigned int i = 0; i < vertices.size()-3; i+=3) {
      vec3 v1 = glm::vec3(object.getModelMatrix() * glm::vec4(vertices.at(i+0).Position, 1.0));
      vec3 v2 = glm::vec3(object.getModelMatrix() * glm::vec4(vertices.at(i+1).Position, 1.0));
      vec3 v3 = glm::vec3(object.getModelMatrix() * glm::vec4(vertices.at(i+2).Position, 1.0));
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
}

  // } else {
  //     vec3 p = closestPointOnTriangle(spherePosition, v1, v2, v3);
  //     t = sphereRadius - glm::length(p - spherePosition);
  //     if ( t > 0 ) {
  //       intersect_values.push_back(t);
  //       normals.push_back(glm::normalize(spherePosition - p));
  //       positions.push_back(p);
  //     }    
  // }

CollisionInfo sphereObjectIntersect(glm::vec3 spherePosition, glm::vec3 sphereVelocity, float deltaTime, float sphereRadius, StaticObject object)
{   
  CollisionInfo c;
  c.depth = 0.0f;
  std::vector<float> intersect_values = {};
  std::vector<glm::vec3> normals = {};

  for (unsigned int j = 0; j < object.model.get()->meshes.size(); j++) {
    std::vector<Vertex> vertices = object.model.get()->meshes.at(j).vertices;

    for (unsigned int i = 0; i < vertices.size(); i+=3) {
      vec3 v1 = glm::vec3(object.getModelMatrix() * glm::vec4(vertices.at(i+0).Position, 1.0));
      vec3 v2 = glm::vec3(object.getModelMatrix() * glm::vec4(vertices.at(i+1).Position, 1.0));
      vec3 v3 = glm::vec3(object.getModelMatrix() * glm::vec4(vertices.at(i+2).Position, 1.0));
      vec3 normal = glm::normalize(glm::cross(v2-v1, v3-v1));
      float t = rayTriangleIntersect(spherePosition, glm::normalize(-sphereRadius * normal), v1, v2, v3);
      if (t) {
        intersect_values.push_back(t);
        normals.push_back(normal);
      }
    }
  }
  if (intersect_values.size()) {
    float shortest_distance = FLT_MAX;
    for (unsigned int i = 0; i < intersect_values.size(); i++) {
        if (intersect_values.at(i) < shortest_distance) {
          c.depth = intersect_values.at(i);
          c.normal = normals.at(i);
        }
    }
    return c;
  } else {
    return c;
  }
}
