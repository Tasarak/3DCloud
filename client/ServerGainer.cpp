//
// Created by Jakub Klemens on 27/08/2018.
//

#include "ServerGainer.h"
using grpc::ClientWriter;
using grpc::Status;

using Cloud3D::LoadBalance;
using Cloud3D::ServerRequest;
using Cloud3D::ServerReply;

std::string ServerGainer::GetServer(std::vector<std::string> operations)
{
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("ServerGainer"));

    ServerRequest request;
    ServerReply response;
    grpc::ClientContext ctx;

    request.set_version(1.2);

    for (const auto &operation : operations)
    {
        request.add_operation(operation);
    }

    Status status = stub_->ListServer(&ctx, request, &response);

    if (status.ok())
    {
        return response.serveraddresss();
    }
    else
    {
        return "";
    }
}