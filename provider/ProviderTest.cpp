//
// Created by Jakub Klemens on 10/01/2019.
//

#include "ProviderTest.h"

void meshSmooth(ModelToModelService *service)
{
    std::vector<ModelManager::CloudMesh> outModels;
    for (auto mesh : service->getIncomingMeshModels())
    {
        OpenMesh::Smoother::JacobiLaplaceSmootherT<ModelManager::CloudMesh> smoother(mesh);

        smoother.initialize(smoother.Tangential_and_Normal, smoother.C0);
        smoother.smooth(5);

        outModels.push_back(mesh);
    }

    service->setOutgoingMeshModels(outModels);
}

int main(int argc, char *argv[])
{
    ::log4cplus::initialize();
    ::log4cplus::PropertyConfigurator::doConfigure("./Shared/log4cplus_configure.ini");
    float version = 1.1;
    int heartBeatRate = 2500;

    ServiceProvider* provider = new ServiceProvider(argv[1], argv[2], version, heartBeatRate);
    std::string serviceName = "MeshSmooth";
    ModelToModelService service(serviceName, meshSmooth);
    provider->setModelsToModelsService(service);

    provider->Run();

    return 0;
}