/*
 * Copyright 2018 The Cartographer Authors
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
#include "cartographer/mapping/2d/grid_2d.h"

#include "cartographer/mapping/probability_values.h"

namespace cartographer {
namespace mapping {

Grid2D::Grid2D(const MapLimits& limits)
    : limits_(limits),
      correspondence_cost_cells_(
          limits_.cell_limits().num_x_cells * limits_.cell_limits().num_y_cells,
          kUnknownCorrespondenceValue) {}

Grid2D::Grid2D(const proto::Grid2D& proto) : Grid2D(MapLimits(proto.limits())) {
  if (proto.has_known_cells_box()) {
    const auto& box = proto.known_cells_box();
    known_cells_box_ =
        Eigen::AlignedBox2i(Eigen::Vector2i(box.min_x(), box.min_y()),
                            Eigen::Vector2i(box.max_x(), box.max_y()));
  }
  correspondence_cost_cells_.reserve(proto.cells_size());
  for (const auto& cell : proto.cells()) {
    CHECK_LE(cell, std::numeric_limits<uint16>::max());
    correspondence_cost_cells_.push_back(cell);
  }
}

// Finishes the update sequence.
void Grid2D::FinishUpdate() {
  while (!update_indices_.empty()) {
    DCHECK_GE(correspondence_cost_cells_[update_indices_.back()],
              kUpdateMarker);
    correspondence_cost_cells_[update_indices_.back()] -= kUpdateMarker;
    update_indices_.pop_back();
  }
}

// Sets the probability of the cell at 'cell_index' to the given
// 'probability'. Only allowed if the cell was unknown before.
void Grid2D::SetCorrespondenceCost(const Eigen::Array2i& cell_index,
                                   const float correspondence_cost) {
  LOG(FATAL) << "Not implemented";
}

// Returns the probability of the cell with 'cell_index'.
float Grid2D::GetCorrespondenceCost(const Eigen::Array2i& cell_index) const {
  LOG(FATAL) << "Not implemented";
  return kUnknownCorrespondenceValue;
}

// Returns true if the probability at the specified index is known.
bool Grid2D::IsKnown(const Eigen::Array2i& cell_index) const {
  return limits_.Contains(cell_index) &&
         correspondence_cost_cells_[ToFlatIndex(cell_index)] !=
             kUnknownCorrespondenceValue;
}

// Fills in 'offset' and 'limits' to define a subregion of that contains all
// known cells.
void Grid2D::ComputeCroppedLimits(Eigen::Array2i* const offset,
                                  CellLimits* const limits) const {
  if (known_cells_box_.isEmpty()) {
    *offset = Eigen::Array2i::Zero();
    *limits = CellLimits(1, 1);
    return;
  }
  *offset = known_cells_box_.min().array();
  *limits = CellLimits(known_cells_box_.sizes().x() + 1,
                       known_cells_box_.sizes().y() + 1);
}

// Grows the map as necessary to include 'point'. This changes the meaning of
// these coordinates going forward. This method must be called immediately
// after 'FinishUpdate', before any calls to 'ApplyLookupTable'.
void Grid2D::GrowLimits(const Eigen::Vector2f& point) {
  CHECK(update_indices_.empty());
  while (!limits_.Contains(limits_.GetCellIndex(point))) {
    const int x_offset = limits_.cell_limits().num_x_cells / 2;
    const int y_offset = limits_.cell_limits().num_y_cells / 2;
    const MapLimits new_limits(
        limits_.resolution(),
        limits_.max() +
            limits_.resolution() * Eigen::Vector2d(y_offset, x_offset),
        CellLimits(2 * limits_.cell_limits().num_x_cells,
                   2 * limits_.cell_limits().num_y_cells));
    const int stride = new_limits.cell_limits().num_x_cells;
    const int offset = x_offset + stride * y_offset;
    const int new_size = new_limits.cell_limits().num_x_cells *
                         new_limits.cell_limits().num_y_cells;
    std::vector<uint16> new_cells(new_size, kUnknownCorrespondenceValue);
    for (int i = 0; i < limits_.cell_limits().num_y_cells; ++i) {
      for (int j = 0; j < limits_.cell_limits().num_x_cells; ++j) {
        new_cells[offset + j + i * stride] =
            correspondence_cost_cells_[j +
                                       i * limits_.cell_limits().num_x_cells];
      }
    }
    correspondence_cost_cells_ = new_cells;
    limits_ = new_limits;
    if (!known_cells_box_.isEmpty()) {
      known_cells_box_.translate(Eigen::Vector2i(x_offset, y_offset));
    }
  }
}

proto::Grid2D Grid2D::ToProto() const {
  proto::Grid2D result;
  *result.mutable_limits() = mapping::ToProto(limits_);
  result.mutable_cells()->Reserve(correspondence_cost_cells_.size());
  for (const auto& cell : correspondence_cost_cells_) {
    result.mutable_cells()->Add(cell);
  }
  CHECK(update_indices().empty()) << "Serializing a grid during an update is "
                                     "not supported. Finish the update first.";
  if (!known_cells_box().isEmpty()) {
    auto* const box = result.mutable_known_cells_box();
    box->set_max_x(known_cells_box().max().x());
    box->set_max_y(known_cells_box().max().y());
    box->set_min_x(known_cells_box().min().x());
    box->set_min_y(known_cells_box().min().y());
  }
  return result;
}

int Grid2D::ToFlatIndex(const Eigen::Array2i& cell_index) const {
  CHECK(limits_.Contains(cell_index)) << cell_index;
  return limits_.cell_limits().num_x_cells * cell_index.y() + cell_index.x();
}

}  // namespace mapping
}  // namespace cartographer
