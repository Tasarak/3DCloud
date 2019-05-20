//
// Created by Jakub Klemens on 10/01/2019.
//

#include "ProviderSample.h"
static unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;

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

void init(){
    FILE* file = fopen("/proc/stat", "r");
    fscanf(file, "cpu %llu %llu %llu %llu", &lastTotalUser, &lastTotalUserLow,
           &lastTotalSys, &lastTotalIdle);
    fclose(file);
}

void getCurrentValue(int &usage){
    double percent;
    FILE* file;
    unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;

    file = fopen("/proc/stat", "r");
    fscanf(file, "cpu %llu %llu %llu %llu", &totalUser, &totalUserLow,
           &totalSys, &totalIdle);
    fclose(file);

    if (totalUser < lastTotalUser || totalUserLow < lastTotalUserLow ||
        totalSys < lastTotalSys || totalIdle < lastTotalIdle){
        //Overflow detection. Just skip this value.
        percent = -1.0;
    }
    else{
        total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) +
                (totalSys - lastTotalSys);
        percent = total;
        total += (totalIdle - lastTotalIdle);
        percent /= total;
        percent *= 100;
    }

    lastTotalUser = totalUser;
    lastTotalUserLow = totalUserLow;
    lastTotalSys = totalSys;
    lastTotalIdle = totalIdle;

    usage = percent;
}

int main(int argc, char *argv[])
{
    std::string configFile = std::string(argv[1]);

    ServiceProvider* provider = new ServiceProvider(configFile);
    std::string serviceName = "MeshSmooth";
    ModelToModelService service(serviceName, meshSmooth);
    provider->setModelsToModelsService(service);
    provider->setUsageFunction(getCurrentValue);

    provider->run();

    return 0;
}