//
// Created by sohaibalam on 26/05/24.
//

#ifndef RAYTRACINGBUTGOOD_MESHLOADER_H
#define RAYTRACINGBUTGOOD_MESHLOADER_H

#include "mesh.h"

#include <fstream>
#include <iostream>
#include <sstream>

// assimp imports
#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

std::pair<std::vector<Vertex>, std::vector<int>>
loadMesh(const std::string &filename) {
  Assimp::Importer importer;
  const aiScene *scene = importer.ReadFile(filename, aiProcess_Triangulate);

  std::vector<Vertex> vertices;
  std::vector<int> indices;

  if (!scene && !scene->HasMeshes()) {
    std::cerr << "Error loading mesh file: " << filename << std::endl;
  }

  aiMesh *mesh = scene->mMeshes[0];
  vertices.reserve(mesh->mNumVertices);
  indices.reserve(mesh->mNumFaces * 3);

  for (size_t i = 0; i < mesh->mNumVertices; i++) {
    aiVector3D pos = mesh->mVertices[i];
    aiVector3D normal = mesh->mNormals[i];
    aiVector3D texCoord = aiVector3D(0.0f, 0.0f, 0.0f);
    if (mesh->HasTextureCoords(0)) {
      texCoord = mesh->mTextureCoords[0][i];
    }

    vertices.push_back(Vertex(Vec3(pos.x, pos.y, pos.z),
                              Vec3(normal.x, normal.y, normal.z),
                              Vec2(texCoord.x, texCoord.y)));
  }

  for (size_t i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (size_t j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }

  return {vertices, indices};
}

#endif // RAYTRACINGBUTGOOD_MESHLOADER_H
