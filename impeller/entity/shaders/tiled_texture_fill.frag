// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <impeller/texture.glsl>

uniform sampler2D texture_sampler;

uniform FragInfo {
  float texture_sampler_y_coord_scale;
  vec2 half_texel;
  float x_tile_mode;
  float y_tile_mode;
  float alpha;
}
frag_info;

in vec2 v_texture_coords;

out vec4 frag_color;

void main() {
  frag_color =
      frag_info.alpha *
      IPSampleLinearWithTileMode(
          texture_sampler,                          // sampler
          v_texture_coords,                         // texture coordinates
          frag_info.texture_sampler_y_coord_scale,  // y coordinate scale
          frag_info.half_texel,                     // half texel
          frag_info.x_tile_mode,                    // x tile mode
          frag_info.y_tile_mode                     // y tile mode
      ) * frag_info.alpha;
}
