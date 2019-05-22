//
// Created by Jakub Klemens on 27/08/2018.
//

#include "ProviderFinder.h"

using grpc::ClientWriter;
using grpc::Status;

using Cloud3D::LoadBalance;
using Cloud3D::ServerRequest;
using Cloud3D::ServerReply;
using grpc::Channel;
using grpc::ClientContext;

/***
 * Connec to Load Balancer and get Provide address.
 * Based on channel, it's done with authentication or not
 * @param operations
 * @param channel
 * @return address of Provide if successful
 */
std::string ProviderFinder::getServer(std::vector<std::string> operations,
                                      std::shared_ptr<::grpc::Channel> channel)
{

    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("ProviderFinder"));

    stub_ = Cloud3D::LoadBalance::NewStub(channel);
    ServerRequest request;
    ServerReply response;
    grpc::ClientContext ctx;

    request.set_version(version_);

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