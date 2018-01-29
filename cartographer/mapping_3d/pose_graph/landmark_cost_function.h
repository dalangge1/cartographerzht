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

#ifndef CARTOGRAPHER_MAPPING_3D_POSE_GRAPH_LANDMARK_COST_FUNCTION_H_
#define CARTOGRAPHER_MAPPING_3D_POSE_GRAPH_LANDMARK_COST_FUNCTION_H_

#include "Eigen/Core"
#include "Eigen/Geometry"
#include "cartographer/mapping/pose_graph.h"
#include "cartographer/mapping_3d/pose_graph/spa_cost_function.h"
#include "cartographer/transform/rigid_transform.h"
#include "cartographer/transform/transform.h"
#include "ceres/ceres.h"
#include "ceres/jet.h"

namespace cartographer {
namespace mapping_3d {
namespace pose_graph {

// Cost function measuring weighted error between the observed relative pose
// given by sensor and relative pose computed from the linearly interpolated
// pose of the robot at the moment of observation.
class LandmarkCostFunction {
 public:
  using LandmarkObservation =
      mapping::PoseGraph::LandmarkNode::LandmarkObservation;

  static ceres::CostFunction* CreateAutoDiffCostFunction(
      const LandmarkObservation& observation, common::Time prev_node_time,
      common::Time next_node_time) {
    return new ceres::AutoDiffCostFunction<
        LandmarkCostFunction, 6 /* residuals */,
        3 /* previous node translation variables */,
        4 /* previous node rotation variables */,
        3 /* next node translation variables */,
        4 /* next node rotation variables */,
        3 /* landmark translation variables */,
        4 /* landmark rotation variables */>(
        new LandmarkCostFunction(observation, prev_node_time, next_node_time));
  }

  template <typename T>
  bool operator()(const T* const prev_node_rotation,
                  const T* const prev_node_translation,
                  const T* const next_node_rotation,
                  const T* const next_node_translation,
                  const T* const landmark_rotation,
                  const T* const landmark_translation, T* const e) const {
    const T interpolated_pose_translation[3] = {
        prev_node_translation[0] +
            interpolation_parameter_ *
                (next_node_translation[0] - prev_node_translation[0]),
        prev_node_translation[1] +
            interpolation_parameter_ *
                (next_node_translation[1] - prev_node_translation[1]),
        prev_node_translation[2] +
            interpolation_parameter_ *
                (next_node_translation[2] - prev_node_translation[2])};

    const Eigen::Quaternion<T> prev_quaternion(
        prev_node_rotation[0], prev_node_rotation[1], prev_node_rotation[2],
        prev_node_rotation[3]);
    const Eigen::Quaternion<T> next_quaternion(
        next_node_rotation[0], next_node_rotation[1], next_node_rotation[2],
        next_node_rotation[3]);

    const Eigen::Quaternion<T> interpolated_quaternion =
        prev_quaternion.slerp(T(interpolation_parameter_), next_quaternion);
    const T interpolated_pose_rotation[4] = {
        interpolated_quaternion.w(), interpolated_quaternion.x(),
        interpolated_quaternion.y(), interpolated_quaternion.z()};

    // TODO(pifon2a): Move functions common for all cost functions outside of
    // SpaCostFunction scope.
    const std::array<T, 6> unscaled_error =
        SpaCostFunction::ComputeUnscaledError(
            landmark_to_tracking_transform_, interpolated_pose_rotation,
            interpolated_pose_translation, landmark_rotation,
            landmark_translation);

    e[0] = T(translation_weight_) * unscaled_error[0];
    e[1] = T(translation_weight_) * unscaled_error[1];
    e[2] = T(translation_weight_) * unscaled_error[2];
    e[3] = T(rotation_weight_) * unscaled_error[3];
    e[4] = T(rotation_weight_) * unscaled_error[4];
    e[5] = T(rotation_weight_) * unscaled_error[5];
    return true;
  }

 private:
  LandmarkCostFunction(const LandmarkObservation& observation,
                       common::Time prev_node_time, common::Time next_node_time)
      : landmark_to_tracking_transform_(
            observation.landmark_to_tracking_transform),
        translation_weight_(observation.translation_weight),
        rotation_weight_(observation.rotation_weight),
        interpolation_parameter_(
            common::ToSeconds(observation.time - prev_node_time) /
            common::ToSeconds(next_node_time - prev_node_time)) {}

  const transform::Rigid3d landmark_to_tracking_transform_;
  const double translation_weight_;
  const double rotation_weight_;
  const double interpolation_parameter_;
};

}  // namespace pose_graph
}  // namespace mapping_3d
}  // namespace cartographer

#endif  // CARTOGRAPHER_MAPPING_3D_POSE_GRAPH_LANDMARK_COST_FUNCTION_H_