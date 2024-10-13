#include "mesh.h"

#include<assimp/cimport.h>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

#include "../external/assimp/contrib/rapidjson/include/rapidjson/error/error.h"
#include "assimp/Importer.hpp"


struct MeshData
{
    std::vector<glm::vec3> vertices;
    std::vector<int> indices;
    std::vector<glm::vec3> normals;
};


Mesh::Mesh(const std::string& filename, std::shared_ptr<material> mat, Transform modelMatrix, bool useOctree)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        throw std::runtime_error("Failed to load model");
    }

    aiMesh* mesh = scene->mMeshes[0];

    MeshData meshData;
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        meshData.vertices.push_back(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));

        if (mesh->HasNormals())
        {
            meshData.normals.push_back(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
        }

        // TODO: Texture coords
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            meshData.indices.push_back(face.mIndices[j]);
        }
    }

    for (int i = 0; i < meshData.indices.size(); i += 3)
    {
        int index = meshData.indices[i];
        Vec3 v0 = meshData.vertices[index];
        Vec3 v1 = meshData.vertices[index + 1];
        Vec3 v2 = meshData.vertices[index + 2];

        v0 = glm::vec3(modelMatrix * glm::vec4(v0, 1.0f));
        v1 = glm::vec3(modelMatrix * glm::vec4(v1, 1.0f));
        v2 = glm::vec3(modelMatrix * glm::vec4(v2, 1.0f));

        auto normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
        triangles.push_back(std::make_shared<Triangle>(v0, v1, v2, mat, normal));
        list.add(triangles.back());
    }

    calculate_bounding_box(meshData.vertices, modelMatrix);

    if (useOctree)
    {
        octree = new Octree(bbox);
        octree->build(triangles);
        this->useOctree = true;
    } else
    {
        bvh = std::make_unique<bvh_node>(list.objects, 0, list.objects.size());
    }
}


bool Mesh::hit(const Ray& r, float t_min, float t_max, hitrecord& rec) const
{
    if (useOctree)
    {
        return octree->hit(r, t_min, t_max, rec);
    } else
    {
        return bvh->hit(r, t_min, t_max, rec);
    }
}


void Mesh::calculate_bounding_box(const std::vector<glm::vec3>& vertices, Transform modelMatrix)
{
    // iterate through all of the triangle's bounding boxes and calculate the mesh's bounding box
    Vec3 min = Vec3(std::numeric_limits<float>::infinity());
    Vec3 max = Vec3(-std::numeric_limits<float>::infinity());

    for (const auto& vertex : vertices)
    {
        Vec3 transformed_vertex = modelMatrix * glm::vec4(vertex, 1.0f);

        min.x = std::min(min.x, transformed_vertex.x);
        min.y = std::min(min.y, transformed_vertex.y);
        min.z = std::min(min.z, transformed_vertex.z);

        max.x = std::max(max.x, transformed_vertex.x);
        max.y = std::max(max.y, transformed_vertex.y);
        max.z = std::max(max.z, transformed_vertex.z);
    }


    bbox = aabb(min, max);
}



