//
// Created by Jakub Klemens on 06/08/2018.
//

#ifndef INC_CLOUD_CLOUD_SERVER_H
#define INC_CLOUD_CLOUD_SERVER_H

#include <stdexcept>
#include <fstream>

#include <cloud_services.grpc.pb.h>
#include <cloud_services.pb.h>

#include "BalancerEstablisher.h"
#include "ServiceProviderImpl.h"

class ServiceProvider
{
public:
    ServiceProvider(std::string providerAddress, std::string balancerAddress);
    ServiceProvider(std::string providerAddress,
                    std::string balancerAddress,
                    std::string &certFilename,
                    std::string &keyFilename,
                    std::string &rootFilename);
    ~ServiceProvider();
    int Run();

    void setModelsToModelsService(ModelToModelService);
    void setModelsToNumbersService(ModelToNumberService);

private:
    void read (const std::string& filename, std::string& data);
    void StartServer();
    std::unique_ptr<grpc::ServerCompletionQueue> cq_;
    std::unique_ptr<grpc::Server> server_;
    grpc::ServerBuilder serverBuilder;
    ServiceProviderImpl* serviceProviderImpl;
    BalancerEstablisher* balancer;
    grpc::SslServerCredentialsOptions sslOps;

    bool useAuthorization = false;
    std::string _key;
    std::string _cert;
    std::string _root;
};

#endif //INC_CLOUD_CLOUD_SERVER_H
