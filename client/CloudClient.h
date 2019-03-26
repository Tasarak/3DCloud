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

#include "cloud_services.grpc.pb.h"
#include "ProviderFinder.h"
#include "ModelProcessor.h"

class CloudClient
{
public:
    typedef OpenMesh::PolyMesh_ArrayKernelT<> CloudMesh;

    CloudClient(std::string &balancerAddress, std::vector<std::string> services);

    CloudClient(std::string &balancerAddress,
                std::vector<std::string> services,
                std::string &certFilename,
                std::string &keyFilename,
                std::string &rootFilename);
    ~CloudClient() = default;
    void UpdateModel(std::string);
    int performModelsToModelsOperation(std::string serviceName,
                                       std::vector<std::string> &outgoingModels,
                                       std::vector<std::string> &incomingModels);

    int performModelsToModelsOperation(std::string serviceName,
                                       std::vector<CloudMesh> &outgoingModels,
                                       std::vector<CloudMesh> &incomingModels);

    int performModelsToNumbersOperation(std::string serviceName,
                                        std::vector<std::string> &outgoingModels,
                                        std::vector<float> &incomingNumbers);

    int performModelsToNumbersOperation(std::string serviceName,
                                        std::vector<CloudMesh> &outgoingModels,
                                        std::vector<float> &incomingNumbers);
    ModelProcessor::CloudMesh generateCube();

private:
    struct AsyncClientCall
    {
        Cloud3D::Model updatedModel;
        grpc::ClientContext context;
        grpc::Status status;
        std::unique_ptr<grpc::ClientAsyncResponseReader<Cloud3D::Model>> responseReader;
    };

    void read(const std::string &filename, std::string &data);
    std::unique_ptr<Cloud3D::ServiceProvide::Stub> stub_;
    grpc::CompletionQueue cq_;
};

#endif //INC_CLOUD_CLOUD_CLIENT_H
