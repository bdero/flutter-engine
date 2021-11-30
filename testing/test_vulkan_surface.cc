// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/testing/test_vulkan_surface.h"
#include <memory>
#include "flutter/fml/logging.h"
#include "flutter/testing/test_vulkan_context.h"

#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/core/SkSurfaceProps.h"

namespace flutter {
namespace testing {

TestVulkanSurface::TestVulkanSurface() = default;

std::unique_ptr<TestVulkanSurface> TestVulkanSurface::Create(
    const TestVulkanContext& context,
    const SkISize& surface_size) {
  VkImage image = context.CreateImage(surface_size);

  if (!image) {
    FML_LOG(ERROR) << "Could not create VkImage.";
    return nullptr;
  }

  GrVkImageInfo image_info = {
      .fImage = static_cast<VkImage>(image),
      .fImageTiling = VK_IMAGE_TILING_OPTIMAL,
      .fImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .fFormat = VK_FORMAT_R8G8B8A8_UNORM,
      .fImageUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                          VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                          VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                          VK_IMAGE_USAGE_SAMPLED_BIT,
      .fSampleCount = 1,
      .fLevelCount = 1,
  };
  GrBackendTexture backend_texture(surface_size.width(),   //
                                   surface_size.height(),  //
                                   image_info              //
  );

  SkSurfaceProps surface_properties(0, kUnknown_SkPixelGeometry);

  auto result = std::unique_ptr<TestVulkanSurface>(new TestVulkanSurface());

  result->surface_ = SkSurface::MakeFromBackendTexture(
      context.GetGrDirectContext().get(),  // context
      backend_texture,                     // back-end texture
      kTopLeft_GrSurfaceOrigin,            // surface origin
      1,                                   // sample count
      kRGBA_8888_SkColorType,              // color type
      SkColorSpace::MakeSRGB(),            // color space
      &surface_properties,                 // surface properties
      nullptr,                             // release proc
      nullptr                              // release context
  );

  if (!result->surface_) {
    FML_LOG(ERROR)
        << "Could not wrap VkImage as an SkSurface Vulkan render texture.";
    return nullptr;
  }

  return result;
}

bool TestVulkanSurface::IsValid() const {
  return surface_ != nullptr;
}

sk_sp<SkImage> TestVulkanSurface::GetSurfaceSnapshot() const {
  if (!IsValid()) {
    return nullptr;
  }

  if (!surface_) {
    FML_LOG(ERROR) << "Aborting snapshot because of on-screen surface "
                      "acquisition failure.";
    return nullptr;
  }

  auto device_snapshot = surface_->makeImageSnapshot();

  if (!device_snapshot) {
    FML_LOG(ERROR) << "Could not create the device snapshot while attempting "
                      "to snapshot the Vulkan surface.";
    return nullptr;
  }

  auto host_snapshot = device_snapshot->makeRasterImage();

  if (!host_snapshot) {
    FML_LOG(ERROR) << "Could not create the host snapshot while attempting to "
                      "snapshot the Vulkan surface.";
    return nullptr;
  }

  return host_snapshot;
}

}  // namespace testing
}  // namespace flutter
