//
// Created by Jakub Klemens on 10/01/2019.
//

#include "ClientSample.h"

int main(int argc, char *argv[])
{
    std::vector<std::string> vec{"MeshSmooth"};
    std::string balancerAddress = std::string(argv[1]);
    CloudClient::CloudMesh loadedMesh;

    std::string fileName = "./client/config.txt";
    CloudClient client(fileName, vec);

    std::vector<CloudClient::CloudMesh> mesh;
    client.loadMeshFromFile("./Samples/a380.obj", loadedMesh);
    mesh.push_back(loadedMesh);
    std::vector<CloudClient::CloudMesh> cloudMesh;

    client.performModelsToModelsOperation("MeshSmooth", mesh, cloudMesh);
    for (auto mesh1 : cloudMesh)
    {
        client.saveMeshToFile("./Samples/Controller.obj", mesh1);
    }

    return 0;
}
