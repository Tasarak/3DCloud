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

void ServiceProvider::read(const std::string &filename, std::string &data)
{
    std::ifstream file(filename.c_str(), std::ios::in);

    if (file.is_open())
    {
        std::stringstream ss;
        ss << file.rdbuf();

        file.close();

        data = ss.str();
    }
}

ServiceProvider::ServiceProvider(std::string providerAddress, std::string balancerAddress)
{
    serviceProviderImpl = &ServiceProviderImpl::GetInstance();
    balancer = new BalancerEstablisher(providerAddress, grpc::CreateChannel(balancerAddress,
                                                                    grpc::InsecureChannelCredentials()));
    serverBuilder.AddListeningPort(providerAddress, grpc::InsecureServerCredentials());
}

ServiceProvider::ServiceProvider(std::string providerAddress,
                                 std::string balancerAddress,
                                 std::string &certFilename,
                                 std::string &keyFilename,
                                 std::string &rootFilename)
{
    serviceProviderImpl = &ServiceProviderImpl::GetInstance();
    balancer = new BalancerEstablisher(providerAddress, grpc::CreateChannel(balancerAddress,
                                                                            grpc::InsecureChannelCredentials()));
    read(certFilename, _cert);
    read(keyFilename, _key);
    read(rootFilename, _root);

    grpc::SslServerCredentialsOptions::PemKeyCertPair keycert = {_key, _cert};

    sslOps.pem_root_certs = _root;
    sslOps.pem_key_cert_pairs.push_back ( keycert );

    serverBuilder.AddListeningPort(providerAddress, grpc::SslServerCredentials(sslOps));
}

ServiceProvider::~ServiceProvider()
{
    server_->Shutdown();
    cq_->Shutdown();
}

void ServiceProvider::StartServer()
{
    server_->Wait();
}

int ServiceProvider::Run()
{
    serverBuilder.RegisterService(serviceProviderImpl);

    server_ = serverBuilder.BuildAndStart();

    balancer->EstablishServer();

    //main loop
    std::thread first(&BalancerEstablisher::SendHeartBeat, balancer);
    std::thread second(&ServiceProvider::StartServer, this);

    first.join();
    second.join();

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
