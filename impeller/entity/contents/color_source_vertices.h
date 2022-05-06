// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <optional>
#include <vector>

#include "impeller/geometry/color.h"
#include "impeller/geometry/path.h"
#include "impeller/geometry/point.h"
#include "impeller/geometry/rect.h"
#include "impeller/renderer/formats.h"

namespace impeller {

struct ColorSourceVertices {
  struct PosTexCol {
    Point position;
    Point texture_coordinate;
    Color color;
  };

  struct PosTex {
    Point position;
    Point texture_coordinate;
  };

  PrimitiveType primitive_type = PrimitiveType::kTriangle;
  std::vector<Point> positions;
  std::optional<std::vector<Point>> texture_coordinates;
  std::optional<std::vector<Color>> colors;
  std::optional<std::vector<uint16_t>> indices;

  static ColorSourceVertices MakeFromRect(Rect rect);

  static ColorSourceVertices MakeFromPathFill(Path path);

  std::optional<Rect> GetBounds() const;

  bool IsValid() const;

  void SetNormalizedTextureCoordinates();

  void SetCountingIndices();

  std::optional<std::vector<PosTexCol>> GetPosTexCol();

  std::optional<std::vector<PosTex>> GetPosTex();

 private:
  mutable std::optional<Rect> bounds_;
};

}  // namespace impeller
