// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "impeller/entity/contents/color_source_vertices.h"

#include <optional>
#include <utility>

#include "impeller/tessellator/tessellator.h"

namespace impeller {

ColorSourceVertices ColorSourceVertices::MakeFromPathFill(
    Path path,
    bool generate_texture_coordinates) {
  ColorSourceVertices vertices;
  vertices.primitive_type = PrimitiveType::kTriangle;

  Tessellator{}.Tessellate(
      path.GetFillType(), path.CreatePolyline(), [&vertices](Point point) {
        vertices.positions.push_back(point);
        vertices.bounds_ = vertices.bounds_.has_value()
                               ? vertices.bounds_->Union(point)
                               : Rect();
      });

  return vertices;
}

std::optional<Rect> ColorSourceVertices::GetBounds() const {
  if (bounds_.has_value()) {
    return bounds_;
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

void ColorSourceVertices::GenerateTextureCoordinates() {
  auto bounds = GetBounds();
  if (!bounds.has_value()) {
    return;
  }

  texture_coordinates = std::vector<Point>();

  for (const auto& position : positions) {
    texture_coordinates->push_back((position - bounds->origin) / bounds->size);
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
