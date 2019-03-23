//
// Created by Jakub Klemens on 27/08/2018.
//

#ifndef INC_3DCLOUD_SERVERGAINER_H
#define INC_3DCLOUD_SERVERGAINER_H

#include <memory>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>

#include "cloud_services.grpc.pb.h"

class ServerGainer
{
public:
    ServerGainer(std::shared_ptr<grpc::Channel> channel) :
    stub_(Cloud3D::LoadBalance::NewStub(channel)) {}
    std::string GetServer(std::vector<std::string>);

private:
    std::unique_ptr<Cloud3D::LoadBalance::Stub> stub_;
};


#endif //INC_3DCLOUD_SERVERGAINER_H
