//
// Created by Jakub Klemens on 27/08/2018.
//

#ifndef INC_3DCLOUD_LOADBALANCEIMPL_H
#define INC_3DCLOUD_LOADBALANCEIMPL_H

#include <memory>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <log4cplus/loglevel.h>
#include <cloud_services.pb.h>
#include <cloud_services.grpc.pb.h>

#include "cloud_services.grpc.pb.h"

struct ServerNode
{
    std::string address;
    std::vector<std::string> operations;
    float version;
    u_int usage;
    std::chrono::time_point<std::chrono::system_clock> lastBeat;
};

class LoadBalanceImpl final : public Cloud3D::LoadBalance::Service
{
public:
    LoadBalanceImpl(int &heartBeatRate);
    ~LoadBalanceImpl() override;

    ::grpc::Status ListServer(::grpc::ServerContext* context,
                              const ::Cloud3D::ServerRequest* request,
                              ::Cloud3D::ServerReply* response) override;

    ::grpc::Status SendHeartbeat(::grpc::ServerContext* context,
                                 const ::Cloud3D::HeartBeat* request,
                                 ::Cloud3D::HeartBeatReply* response) override;

    ::grpc::Status EstablishServer(::grpc::ServerContext* context,
                                   const ::Cloud3D::NewServer* request,
                                   ::Cloud3D::Confirmation* response) override;

private:
    void UpdateServerList();
    void RemoveInactiveServer();
    std::string FindBestServer(ServerNode sNode);
    std::string MinUsageServer(std::vector<ServerNode> possibleServers);
    std::vector<ServerNode> servers;
    std::thread updater_;
    std::mutex mutex_;
    int heartBeatRate_;
};


#endif //INC_3DCLOUD_LOADBALANCEIMPL_H
