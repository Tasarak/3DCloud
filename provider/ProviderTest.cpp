//
// Created by Jakub Klemens on 10/01/2019.
//

#include "ProviderTest.h"

void foo(ModelToModelService *service)
{
    service->setOutgoingMeshModels(service->getIncomingMeshModels());
    std::cout << service->getName() << "\n";
}

int main(int argc, char *argv[])
{
    ::log4cplus::initialize();
    ::log4cplus::PropertyConfigurator::doConfigure("./Shared/log4cplus_configure.ini");

    ServiceProvider* provider = new ServiceProvider(argv[1], argv[2]);
    std::string serviceName = "Service";
    ModelToModelService service(serviceName, foo);
    provider->setModelsToModelsService(service);

    provider->Run();

    return 0;
}