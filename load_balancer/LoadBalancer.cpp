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

void LoadBalancer::init()
{
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("LoadBalancer"));
    ServerBuilder serverBuilder;
    serverBuilder.AddListeningPort(balancerAddress_, grpc::InsecureServerCredentials());

    LoadBalanceImpl service(heartBeatRate_);
    serverBuilder.RegisterService(&service);

    server_ = serverBuilder.BuildAndStart();
    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Load balancer listening on: ") << balancerAddress_);
    std::cout << "Load balancer listening on " << balancerAddress_ << std::endl;
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
    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Load balancer listening on: ") << balancerAddress_);
    std::cout << "Load balancer listening on " << balancerAddress_ << std::endl;
    server_->Wait();
}

LoadBalancer::LoadBalancer(std::string &configFile)
{
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

void LoadBalancer::Run()
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
