//
// Created by Jakub Klemens on 06/08/2018.
//

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

void ServiceProvider::initServiceProviderImpl()
{
    serviceProviderImpl_ = &ServiceProviderImpl::getInstance();
}

/***
 * Create insecure connection
 */
void ServiceProvider::init()
{
    balancer_ = new BalancerEstablisher(providerAddress_, grpc::CreateChannel(balancerAddress_,
                                                                            grpc::InsecureChannelCredentials()),
                                       version_,
                                       heartBeatRate_);
    serverBuilder_.AddListeningPort(providerAddress_, grpc::InsecureServerCredentials());
}

/***
 * Create secure connection using SSL
 */
void ServiceProvider::initWithSSL()
{
    FileParser parser;
    parser.read(certFilename_, cert_);
    parser.read(keyFilename_, key_);
    parser.read(rootFilename_, root_);

    grpc::SslCredentialsOptions opts = {root_, key_, cert_};

    auto channel_creds = grpc::SslCredentials(grpc::SslCredentialsOptions(opts));

    balancer_ = new BalancerEstablisher(providerAddress_, grpc::CreateChannel(balancerAddress_, channel_creds),
                                       version_, heartBeatRate_);

    grpc::SslServerCredentialsOptions::PemKeyCertPair keycert = {key_, cert_};

    sslOps_.pem_root_certs = root_;
    sslOps_.pem_key_cert_pairs.push_back ( keycert );

    serverBuilder_.AddListeningPort(providerAddress_, grpc::SslServerCredentials(sslOps_));
}

ServiceProvider::ServiceProvider(std::string providerAddress, std::string balancerAddress,
                                 float &version, int &heartBeatRate)
                                : providerAddress_(providerAddress), balancerAddress_(balancerAddress),
                                  version_(version), heartBeatRate_(heartBeatRate)
{
    isUsingAuthentication_ = false;
    init();
    initServiceProviderImpl();
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
    isUsingAuthentication_ = true;
    initWithSSL();
    initServiceProviderImpl();
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
        isUsingAuthentication_ = true;
        initWithSSL();
        initServiceProviderImpl();
    }
    else
    {
        isUsingAuthentication_ = false;
        init();
        initServiceProviderImpl();
    }

}

ServiceProvider::~ServiceProvider()
{
    server_->Shutdown();
    cq_->Shutdown();
}

void ServiceProvider::startServer()
{
    server_->Wait();
}

/***
 * Main loop of Service Provider.
 * Spawn two threads. First for connecting to Load Balancer. Second to handle incoming RPCs.
 * @return 0
 */
int ServiceProvider::run()
{
    serverBuilder_.RegisterService(serviceProviderImpl_);
    serverBuilder_.SetMaxReceiveMessageSize(-1);

    server_ = serverBuilder_.BuildAndStart();

    balancer_->establishServer();

    //main loop
    std::thread first(&BalancerEstablisher::sendHeartBeat, balancer_);
    std::thread second(&ServiceProvider::startServer, this);

    first.join();
    second.join();

    return 0;
}

void ServiceProvider::setModelsToModelsService(ModelToModelService newService)
{
    for (auto const &name : serviceProviderImpl_->ListOfServiceNames)
    {
        if (name == newService.getName())
        {
            throw std::invalid_argument("Service name_ already exists.\n");
        }
    }

    serviceProviderImpl_->ModelsToModelsServices.push_back(newService);
    serviceProviderImpl_->ListOfServiceNames.push_back(newService.getName());
}

void ServiceProvider::setModelsToNumbersService(ModelToNumberService newService)
{
    for (auto const &name : serviceProviderImpl_->ListOfServiceNames)
    {
        if (name == newService.getName())
        {
            throw std::invalid_argument("Service name_ already exists.\n");
        }
    }

    serviceProviderImpl_->ModelToNumbersServices.push_back(newService);
    serviceProviderImpl_->ListOfServiceNames.push_back(newService.getName());
}

void ServiceProvider::setUsageFunction(void (*fp)(int &))
{
    balancer_->setUsageFunction(fp);
}
