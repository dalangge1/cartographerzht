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

#include "cartographer/mapping/internal/2d/overlapping_submaps_trimmer_2d.h"

#include <vector>

#include "cartographer/common/time.h"
#include "cartographer/mapping/2d/submap_2d.h"
#include "cartographer/mapping/id.h"
#include "cartographer/mapping/internal/testing/fake_trimmable.h"
#include "cartographer/transform/transform.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace cartographer {
namespace mapping {
namespace {

using ::testing::ElementsAre;
using ::testing::Field;

class OverlappingSubmapsTrimmer2DTest : public ::testing::Test {
 protected:
  // Creates a submap with num_cells x num_cells grid.
  void AddSquareSubmap(const transform::Rigid2d& pose_2d, int submap_index,
                       int num_cells, bool is_finished) {
    const transform::Rigid3d pose_3d = transform::Embed3D(pose_2d);
    proto::Submap2D submap_2d;
    submap_2d.set_num_range_data(1);
    submap_2d.set_finished(is_finished);
    *submap_2d.mutable_local_pose() = transform::ToProto(pose_3d);

    auto* probability_grid = submap_2d.mutable_probability_grid();
    for (int i = 0; i < num_cells * num_cells; ++i) {
      probability_grid->add_cells(1);
    }

    auto* map_limits = probability_grid->mutable_limits();
    map_limits->set_resolution(1.0);
    *map_limits->mutable_max() =
        transform::ToProto(Eigen::Vector2d(num_cells, num_cells));
    map_limits->mutable_cell_limits()->set_num_x_cells(num_cells);
    map_limits->mutable_cell_limits()->set_num_y_cells(num_cells);
    // LOG(INFO) << "!proto " << submap_2d.DebugString();

    fake_pose_graph_.mutable_submap_data()->Insert(
        {0 /* trajectory_id */, submap_index},
        {std::make_shared<const Submap2D>(submap_2d), pose_3d});
  }

  void AddTrajectoryNode(int node_index, int64 timestamp) {
    TrajectoryNode::Data data;
    data.time = common::FromUniversal(timestamp);

    fake_pose_graph_.mutable_trajectory_nodes()->Insert(
        NodeId{0 /* trajectory_id */, node_index},
        {std::make_shared<const TrajectoryNode::Data>(data), {} /* pose */});
  }

  void AddConstraint(int submap_index, int node_index, bool is_intra_submap) {
    fake_pose_graph_.mutable_constraints()->push_back(
        {SubmapId{0 /* trajectory_id */, submap_index},
         NodeId{0 /* trajectory_id */, node_index},
         {} /* pose */,
         is_intra_submap ? PoseGraphInterface::Constraint::INTRA_SUBMAP
                         : PoseGraphInterface::Constraint::INTER_SUBMAP});
  }

  testing::FakeTrimmable fake_pose_graph_;
};

::testing::Matcher<const SubmapId&> EqualsSubmapId(const SubmapId& expected) {
  return ::testing::AllOf(
      Field(&SubmapId::trajectory_id, expected.trajectory_id),
      Field(&SubmapId::submap_index, expected.submap_index));
}

TEST_F(OverlappingSubmapsTrimmer2DTest, EmptyPoseGraph) {
  OverlappingSubmapsTrimmer2D trimmer(1 /* fresh_submaps_count */,
                                      0 /* min_covered_cells_count */);
  trimmer.Trim(&fake_pose_graph_);
  EXPECT_TRUE(fake_pose_graph_.trimmed_submaps().empty());
}

TEST_F(OverlappingSubmapsTrimmer2DTest, TrimFullyOverlappingSubmaps) {
  AddSquareSubmap(transform::Rigid2d::Identity(), 0 /* submap_index */,
                  1 /* num_cells */, true /* is_finished */);
  AddSquareSubmap(transform::Rigid2d::Identity(), 1 /* submap_index */,
                  1 /* num_cells */, true /* is_finished */);
  AddTrajectoryNode(0 /* node_index */, 1000 /* timestamp */);
  AddTrajectoryNode(1 /* node_index */, 2000 /* timestamp */);
  AddConstraint(0 /*submap_index*/, 0 /*node_index*/, true);
  AddConstraint(1 /*submap_index*/, 1 /*node_index*/, true);

  OverlappingSubmapsTrimmer2D trimmer(1 /* fresh_submaps_count */,
                                      0 /* min_covered_cells_count */);
  trimmer.Trim(&fake_pose_graph_);
  EXPECT_THAT(fake_pose_graph_.trimmed_submaps(),
              ElementsAre(EqualsSubmapId({0, 0})));
}

}  // namespace
}  // namespace mapping
}  // namespace cartographer
