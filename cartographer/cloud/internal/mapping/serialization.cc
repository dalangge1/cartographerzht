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

#include "cartographer/cloud/internal/mapping/serialization.h"

namespace cartographer {
namespace cloud {

proto::TrajectoryState ToProto(
    const mapping::PoseGraphInterface::TrajectoryState& trajectory_state) {
  using TrajectoryState = mapping::PoseGraphInterface::TrajectoryState;
  switch (trajectory_state) {
    case TrajectoryState::ACTIVE:
      return proto::TrajectoryState::ACTIVE;
    case TrajectoryState::FINISHED:
      return proto::TrajectoryState::FINISHED;
    case TrajectoryState::FROZEN:
      return proto::TrajectoryState::FROZEN;
    case TrajectoryState::DELETED:
      return proto::TrajectoryState::DELETED;
    default:
      LOG(FATAL) << "unknown TrajectoryState";
  }
}

mapping::PoseGraphInterface::TrajectoryState FromProto(
    const proto::TrajectoryState& proto) {
  using TrajectoryState = mapping::PoseGraphInterface::TrajectoryState;
  switch (proto) {
    case proto::TrajectoryState::ACTIVE:
      return TrajectoryState::ACTIVE;
    case proto::TrajectoryState::FINISHED:
      return TrajectoryState::FINISHED;
    case proto::TrajectoryState::FROZEN:
      return TrajectoryState::FROZEN;
    case proto::TrajectoryState::DELETED:
      return TrajectoryState::DELETED;
    default:
      LOG(FATAL) << "unknown proto::TrajectoryState";
  }
}

}  // namespace cloud
}  // namespace cartographer