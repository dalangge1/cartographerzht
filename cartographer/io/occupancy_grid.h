/*
 * Copyright 2017 The Cartographer Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CARTOGRAPHER_IO_OCCUPANCY_GRID_H_
#define CARTOGRAPHER_IO_OCCUPANCY_GRID_H_

#include "Eigen/Geometry"
#include "cairo/cairo.h"
#include "cartographer/io/image.h"
#include "cartographer/mapping/id.h"
#include "cartographer/transform/rigid_transform.h"

namespace cartographer {
namespace io {

constexpr cairo_format_t kCairoFormat = CAIRO_FORMAT_ARGB32;

struct OccupancyGridState {
  OccupancyGridState(::cartographer::io::UniqueCairoSurfacePtr surface,
                     Eigen::Array2f origin, Eigen::Array2i size)
      : surface(std::move(surface)), origin(origin), size(size) {}
  ::cartographer::io::UniqueCairoSurfacePtr surface;
  Eigen::Array2f origin;
  Eigen::Array2i size;
};

struct SubmapState {
  SubmapState()
      : surface(::cartographer::io::MakeUniqueCairoSurfacePtr(nullptr)) {}

  // Texture data.
  int width;
  int height;
  int version;
  double resolution;
  ::cartographer::transform::Rigid3d slice_pose;
  ::cartographer::io::UniqueCairoSurfacePtr surface;
  // Pixel data used by 'surface'. Must outlive 'surface'.
  std::vector<uint32_t> cairo_data;

  // Metadata.
  ::cartographer::transform::Rigid3d pose;
  int metadata_version = -1;
};

void CairoDrawEachSubmap(
    const double scale,
    std::map<::cartographer::mapping::SubmapId, SubmapState>* submaps,
    cairo_t* cr, std::function<void(const SubmapState&)> draw_callback);

OccupancyGridState DrawOccupancyGrid(
    std::map<::cartographer::mapping::SubmapId, SubmapState>* submaps,
    const double resolution);

}  // namespace io
}  // namespace cartographer

#endif  // CARTOGRAPHER_IO_OCCUPANCY_GRID_H_
