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
    explicit BalancerEstablisher(std::string &srvAddress, std::shared_ptr<grpc::Channel> channel,
                                 float &version, int &heartBeatRate);
    void sendHeartBeat();
    int establishServer();
    void setUsageFunction(void (*fp_)(int &));

private:
    void sendBeat();
    void countUsage();

    std::unique_ptr<Cloud3D::LoadBalance::Stub> stub_;
    std::string srvAddress_;
    float version_ = 0.0;
    int heartBeatRate_ = 1000;
    int usage_ = 0;
    void (*fp_)(int&);
    bool isUsageSet = false;
};


#endif //INC_3DCLOUD_BALANCERESTABLISHER_H
