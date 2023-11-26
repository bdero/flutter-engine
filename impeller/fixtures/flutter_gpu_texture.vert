// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

uniform mat4 mvp;
uniform vec2 texture_coords;

in vec2 position;
in vec4 color;
out vec2 v_texture_coords;
out vec4 v_color;

void main() {
  v_texture_coords = texture_coords;
  v_color = color;
  gl_Position = mvp * vec4(position, 0.0, 1.0);
}