/*
 * Copyright 2016 The Cartographer Authors
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

#ifndef CARTOGRAPHER_MAPPING_3D_SCAN_MATCHING_PRECOMPUTATION_GRID_H_
#define CARTOGRAPHER_MAPPING_3D_SCAN_MATCHING_PRECOMPUTATION_GRID_H_

#include "cartographer/mapping_3d/hybrid_grid.h"

namespace cartographer {
namespace mapping_3d {
namespace scan_matching {

class PrecomputationGrid : public mapping::HybridGridBase<uint8> {
 public:
  explicit PrecomputationGrid(const float resolution)
      : mapping::HybridGridBase<uint8>(resolution) {}

  // Maps values from [0, 255] to [kMinProbability, kMaxProbability].
  static float ToProbability(float value) {
    return mapping::kMinProbability +
           value *
               ((mapping::kMaxProbability - mapping::kMinProbability) / 255.f);
  }
};

// Converts a HybridGrid to a PrecomputationGrid representing the same data,
// but only using 8 bit instead of 2 x 16 bit.
PrecomputationGrid ConvertToPrecomputationGrid(
    const mapping::HybridGrid& hybrid_grid);

// Returns a grid of the same resolution containing the maximum value of
// original voxels in 'grid'. This maximum is over the 8 voxels that have
// any combination of index components optionally increased by 'shift'.
// If 'shift' is 2 ** (depth - 1), where depth 0 is the original grid, and this
// is using the precomputed grid of one depth before, this results in
// precomputation grids analogous to the 2D case.
PrecomputationGrid PrecomputeGrid(const PrecomputationGrid& grid,
                                  bool half_resolution,
                                  const Eigen::Array3i& shift);

}  // namespace scan_matching
}  // namespace mapping_3d
}  // namespace cartographer

#endif  // CARTOGRAPHER_MAPPING_3D_SCAN_MATCHING_PRECOMPUTATION_GRID_H_
