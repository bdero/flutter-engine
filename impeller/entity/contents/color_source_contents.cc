// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "impeller/entity/contents/color_source_contents.h"

#include "impeller/entity/contents/color_source_vertices.h"
#include "impeller/entity/entity.h"
#include "impeller/renderer/render_pass.h"

namespace impeller {

ColorSourceContents::ColorSourceContents() = default;

ColorSourceContents::~ColorSourceContents() = default;

// |Contents|
std::optional<Rect> ColorSourceContents::GetCoverage(
    const Entity& entity) const {
  if (!vertices_.has_value()) {
    return std::nullopt;
  }

  auto bounds = vertices_->GetBounds();
  if (!bounds.has_value()) {
    return std::nullopt;
  }

  return bounds->TransformBounds(entity.GetTransformation());
}

// |Contents|
bool ColorSourceContents::Render(const ContentContext& renderer,
                                 const Entity& entity,
                                 RenderPass& pass) const {
  // Paint the whole screen if no vertex data has been assigned.
  ColorSourceVertices rect_vertices;
  if (!vertices_.has_value()) {
    rect_vertices = ColorSourceVertices::MakeFromRect(
        Rect::MakeSize(Size(pass.GetRenderTargetSize())));
  }

  const ColorSourceVertices& vertices =
      vertices_.has_value() ? vertices_.value() : rect_vertices;
}

void ColorSourceContents::SetVertices(
    std::optional<ColorSourceVertices> vertices) {
  vertices_ = std::move(vertices);
}

}  // namespace impeller
