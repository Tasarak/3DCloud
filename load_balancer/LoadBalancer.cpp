//
// Created by Jakub Klemens on 23/08/2018.
//

#include "LoadBalancer.h"

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerCompletionQueue;
using grpc::Status;

void LoadBalancer::Run(std::string serverAddress)
{
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("LoadBalancer"));
    ServerBuilder serverBuilder;
    serverBuilder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());

    LoadBalanceImpl service;
    serverBuilder.RegisterService(&service);

    server_ = serverBuilder.BuildAndStart();
    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Load balancer listening on: ") << serverAddress);
    std::cout << "Load balancer listening on " << serverAddress << std::endl;
    server_->Wait();
}


LoadBalancer::~LoadBalancer()
{
    server_->Shutdown();
    cq_->Shutdown();
}
