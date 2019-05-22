//
// Created by Jakub Klemens on 06/08/2018.
//

#ifndef INC_CLOUD_CLOUD_CLIENT_H
#define INC_CLOUD_CLOUD_CLIENT_H

#include <memory>
#include <iostream>
#include <string>
#include <thread>
#include <cstdlib>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

#include "ProviderFinder.h"
#include "Shared/ModelProcessor.h"
#include "Shared/FileParser.h"

#define MAX_STREAM_VALUE 4 * 1024

class CloudClient
{
public:
    typedef OpenMesh::PolyMesh_ArrayKernelT<> CloudMesh;

    CloudClient(std::string &balancerAddress, float &minVersion, std::vector<std::string> &services);

    CloudClient(std::string &balancerAddress,
                float &minVersion,
                std::vector<std::string> &services,
                std::string &certFilename,
                std::string &keyFilename,
                std::string &rootFilename);

    CloudClient(std::string &configFile, std::vector<std::string> &services);
    ~CloudClient() = default;
    int performOperation(std::string serviceName,
                         std::vector<std::string> &outgoingModels,
                         std::vector<std::string> &incomingModels);

    int performOperation(std::string serviceName,
                         std::vector<CloudMesh> &outgoingModels,
                         std::vector<CloudMesh> &incomingModels);

    int performOperation(std::string serviceName,
                         std::vector<std::string> &outgoingModels,
                         std::vector<double> &incomingNumbers);

    int performOperation(std::string serviceName,
                         std::vector<CloudMesh> &outgoingModels,
                         std::vector<double> &incomingNumbers);
    static void loadMeshFromFile(std::string file, CloudMesh &loadedMesh);
    static void saveMeshToFile(std::string file, CloudMesh &loadedMesh);

private:
    int initWithSSL();
    int init();
    ::grpc::Status streamData(std::string &model, std::string &outModel, Cloud3D::OperationType type);

    std::unique_ptr<Cloud3D::ServiceProvide::Stub> stub_;
    std::vector<std::string> services_;
    std::string balancerAddress_, certFilename_, keyFilename_, rootFilename_ = "";
    float minVersion_ =  0.0;
};

#endif //INC_CLOUD_CLOUD_CLIENT_H
