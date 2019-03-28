//
// Created by Jakub Klemens on 10/01/2019.
//

#include "ClientTest.h"

int main(int argc, char *argv[])
{
    ::log4cplus::initialize();
    ::log4cplus::PropertyConfigurator::doConfigure("./Shared/log4cplus_configure.ini");

    std::vector<std::string> vec{"MeshSmooth"};
    std::string balancerAddress = std::string(argv[1]);

    //CloudClient client(balancerAddress, vec);
    std::string fileName = "./client/config.txt";
    CloudClient client(fileName, vec);
/*
    std::vector<std::string> outModels{"Test1", "Test2"};
    std::vector<std::string> incModels;
    client.performModelsToModelsOperation("Service1", outModels, incModels);

    for (auto model : incModels)
    {
        std::cout << model << std::endl;
    }
*/

    std::vector<CloudClient::CloudMesh> mesh;
    mesh.push_back(client.generateCube());
    mesh.push_back(client.generateCube());
    std::vector<CloudClient::CloudMesh> cloudMesh;
    client.performModelsToModelsOperation("MeshSmooth", mesh, cloudMesh);

    return 0;
}
