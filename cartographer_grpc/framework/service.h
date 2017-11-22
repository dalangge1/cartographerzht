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

#ifndef CARTOGRAPHER_GRPC_FRAMEWORK_SERVICE_H
#define CARTOGRAPHER_GRPC_FRAMEWORK_SERVICE_H

#include <grpc++/impl/codegen/service_type.h>

#include "cartographer_grpc/framework/rpc_handler.h"

namespace cartographer_grpc {
namespace framework {

class Service : public ::grpc::Service {
 public:
  Service(const std::map<std::string, RpcHandlerInfo>& rpc_handlers);

 private:
  std::map<std::string, RpcHandlerInfo> rpc_handlers_;
};

}  // namespace framework
}  // namespace cartographer_grpc

#endif  // CARTOGRAPHER_GRPC_FRAMEWORK_SERVICE_H
