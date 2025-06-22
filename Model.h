#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <json/json.h>
#include "Mesh.h"

using json = nlohmann::json;

class Model
{
public:
    // Constructor para modelo sin transformación inicial
    Model(const char* file);

    // Constructor que incluye una transformación inicial
    Model(const char* file, glm::mat4 transform);

    // Dibuja el modelo usando su transformación interna
    void Draw(Shader& shader, Camera& camera);

    // Dibuja el modelo usando una transformación externa
    void Draw(Shader& shader, Camera& camera, const glm::mat4& externalTransform);

    // Permite obtener y establecer la transformación del modelo
    const glm::mat4& getTransform() const { return modelTransform; }
    void setTransform(const glm::mat4& transform) { modelTransform = transform; }

private:
    const char* file;
    std::vector<unsigned char> data;
    json JSON;

    // Almacena las mallas y transformaciones
    std::vector<Mesh> meshes;
    std::vector<glm::vec3> translationsMeshes;
    std::vector<glm::quat> rotationsMeshes;
    std::vector<glm::vec3> scalesMeshes;
    std::vector<glm::mat4> matricesMeshes;

    // Texturas ya cargadas
    std::vector<std::string> loadedTexName;
    std::vector<Texture> loadedTex;

    // Transformación del modelo
    glm::mat4 modelTransform = glm::mat4(1.0f);

    // Métodos privados para cargar y procesar el modelo
    void loadMesh(unsigned int indMesh);
    void traverseNode(unsigned int nextNode, glm::mat4 matrix = glm::mat4(1.0f));
    std::vector<unsigned char> getData();
    std::vector<float> getFloats(json accessor);
    std::vector<GLuint> getIndices(json accessor);
    std::vector<Texture> getTextures();

    std::vector<Vertex> assembleVertices(
        std::vector<glm::vec3> positions,
        std::vector<glm::vec3> normals,
        std::vector<glm::vec2> texUVs
    );

    std::vector<glm::vec2> groupFloatsVec2(std::vector<float> floatVec);
    std::vector<glm::vec3> groupFloatsVec3(std::vector<float> floatVec);
    std::vector<glm::vec4> groupFloatsVec4(std::vector<float> floatVec);
};

#endif
