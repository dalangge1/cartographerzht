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

#ifndef CARTOGRAPHER_GRPC_FRAMEWORK_CLIENT_H
#define CARTOGRAPHER_GRPC_FRAMEWORK_CLIENT_H

#include "grpc++/grpc++.h"
#include "grpc++/impl/codegen/client_unary_call.h"
#include "grpc++/impl/codegen/sync_stream.h"

namespace cartographer_grpc {
namespace framework {

template <typename RpcHandlerType>
class Client {
 public:
  Client(std::shared_ptr<grpc::Channel> channel)
      : channel_(channel),
        rpc_method_name_(
            RpcHandlerInterface::Instantiate<RpcHandlerType>()->method_name()),
        rpc_method_(rpc_method_name_.c_str(),
                    RpcType<typename RpcHandlerType::IncomingType,
                            typename RpcHandlerType::OutgoingType>::value,
                    channel_) {
    switch (rpc_method_.method_type()) {
      case grpc::internal::RpcMethod::NORMAL_RPC:
        break;
      case grpc::internal::RpcMethod::CLIENT_STREAMING:
        break;
      default:
        LOG(FATAL) << "Not implemented.";
    }
  }

  bool Write(const typename RpcHandlerType::RequestType& request) {
    switch (rpc_method_.method_type()) {
      case grpc::internal::RpcMethod::NORMAL_RPC:
        return MakeBlockingUnaryCall(request, &response_).ok();
      case grpc::internal::RpcMethod::CLIENT_STREAMING:
        InstantiateClientWriterIfNeeded();
        return client_writer_->Write(request);
      default:
        LOG(FATAL) << "Not implemented.";
    }
  }

  bool WritesDone() {
    switch (rpc_method_.method_type()) {
      case grpc::internal::RpcMethod::CLIENT_STREAMING:
        InstantiateClientWriterIfNeeded();
        return client_writer_->WritesDone();
      default:
        LOG(FATAL) << "Not implemented.";
    }
  }

  grpc::Status Finish() {
    switch (rpc_method_.method_type()) {
      case grpc::internal::RpcMethod::CLIENT_STREAMING:
        InstantiateClientWriterIfNeeded();
        return client_writer_->Finish();
      default:
        LOG(FATAL) << "Not implemented.";
    }
  }

  const typename RpcHandlerType::ResponseType& response() {
    CHECK(rpc_method_.method_type() == grpc::internal::RpcMethod::NORMAL_RPC ||
          rpc_method_.method_type() ==
              grpc::internal::RpcMethod::CLIENT_STREAMING);
    return response_;
  }

 private:
  void InstantiateClientWriterIfNeeded() {
    CHECK_EQ(rpc_method_.method_type(),
             grpc::internal::RpcMethod::CLIENT_STREAMING);
    if (!client_writer_) {
      client_writer_.reset(
          grpc::internal::ClientWriterFactory<
              typename RpcHandlerType::RequestType>::Create(channel_.get(),
                                                            rpc_method_,
                                                            &client_context_,
                                                            &response_));
    }
  }

  grpc::Status MakeBlockingUnaryCall(
      const typename RpcHandlerType::RequestType& request,
      typename RpcHandlerType::ResponseType* response) {
    CHECK_EQ(rpc_method_.method_type(), grpc::internal::RpcMethod::NORMAL_RPC);
    return ::grpc::internal::BlockingUnaryCall(
        channel_.get(), rpc_method_, &client_context_, request, response);
  }

  std::shared_ptr<grpc::Channel> channel_;
  grpc::ClientContext client_context_;
  const std::string rpc_method_name_;
  const ::grpc::internal::RpcMethod rpc_method_;

  std::unique_ptr<grpc::ClientWriter<typename RpcHandlerType::RequestType>>
      client_writer_;
  std::unique_ptr < grpc::ClientReaderWriter
      typename RpcHandlerType::ResponseType response_;
};

}  // namespace framework
}  // namespace cartographer_grpc

#endif  // CARTOGRAPHER_GRPC_FRAMEWORK_CLIENT_H
