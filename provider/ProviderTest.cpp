//
// Created by Jakub Klemens on 10/01/2019.
//

#include "ProviderTest.h"

void foo(ModelToModelService *service)
{
    service->setOutgoingMeshModels(service->getIncomingMeshModels());
    std::cout << service << "\n";
}

int main(int argc, char *argv[])
{
    ::log4cplus::initialize();
    ::log4cplus::PropertyConfigurator::doConfigure("./Shared/log4cplus_configure.ini");

    ServiceProvider* server = new ServiceProvider();
    std::string serviceName = "Service";
    ModelToModelService service(serviceName, foo);
    server->setModelsToModelsService(service);

    BalancerEstablisher* balancer;
    balancer = new BalancerEstablisher(argv[1], grpc::CreateChannel(argv[2],
                                                                    grpc::InsecureChannelCredentials()));
    std::thread first(&BalancerEstablisher::SendHeartBeat, balancer);
    std::thread second(&ServiceProvider::Run, server, argv[1]);

    first.join();
    second.join();

    return 0;
}