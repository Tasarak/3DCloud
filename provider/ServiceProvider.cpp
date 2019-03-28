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

void ServiceProvider::init()
{
    serviceProviderImpl = &ServiceProviderImpl::GetInstance();
    balancer = new BalancerEstablisher(providerAddress_, grpc::CreateChannel(balancerAddress_,
                                                                            grpc::InsecureChannelCredentials()),
                                       version_,
                                       heartBeatRate_);
    serverBuilder.AddListeningPort(providerAddress_, grpc::InsecureServerCredentials());
}

void ServiceProvider::initWithSSL()
{
    serviceProviderImpl = &ServiceProviderImpl::GetInstance();

    FileParser parser;
    parser.read(certFilename_, cert_);
    parser.read(keyFilename_, key_);
    parser.read(rootFilename_, root_);

    grpc::SslCredentialsOptions opts = {root_, key_, cert_};

    auto channel_creds = grpc::SslCredentials(grpc::SslCredentialsOptions(opts));

    balancer = new BalancerEstablisher(providerAddress_, grpc::CreateChannel(balancerAddress_, channel_creds),
                                       version_, heartBeatRate_);

    grpc::SslServerCredentialsOptions::PemKeyCertPair keycert = {key_, cert_};

    sslOps.pem_root_certs = root_;
    sslOps.pem_key_cert_pairs.push_back ( keycert );

    serverBuilder.AddListeningPort(providerAddress_, grpc::SslServerCredentials(sslOps));
}

ServiceProvider::ServiceProvider(std::string providerAddress, std::string balancerAddress,
                                 float &version, int &heartBeatRate)
                                : providerAddress_(providerAddress), balancerAddress_(balancerAddress),
                                  version_(version), heartBeatRate_(heartBeatRate)
{
    init();
}

ServiceProvider::ServiceProvider(std::string providerAddress,
                                 std::string balancerAddress,
                                 float &version,
                                 int &heartBeatRate,
                                 std::string &certFilename,
                                 std::string &keyFilename,
                                 std::string &rootFilename)
                                :  providerAddress_(providerAddress),
                                   balancerAddress_(balancerAddress),
                                   version_(version),
                                   heartBeatRate_(heartBeatRate),
                                   certFilename_(certFilename),
                                   keyFilename_(keyFilename),
                                   rootFilename_(rootFilename)
{
    initWithSSL();
}

ServiceProvider::ServiceProvider(std::string &inputFile)
{
    FileParser parser;
    try
    {
        parser.parseProviderConfig(inputFile, providerAddress_, balancerAddress_, 
                                   heartBeatRate_, version_, 
                                   certFilename_, keyFilename_, rootFilename_);
    }
    catch (...)
    {
        throw;
    }

    if (balancerAddress_.empty() || providerAddress_.empty())
    {
        throw std::invalid_argument("Address of Load Balancer or Service Provider not defined in config.");
    }

    if (!certFilename_.empty() && !keyFilename_.empty() && !rootFilename_.empty())
    {
        initWithSSL();
    }
    else
    {
        init();
    }

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
