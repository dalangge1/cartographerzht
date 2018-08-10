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

#ifndef CARTOGRAPHER_POSE_GRAPH_CONSTRAINT_CONSTRAINT_UTILS_H_
#define CARTOGRAPHER_POSE_GRAPH_CONSTRAINT_CONSTRAINT_UTILS_H_

#include "cartographer/pose_graph/node/imu_calibration.h"
#include "cartographer/pose_graph/node/pose_2d.h"
#include "cartographer/pose_graph/node/pose_3d.h"
#include "ceres/problem.h"

namespace cartographer {
namespace pose_graph {

#define FIND_NODE(node_name, node_id, map, log_message)                   \
  FIND_NODE_IMPL(STATUS_MACROS_IMPL_CONCAT_(__node, __LINE__), node_name, \
                 node_id, map, log_message)

#define FIND_NODE_IMPL(node_item, node_name, node_id, map, log_message) \
  auto node_item = common::FindOrNull(map, node_id);                    \
  if (node_item == nullptr) {                                           \
    LOG(INFO) << log_message;                                           \
    return;                                                             \
  }                                                                     \
  auto node_name = node_item;

#define STATUS_MACROS_IMPL_CONCAT_INNER_(x, y) x##y
#define STATUS_MACROS_IMPL_CONCAT_(x, y) STATUS_MACROS_IMPL_CONCAT_INNER_(x, y)

void AddPose2D(Pose2D* pose, ceres::Problem* problem);

void AddPose3D(Pose3D* pose, ceres::Problem* problem);

void AddImuCalibration(ImuCalibration* pose, ceres::Problem* problem);

}  // namespace pose_graph
}  // namespace cartographer

#endif  // CARTOGRAPHER_POSE_GRAPH_CONSTRAINT_CONSTRAINT_UTILS_H_
