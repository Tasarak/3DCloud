//
// Created by Jakub Klemens on 06/08/2018.
//

#ifndef INC_CLOUD_CLOUD_SERVER_H
#define INC_CLOUD_CLOUD_SERVER_H

#include <stdexcept>
#include <cloud_services.grpc.pb.h>
#include <cloud_services.pb.h>

#include "BalancerEstablisher.h"
#include "ServiceProviderImpl.h"

class ServiceProvider
{
public:
    ServiceProvider();
    ~ServiceProvider();
    int Run(std::string serverAddress);

    void setModelsToModelsService(ModelToModelService);
    void setModelsToNumbersService(ModelToNumberService);

private:
    std::unique_ptr<grpc::ServerCompletionQueue> cq_;
    std::unique_ptr<grpc::Server> server_;
    ServiceProviderImpl* serviceProviderImpl;
};

#endif //INC_CLOUD_CLOUD_SERVER_H
