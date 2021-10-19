#ifndef STATIC_MODEL_H
#define STATIC_MODEL_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <memory>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "static_mesh.h"
#include "static_shader.h"
#include "camera.h"

using namespace std;

unsigned int TextureFromFile(const char *path, const string &directory);
unsigned int TextureFromData(unsigned int width, unsigned int height, std::vector<glm::vec3> data);

/// @brief initiliazation function for heap allocation and similar
/// @param vertexPath the filepath of the vertex shader
/// @param fragmentPath the filepath of the fragment shader
/// @todo fix segmentation fault when creating StaticModel's on the stack (only "new StaticModel" works)
class StaticModel
{
public:

    StaticModel(string path);
    StaticModel(){};

    bool operator==(StaticModel const& rhs) const {
        return *this == rhs;
    }

    /// @brief draws the model, and thus all its meshes
    /// @param shader a StaticShader instance
    virtual int Draw();
    void DrawInstanced(std::vector<mat4> modelTransforms);
    vector<StaticMesh> meshes;
    /// @brief loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    int loadModel(string path);

    virtual std::vector<float> getAABB();

    // abstract callbacks (can be overridden, no error if not though)
    //virtual void onKeyPressed(int key, int scancode, int action, int mods){};
    //virtual void update(){};


public:

    /// @brief processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    /// @param node an aiNode pointer
    /// @param scene an aiScene pointer
    void processNode(aiNode *node, const aiScene *scene);

    StaticMesh processMesh(aiMesh *mesh, const aiScene *scene);

    /// @brief checks all material textures of a given type and loads the textures if they're not loaded yet. the required info is returned as a Texture struct.
    /// @param scene an aiMaterial pointer
    /// @param type an aiTextureType instance
    /// @param typeName a string of the texture type
    virtual vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);

    /*  Model Data */
    /// @brief // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Texture> textures_loaded;	
    string directory;
    bool gammaCorrection;
    /// @todo make shaders static? Every instance uses the same shader anyway
    /// and it would minimize state changes if the instances all shared one shader
    /// consider custom shader accessibility for adding special effects like taking damage, special powers, etc...
    static StaticShader shader;


    mat4 model;
    bool isInitialized = false;
    std::string filePath;
};


class StaticObject {

public:
    // interface
    StaticObject(const std::string &filePath);

    int remove();
    mat4 modelMatrix = mat4(1.0f);
    int setModelMatrix(mat4 m);
    mat4 getModelMatrix();
    void setToDraw(int b);
    int toDraw = 1;
    int renderToUI = 0;
    
    vec3 getColor();
    int setColor(vec3 c);

    vec3 color = vec3(0.0, 0.0, 0.0);

    // remove this access func asap
    StaticModel getModel();

    StaticObject(){}; // necessary for derived class constructor
    static int drawAllObjects(Camera &active_camera, StaticShader &shader);
    static vector<vector<StaticObject>> modelRegistry;
    std::shared_ptr<StaticModel> model;
    std::vector<unsigned short> meshIsCulled = {};
    std::vector<unsigned short> meshIsOccluded = {};
    std::vector<unsigned int> meshOcclusionQueries = {};

    unsigned int modelID;
    unsigned int uniqueID;
    std::string filePath;
    static unsigned int uniqueObjectCount;
private:

    static vector<mat4> getObjectTransforms(const vector<StaticObject> &objectStore);
};


#endif


