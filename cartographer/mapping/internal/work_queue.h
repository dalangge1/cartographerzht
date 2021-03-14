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

#ifndef CARTOGRAPHER_MAPPING_INTERNAL_WORK_QUEUE_H
#define CARTOGRAPHER_MAPPING_INTERNAL_WORK_QUEUE_H

#include <chrono>
#include <deque>
#include <functional>

namespace cartographer {
namespace mapping {

enum WorkItemType {
  CHANGE_TRAJECTORY_STATE, 
  OPTIMIZATION_ADD_IMU_DATA,
  OPTIMIZATION_ADD_ODOM_DATA, 
  OPTIMIZATION_ADD_LANDMARK_DATA,
  OPTIMIZATION_ADD_FIXED_FRAME_DATA,
  OPTIMIZATION_SOLVE, 
  OPTIMIZATION_INSERT_SUBMAP, 
  COMPUTE_CONSTRAINT,  // CAN BE LOOP CLOSURES OR INTRA_SUBMAP CONSTRAINT 
  NODE_TRAJECTORY_INSERTION, 
  NODE_SUBMAP_INSERTION, 
  OTHER_ITEM };

struct WorkItem {
  enum class Result {
    kDoNotRunOptimization,
    kRunOptimization,
  };
  using Details = std::map<std::string, double>;

  std::chrono::steady_clock::time_point time;
  std::function<std::pair<Result, Details>()> task;
  WorkItemType t{OTHER_ITEM};
};

using WorkQueue = std::deque<WorkItem>;

}  // namespace mapping
}  // namespace cartographer

#endif  // CARTOGRAPHER_MAPPING_INTERNAL_WORK_QUEUE_H
