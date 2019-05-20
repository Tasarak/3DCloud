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
#include <sstream>
#include <fstream>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <log4cplus/loglevel.h>

#include <cloud_services.grpc.pb.h>

class ProviderFinder
{
public:
    ProviderFinder(std::string &balancerAddress, float &version);
    std::string getServer(std::vector<std::string>);

private:
    std::unique_ptr<Cloud3D::LoadBalance::Stub> stub_;
    float version_;
};


#endif //INC_3DCLOUD_SERVERGAINER_H
