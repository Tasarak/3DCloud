//
// Created by Jakub Klemens on 06/08/2018.
//

#include <cloud_services.grpc.pb.h>
#include <cloud_services.pb.h>
#include "ServiceProvider.h"

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerCompletionQueue;
using grpc::Status;

using Cloud3D::Model;
using Cloud3D::VectorofNumbers;
using Cloud3D::ServiceProvide;

ServiceProvider::ServiceProvider()
{
    serviceProviderImpl = &ServiceProviderImpl::GetInstance();
}

ServiceProvider::~ServiceProvider()
{
    server_->Shutdown();
    cq_->Shutdown();
}

int ServiceProvider::Run(std::string serverAddress)
{
    ServerBuilder serverBuilder;
    serverBuilder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());

    serverBuilder.RegisterService(serviceProviderImpl);

    server_ = serverBuilder.BuildAndStart();
    std::cout << "Server listening on " << serverAddress << std::endl;

    //main loop
    server_->Wait();

    return 0;
}

void ServiceProvider::setModelsToModelsService(ModelToModelService newService)
{
    for (auto const &name : serviceProviderImpl->ListOfServiceNames)
    {
        if (name == newService.getName())
        {
            throw std::invalid_argument("Service name already exists.\n");
        }
    }

    serviceProviderImpl->ModelsToModelsServices.push_back(newService);
    serviceProviderImpl->ListOfServiceNames.push_back(newService.getName());
}

void ServiceProvider::setModelsToNumbersService(ModelToNumberService newService)
{
    for (auto const &name : serviceProviderImpl->ListOfServiceNames)
    {
        if (name == newService.getName())
        {
            throw std::invalid_argument("Service name already exists.\n");
        }
    }

    serviceProviderImpl->ModelToNumbersServices.push_back(newService);
    serviceProviderImpl->ListOfServiceNames.push_back(newService.getName());
}
