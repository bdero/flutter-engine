// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_LIB_UI_PAINTING_ESOTERIC_IMAGE_GENERATOR_H_
#define FLUTTER_LIB_UI_PAINTING_ESOTERIC_IMAGE_GENERATOR_H_

#include "flutter/lib/ui/painting/image_generator.h"

namespace flutter {

class EsotericImageGenerator : public ImageGenerator {
 public:
  const int MAX_ITERATIONS = 100;
  const double ESCAPE_DISTANCE = 10;

  ~EsotericImageGenerator();

  EsotericImageGenerator(int width,
                         int height,
                         double ox,
                         double oy,
                         double px,
                         double py,
                         double zoom);

  // |ImageGenerator|
  const SkImageInfo& GetInfo() override;

  // |ImageGenerator|
  unsigned int GetFrameCount() const override;

  // |ImageGenerator|
  unsigned int GetPlayCount() const override;

  // |ImageGenerator|
  const ImageGenerator::FrameInfo GetFrameInfo(
      unsigned int frame_index) const override;

  // |ImageGenerator|
  SkISize GetScaledDimensions(float desired_scale) override;

  // |ImageGenerator|
  bool GetPixels(
      const SkImageInfo& info,
      void* pixels,
      size_t row_bytes,
      unsigned int frame_index = 0,
      std::optional<unsigned int> prior_frame = std::nullopt) override;

  static std::unique_ptr<ImageGenerator> MakeFromData(sk_sp<SkData> buffer);

 private:
  FML_DISALLOW_COPY_ASSIGN_AND_MOVE(EsotericImageGenerator);

  SkImageInfo info_;
  double ox_, oy_, px_, py_, zoom_;
};

}  // namespace flutter

#endif  // FLUTTER_LIB_UI_PAINTING_ESOTERIC_IMAGE_GENERATOR_H_
