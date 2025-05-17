#pragma once
#include "renderer.h"

typedef struct {
  VectorVec3 vertices;
  VectorVec3 vertexNormals;
  VectorVec2 vertexTextures;
  VectorVecI3 faces;
} Model;

Model InitModel(String path);
Vec3 GetVertModel(Model *model, size_t faceIndex, size_t vertIndex);
Vec2 GetVertTextModel(Model *model, size_t faceIndex, size_t vertTextIndex);
Vec3 GetVertNormModel(Model *model, size_t faceIndex, size_t vertNormIndex);
