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
//#include "../Shared/CallData.h"
#include "LoadBalanceImpl.h"


class LoadBalancer
{
public:
    LoadBalancer() = default;
    ~LoadBalancer();
    void Run(std::string serverAddress);

private:
    std::unique_ptr<grpc::ServerCompletionQueue> cq_;
    std::unique_ptr<grpc::Server> server_;
    Cloud3D::LoadBalance::AsyncService service_;
};

#endif //INC_3DCLOUD_LOADBALANCER_H
