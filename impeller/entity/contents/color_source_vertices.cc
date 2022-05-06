// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "impeller/entity/contents/color_source_vertices.h"

#include <optional>
#include <utility>
#include <vector>

#include "impeller/tessellator/tessellator.h"

namespace impeller {

ColorSourceVertices ColorSourceVertices::MakeFromRect(Rect rect) {
  ColorSourceVertices vertices;
  vertices.primitive_type = PrimitiveType::kTriangleStrip;

  auto r = rect.GetLTRB();
  vertices.positions = {{r[0], r[1]}, {r[0], r[3]}, {r[2], r[1]}, {r[2], r[3]}};
  vertices.indices = {0, 1, 2, 3};

  return vertices;
}

ColorSourceVertices ColorSourceVertices::MakeFromPathFill(Path path) {
  ColorSourceVertices vertices;
  vertices.primitive_type = PrimitiveType::kTriangle;

  Tessellator{}.Tessellate(
      path.GetFillType(), path.CreatePolyline(), [&vertices](Point point) {
        vertices.positions.push_back(point);
        vertices.bounds_ = vertices.bounds_.has_value()
                               ? vertices.bounds_->Union(point)
                               : Rect();
      });
  vertices.SetCountingIndices();

  return vertices;
}

std::optional<Rect> ColorSourceVertices::GetBounds() const {
  if (bounds_.has_value()) {
    return bounds_;
  }

  if (positions.empty()) {
    return std::nullopt;
  }

  for (const auto& position : positions) {
    bounds_ = bounds_.has_value() ? bounds_->Union(position) : Rect();
  }

  return bounds_;
}

bool ColorSourceVertices::IsValid() const {
  if (!colors->empty() && colors->size() != positions.size()) {
    return false;
  }

  if (!texture_coordinates->empty() &&
      texture_coordinates->size() != positions.size()) {
    return false;
  }

  return true;
}

void ColorSourceVertices::SetNormalizedTextureCoordinates() {
  auto bounds = GetBounds();
  if (!bounds.has_value()) {
    return;
  }

  texture_coordinates = std::vector<Point>();

  for (const auto& position : positions) {
    texture_coordinates->push_back((position - bounds->origin) / bounds->size);
  }
}

void ColorSourceVertices::SetCountingIndices() {
  if (!indices.has_value()) {
    indices = std::vector<uint16_t>();
  }
  indices->clear();
  indices->reserve(positions.size());
  for (size_t i = 0; i < positions.size(); i++) {
    indices->push_back(i);
  }
}

std::optional<std::vector<ColorSourceVertices::PosTexCol>>
ColorSourceVertices::GetPosTexCol() {
  if (!IsValid()) {
    return std::nullopt;
  }

  if (!texture_coordinates.has_value() || !colors.has_value()) {
    return std::nullopt;
  }

  std::vector<PosTexCol> result;
  for (size_t i = 0; i < positions.size(); i++) {
    result.push_back(
        {positions[i], texture_coordinates.value()[i], colors.value()[i]});
  }
  return result;
}

std::optional<std::vector<ColorSourceVertices::PosTex>>
ColorSourceVertices::GetPosTex() {
  if (!IsValid()) {
    return std::nullopt;
  }

  if (!texture_coordinates.has_value()) {
    return std::nullopt;
  }

  std::vector<PosTex> result;
  for (size_t i = 0; i < positions.size(); i++) {
    result.push_back({positions[i], texture_coordinates.value()[i]});
  }
  return result;
}

}  // namespace impeller
