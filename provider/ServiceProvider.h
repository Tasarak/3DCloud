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
#include "../Shared/FileParser.h"

class ServiceProvider
{
public:
    ServiceProvider(std::string providerAddress, std::string balancerAddress, float &version, int &heartBeatRate);
    ServiceProvider(std::string providerAddress,
                    std::string balancerAddress,
                    float &version,
                    int &heartBeatRate,
                    std::string &certFilename,
                    std::string &keyFilename,
                    std::string &rootFilename);
    ServiceProvider(std::string &inputFile);
    ~ServiceProvider();
    int Run();

    void setModelsToModelsService(ModelToModelService);
    void setModelsToNumbersService(ModelToNumberService);
    void setUsageFunction(void (*fp_)(int &));

private:
    void StartServer();
    void init();
    void initWithSSL();

    std::unique_ptr<grpc::ServerCompletionQueue> cq_;
    std::unique_ptr<grpc::Server> server_;
    grpc::ServerBuilder serverBuilder_;
    ServiceProviderImpl* serviceProviderImpl_;
    BalancerEstablisher* balancer_;
    grpc::SslServerCredentialsOptions sslOps_;
    std::string key_;
    std::string cert_;
    std::string root_;
    std::string providerAddress_, balancerAddress_, certFilename_, keyFilename_, rootFilename_ = "";
    float version_ = 0.0;
    int heartBeatRate_ = 1000;
};

#endif //INC_CLOUD_CLOUD_SERVER_H
