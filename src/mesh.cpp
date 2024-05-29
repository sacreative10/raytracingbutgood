#include "mesh.h"

#include <tiny_obj_loader.h>
bool Triangle::hit(const Ray &r, float t_min, float t_max,
                   hitrecord &rec) const {
  const float EPSILON = 0.0001;

  Vec3 e1 = v1 - v0;
  Vec3 e2 = v2 - v0;

  Vec3 pvec = glm::cross(r.direction(), e2);
  float det = glm::dot(e1, pvec);

  if (abs(det) < EPSILON)
    return false;

  auto back_face = det < 0;

  float inv_det = 1.0 / det;

  Vec3 tvec = r.origin() - v0;
  float u = glm::dot(tvec, pvec) * inv_det;

  if (u < 0 || u > 1)
    return false;

  Vec3 qvec = glm::cross(tvec, e1);
  float v = glm::dot(r.direction(), qvec) * inv_det;

  if (v < 0 || u + v > 1)
    return false;

  float t = glm::dot(e2, qvec) * inv_det;

  if (t < t_min || t > t_max)
    return false;

  rec.uv = Vec2(u, v);
  rec.t = t;

  Vec3 outward_normal = glm::cross(e1, e2);
  if (back_face)
    outward_normal = -outward_normal;

  rec.normal = glm::normalize(outward_normal);
  rec.p = r.at(t);
  rec.mat_ptr = mat;

  return true;

  //  auto new_origin = applyTransform(globalToBarycentric, r.origin());
  //  auto new_direction = applyTransform(globalToBarycentric, r.direction());
  //  Ray new_ray(new_origin, new_direction);
  //
  //  auto t = -new_origin.z / new_direction.z;
  //
  //  //  if (t < t_min || t > t_max)
  //  //    return false;
  //
  //  auto b1 = new_origin.x + t * new_direction.x;
  //  auto b2 = new_origin.y + t * new_direction.y;
  //
  //  if (!(interval(0, 1).contains(b1) && interval(0, 1).contains(b2) &&
  //        b2 + b1 <= 1))
  //    return false;
  //
  //  rec.t = t;
  //  rec.uv = Vec2(b1, b2);
  //  rec.mat_ptr = mat;
  //  rec.p = r.at(t);
  //  Vec3 e1 = v1 - v0;
  //  Vec3 e2 = v2 - v0;
  //  rec.normal = glm::normalize(glm::cross(e1, e2));
  //
  //  return true;
}
bool Mesh::hit(const Ray &r, float t_min, float t_max, hitrecord &rec) const {
  hitrecord temp_rec;
  bool hit_anything = false;
  float closest_so_far = t_max;

  for (const auto &triangle : triangles) {
    if (triangle->hit(r, t_min, closest_so_far, temp_rec)) {
      hit_anything = true;
      closest_so_far = temp_rec.t;
      rec = temp_rec;
      if (rec.mat_ptr == nullptr)
        rec.mat_ptr = mat;
      // TODO: Add UVs
    }
  }

  return hit_anything;

  // we will be using octree for mesh
}

#include "bvh_node.h"
#include "objLoader.h"

Mesh::Mesh(const std::vector<Vec3> &vertices_, const std::vector<Vec3> &normals,
           const std::vector<Vec2> &uvs, const std::vector<int> &indices,
           shared_ptr<material> mat, Transform transform)
    : vertices(vertices_), normals(normals), uvs(uvs), indices(indices),
      mat(std::move(mat)) {
  transform_mesh(transform);
  for (int i = 0; i < indices.size(); i += 3) {
    auto v0 = vertices[indices[i]];
    auto v1 = vertices[indices[i + 1]];
    auto v2 = vertices[indices[i + 2]];
    triangles.push_back(std::make_shared<Triangle>(v0, v1, v2, mat));
  }

  set_bounding_box();
}

Mesh::Mesh(const char *filename, shared_ptr<material> mat,
           Transform transform) {
  //  objl::Loader loader;
  //
  //  bool loadobj = loader.LoadFile(filename);
  //
  //  if (!loadobj)
  //    std::cerr << "Could not load obj file" << std::endl;
  //
  //  for (int i = 0; i < loader.LoadedMeshes.size(); i++) {
  //    objl::Mesh currentMesh = loader.LoadedMeshes[i];
  //    vertices.reserve(currentMesh.Vertices.size());
  //    normals.reserve(currentMesh.Vertices.size());
  //    uvs.reserve(currentMesh.Vertices.size());
  //    for (int j = 0; j < currentMesh.Vertices.size(); j++) {
  //      vertices.push_back(
  //          applyTransform(transform, Vec3(currentMesh.Vertices[j].Position.X,
  //                                         currentMesh.Vertices[j].Position.Y,
  //                                         currentMesh.Vertices[j].Position.Z)));
  //      normals.push_back(Vec3(currentMesh.Vertices[j].Normal.X,
  //                             currentMesh.Vertices[j].Normal.Y,
  //                             currentMesh.Vertices[j].Normal.Z));
  //      uvs.push_back(Vec2(currentMesh.Vertices[j].TextureCoordinate.X,
  //                         currentMesh.Vertices[j].TextureCoordinate.Y));
  //    }
  //
  //    for (int j = 0; j < currentMesh.Indices.size(); j += 3) {
  //      indices.push_back(currentMesh.Indices[j]);
  //      indices.push_back(currentMesh.Indices[j + 1]);
  //      indices.push_back(currentMesh.Indices[j + 2]);
  //    }
  //    // TODO: Materials
  //  }
  //
  //  transform_mesh(transform);
  //
  //  for (int i = 0; i < indices.size(); i += 3) {
  //    auto v0 = vertices[indices[i]];
  //    auto v1 = vertices[indices[i + 1]];
  //    auto v2 = vertices[indices[i + 2]];
  //    triangles.push_back(std::make_shared<Triangle>(v0, v1, v2, mat));
  //  }
  //

  tinyobj::ObjReader reader;
  tinyobj::ObjReaderConfig reader_config;

  if (!reader.ParseFromFile(filename, reader_config)) {
    if (!reader.Error().empty()) {
      std::cerr << "TinyObjReader: " << reader.Error();
    }
    exit(1);
  }

  if (!reader.Warning().empty())
    std::cerr << "TinyObjReader: " << reader.Warning();

  auto &attrib = reader.GetAttrib();
  auto &shapes = reader.GetShapes();
  auto &materials = reader.GetMaterials();

  for (size_t s = 0; s < shapes.size(); s++) {
    size_t index_offset = 0;
    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

      for (size_t v = 0; v < fv; v++) {
        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
        tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
        tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
        tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
        vertices.push_back(applyTransform(transform, Vec3(vx, vy, vz)));

        if (idx.normal_index >= 0) {
          tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
          tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
          tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
          normals.push_back(Vec3(nx, ny, nz));
        }
        if (idx.texcoord_index >= 0) {
          tinyobj::real_t tx =
              attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
          tinyobj::real_t ty =
              attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
          uvs.push_back(Vec2(tx, ty));
        }
      }
      index_offset += fv;
      shapes[s].mesh.material_ids[f];
    }
  }

  for (int i = 0; i < vertices.size(); i += 3) {
    auto v0 = vertices[i];
    auto v1 = vertices[i + 1];
    auto v2 = vertices[i + 2];
    triangles.push_back(std::make_shared<Triangle>(v0, v1, v2, mat));
  }

  set_bounding_box();
}
void Mesh::transform_mesh(const Transform &transform) {
  for (auto &vertex : vertices) {
    vertex = applyTransform(transform, vertex);
  }
}
