//
// Created by Jakub Klemens on 23/08/2018.
//

#ifndef INC_3DCLOUD_LOADBALANCER_H
#define INC_3DCLOUD_LOADBALANCER_H

#include <memory>
#include <iostream>
#include <string>
#include <thread>
#include <vector>


#include "cloud_services.grpc.pb.h"
#include "LoadBalanceImpl.h"
#include "../Shared/FileParser.h"


class LoadBalancer
{
public:
    LoadBalancer(std::string &balancerAddress,
                 int &heartBeatRate,
                 std::string &certFilename,
                 std::string &keyFilename,
                 std::string &rootFilename)
                : balancerAddress_(balancerAddress),
                  heartBeatRate_(heartBeatRate),
                  certFilename_(certFilename),
                  keyFilename_(keyFilename),
                  rootFilename_(rootFilename)
                {}
    LoadBalancer(std::string &balancerAddress, int &heartBeatRate)
                : balancerAddress_(balancerAddress), heartBeatRate_(heartBeatRate) {}
    LoadBalancer(std::string &configFile);

    ~LoadBalancer();
    void Run();

private:
    void init();
    void initWithSSL();
    std::unique_ptr<grpc::ServerCompletionQueue> cq_;
    std::unique_ptr<grpc::Server> server_;

    std::string balancerAddress_, certFilename_, keyFilename_, rootFilename_ = "";
    int heartBeatRate_ = 1000;
};

#endif //INC_3DCLOUD_LOADBALANCER_H
