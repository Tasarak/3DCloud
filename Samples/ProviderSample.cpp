//
// Created by Jakub Klemens on 10/01/2019.
//

#include "ProviderSample.h"

void meshSmooth(ModelToModelService *service)
{
    for (auto mesh : service->getIncomingMeshModels())
    {
        OpenMesh::Smoother::JacobiLaplaceSmootherT<ModelProcessor::CloudMesh> smoother(mesh);

        smoother.initialize(smoother.Tangential_and_Normal, smoother.C0);
        smoother.smooth(200);

        service->addOutgoingMeshModel(mesh);
    }
}

void UsageFunction(int &usage)
{
    std::random_device rd;     // only used once to initialise (seed) engine
    std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<int> uni(0, 100);
    usage = uni(rng);
}

int main(int argc, char *argv[])
{
    std::string configFile = std::string(argv[1]);

    ServiceProvider* provider = new ServiceProvider(configFile);
    std::string serviceName = "MeshSmooth";
    ModelToModelService service(serviceName, meshSmooth);
    provider->setModelsToModelsService(service);
    provider->setUsageFunction(UsageFunction);

    provider->Run();

    return 0;
}