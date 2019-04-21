//
// Created by Jakub Klemens on 10/01/2019.
//

#include "ClientSample.h"

int main(int argc, char *argv[])
{
    std::vector<std::string> vec{"MeshSmooth"};
    std::string inputFile = std::string(argv[1]);
    std::string outputFile = std::string(argv[2]);
    std::string fileName = std::string(argv[3]);
    CloudClient::CloudMesh loadedMesh;

    CloudClient client(fileName, vec);

    std::vector<CloudClient::CloudMesh> mesh;
    client.loadMeshFromFile(inputFile, loadedMesh);
    mesh.push_back(loadedMesh);
    std::vector<CloudClient::CloudMesh> cloudMesh;

    client.performModelsToModelsOperation("MeshSmooth", mesh, cloudMesh);
    for (auto mesh1 : cloudMesh)
    {
        client.saveMeshToFile(outputFile, mesh1);
    }

    return 0;
}
