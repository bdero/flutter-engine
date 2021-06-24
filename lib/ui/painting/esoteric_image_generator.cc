#include "flutter/lib/ui/painting/esoteric_image_generator.h"

namespace flutter {

EsotericImageGenerator::~EsotericImageGenerator() = default;

EsotericImageGenerator::EsotericImageGenerator(int width,
                                               int height,
                                               double ox,
                                               double oy,
                                               double px,
                                               double py,
                                               double zoom)
    : info_(SkImageInfo::MakeS32(width,
                                 height,
                                 SkAlphaType::kOpaque_SkAlphaType)),
      ox_(ox),
      oy_(oy),
      px_(ox),
      py_(py),
      zoom_(zoom) {}

const SkImageInfo& EsotericImageGenerator::GetInfo() {
  return info_;
}

unsigned int EsotericImageGenerator::GetFrameCount() const {
  return 1;
}

unsigned int EsotericImageGenerator::GetPlayCount() const {
  return 1;
}

const ImageGenerator::FrameInfo EsotericImageGenerator::GetFrameInfo(
    unsigned int frame_index) const {
  return {.required_frame = std::nullopt,
          .duration = 0,
          .disposal_method = SkCodecAnimation::DisposalMethod::kKeep};
}

SkISize EsotericImageGenerator::GetScaledDimensions(float desired_scale) {
  SkISize s = info_.dimensions();
  return SkISize::Make(static_cast<uint32_t>(s.width() * desired_scale),
                       static_cast<uint32_t>(s.height() * desired_scale));
}

bool EsotericImageGenerator::GetPixels(
    const SkImageInfo& info,
    void* pixels,
    size_t row_bytes,
    unsigned int frame_index,
    std::optional<unsigned int> prior_frame) {
  auto px = static_cast<uint32_t*>(pixels);

  for (int i = 0; i < info.width() * info.height(); i++) {
    double u = static_cast<double>(i % info.width()) / info.width();
    double v = static_cast<double>(i) / info.width() / info.height();
    double x = (u - 0.5) / zoom_ + px_;
    double y = (v - 0.5) / zoom_ + py_;

    double re = x, im = y;
    double escape_time = 0;
    for (int j = 0; j < MAX_ITERATIONS; j++) {
      double tmp = re * re - im * im + ox_;
      im = 2.0 * re * im + oy_;
      re = tmp;

      if (re * re + im * im > ESCAPE_DISTANCE) {
        escape_time = static_cast<double>(j + 1) / MAX_ITERATIONS;
        break;
      }
    }
    if (escape_time == 0) {
      px[i] = 0xff;
      continue;
    }

    uint8_t r = static_cast<char>((cos(escape_time * M_PI) + 1.0) / 2.0 * 0xff);
    uint8_t g =
        static_cast<char>((-cos(escape_time * M_PI) + 1.0) / 2.0 * 0xff);
    uint8_t b = static_cast<char>(sin(escape_time * M_PI) * 0xff);
    px[i] = (static_cast<uint32_t>(r * 255) << 24) +
            (static_cast<uint32_t>(g * 255) << 16) +
            (static_cast<uint32_t>(b * 255) << 8) + 0xff;
  }

  return true;
}

std::unique_ptr<ImageGenerator> EsotericImageGenerator::MakeFromData(
    sk_sp<SkData> buffer) {
  if (buffer->size() < 10)
    return nullptr;

  // const char* input = "julia;-0.73;0.45;0.46;-0.22;0.76";
  const char* data = static_cast<const char*>(buffer.get()->data());
  if (strncmp(data, "julia;", 6) != 0)
    return nullptr;

  std::string s(data, data + buffer->size());
  std::vector<double> inputs;

  size_t last = 0;
  size_t next = 0;
  while ((next = s.find(';', last)) != std::string::npos) {
    if (last > 0) {
      inputs.push_back(std::stod(s.substr(last, next - last)));
    }
    last = next + 1;
  }
  inputs.push_back(std::stod(s.substr(last)));
  if (inputs.size() < 5)
    return nullptr;

  return std::make_unique<EsotericImageGenerator>(
      800, 600, inputs[0], inputs[1], inputs[2], inputs[3], inputs[4]);
}

}  // namespace flutter
