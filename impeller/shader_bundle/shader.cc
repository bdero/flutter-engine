// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "impeller/shader_bundle/shader.h"

namespace impeller {

Shader::Shader() = default;

Shader::Shader(const fb::Shader* shader)
    : runtime_stage_(
          std::make_shared<RuntimeStage>(RuntimeStage(shader->shader()))) {
  is_valid_ = runtime_stage_->IsValid();
}

bool Shader::IsValid() const {
  return is_valid_;
}

}  // namespace impeller
