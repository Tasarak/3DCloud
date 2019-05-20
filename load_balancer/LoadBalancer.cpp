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

LoadBalancer::LoadBalancer(std::string &balancerAddress, int &heartBeatRate) :
                            balancerAddress_(balancerAddress), heartBeatRate_(heartBeatRate)
{
    ::log4cplus::initialize();
    ::log4cplus::PropertyConfigurator::doConfigure("./Shared/log4cplus_configure.ini");
}

LoadBalancer::LoadBalancer(std::string &balancerAddress, int &heartBeatRate, std::string &certFilename,
                           std::string &keyFilename, std::string &rootFilename) :
                           balancerAddress_(balancerAddress),
                           heartBeatRate_(heartBeatRate),
                           certFilename_(certFilename),
                           keyFilename_(keyFilename),
                           rootFilename_(rootFilename)

{
    ::log4cplus::initialize();
    ::log4cplus::PropertyConfigurator::doConfigure("./Shared/log4cplus_configure.ini");
}

void LoadBalancer::init()
{
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("LoadBalancer"));
    ServerBuilder serverBuilder;
    serverBuilder.AddListeningPort(balancerAddress_, grpc::InsecureServerCredentials());

    LoadBalanceImpl service(heartBeatRate_);
    serverBuilder.RegisterService(&service);

    server_ = serverBuilder.BuildAndStart();
    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Load balancer_ listening on: ") << balancerAddress_);
    server_->Wait();
}

void LoadBalancer::initWithSSL()
{
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("LoadBalancer"));
    std::string cert;
    std::string key;
    std::string root;

    FileParser parser;

    parser.read(certFilename_, cert);
    parser.read(keyFilename_, key);
    parser.read(rootFilename_, root);

    grpc::SslServerCredentialsOptions sslOps;

    grpc::SslServerCredentialsOptions::PemKeyCertPair keycert = {key, cert};

    sslOps.pem_root_certs = root;
    sslOps.pem_key_cert_pairs.push_back ( keycert );

    ServerBuilder serverBuilder;
    serverBuilder.AddListeningPort(balancerAddress_, grpc::SslServerCredentials(sslOps));

    LoadBalanceImpl service(heartBeatRate_);
    serverBuilder.RegisterService(&service);

    server_ = serverBuilder.BuildAndStart();
    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Load balancer_ listening on: ") << balancerAddress_);
    server_->Wait();
}

LoadBalancer::LoadBalancer(std::string &configFile)
{
    ::log4cplus::initialize();
    ::log4cplus::PropertyConfigurator::doConfigure("./Shared/log4cplus_configure.ini");

    FileParser parser;
    try
    {
        parser.parseBalancerConfig(configFile, balancerAddress_, heartBeatRate_,
                                   certFilename_, keyFilename_, rootFilename_);
    }
    catch (...)
    {
        throw;
    }
    if (balancerAddress_.empty())
    {
        throw std::invalid_argument("Address of LoadBalancer not defined in config.");
    }
}

void LoadBalancer::run()
{
    if (!certFilename_.empty() && !keyFilename_.empty() && !rootFilename_.empty())
    {
        initWithSSL();
    }
    else
    {
        init();
    }
}


LoadBalancer::~LoadBalancer()
{
    server_->Shutdown();
    cq_->Shutdown();
}
