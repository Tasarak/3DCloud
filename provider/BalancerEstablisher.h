//
// Created by Jakub Klemens on 27/08/2018.
//

#ifndef INC_3DCLOUD_BALANCERESTABLISHER_H
#define INC_3DCLOUD_BALANCERESTABLISHER_H

#include <vector>
#include <memory>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <log4cplus/loglevel.h>

#include "cloud_services.grpc.pb.h"

#include "ServiceProviderImpl.h"

class BalancerEstablisher
{
public:
    explicit BalancerEstablisher(std::string srvAddress, std::shared_ptr<grpc::Channel> channel);
    void SendHeartBeat();
    int EstablishServer();

private:
    void SendBeat();
    std::unique_ptr<Cloud3D::LoadBalance::Stub> stub_;
    std::string srvAddress_;
};


#endif //INC_3DCLOUD_BALANCERESTABLISHER_H
