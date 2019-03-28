//
// Created by Jakub Klemens on 06/08/2018.
//

#include <cloud_services.pb.h>
#include "CloudClient.h"

using Cloud3D::ServiceProvide;
using Cloud3D::Model;
using Cloud3D::VectorofNumbers;
using Cloud3D::OpenMeshModel;

using grpc::Status;
using grpc::ClientContext;

void CloudClient::read(const std::string &filename, std::string &data)
{
    std::ifstream file(filename.c_str(), std::ios::in);

    if (file.is_open())
    {
        std::stringstream ss;
        ss << file.rdbuf();

        file.close();

        data = ss.str();
    }
}

CloudClient::CloudClient(std::string &balancerAddress, std::vector<std::string> services)
{
    ProviderFinder finder(balancerAddress);

    std::string providerAddress = finder.GetServer(services);
    auto channel = grpc::CreateChannel(providerAddress, grpc::InsecureChannelCredentials());
    stub_ = Cloud3D::ServiceProvide::NewStub(channel);
}

CloudClient::CloudClient(std::string &balancerAddress,
                         std::vector<std::string> services,
                         std::string &certFilename,
                         std::string &keyFilename,
                         std::string &rootFilename)
{
    ProviderFinder finder(balancerAddress);

    std::string cert;
    std::string key;
    std::string root;

    read(certFilename, cert);
    read(keyFilename, key);
    read(rootFilename, root);

    grpc::SslCredentialsOptions opts = {root, key, cert};

    auto channel_creds = grpc::SslCredentials(grpc::SslCredentialsOptions(opts));

    std::string providerAddress = finder.GetServer(services);
    auto channel = grpc::CreateChannel(providerAddress, channel_creds);

    stub_ = Cloud3D::ServiceProvide::NewStub(channel);
}

int CloudClient::performModelsToModelsOperation(std::string serviceName,
                                                std::vector<std::string> &outgoingModels,
                                                std::vector<std::string> &incomingModels)
{
    ClientContext ctx;
    Model outModel;
    Model incModel;

    outModel.set_operation(serviceName);
    for (auto &model : outgoingModels)
    {
        outModel.add_data(model);
    }

    Status status = stub_->ModelstoModels(&ctx, outModel, &incModel);

    for (auto &model : incModel.data())
    {
        incomingModels.push_back(model);
    }

    if (status.ok())
    {
        std::cout << "ModelsToModel Successful\n";
        return 0;
    }
    else
    {
        std::cout << "ModelsToModel Failed\n";
        return 1;
    }
}

int CloudClient::performModelsToNumbersOperation(std::string serviceName,
                                                 std::vector<std::string> &outgoingModels,
                                                 std::vector<float> &incomingNumbers)
{
    ClientContext ctx;
    Model outModel;
    VectorofNumbers incNumbers;

    outModel.set_operation(serviceName);
    for (auto &model : outgoingModels)
    {
        outModel.add_data(model);
    }

    Status status = stub_->ModelstoNumbers(&ctx, outModel, &incNumbers);

    for (auto &number : incNumbers.point())
    {
        incomingNumbers.push_back(number);
    }

    if (status.ok())
    {
        std::cout << "ModelsToNumbers Successful\n";
        return 0;
    }
    else
    {
        std::cout << "ModelsToNumbers Failed\n";

        return 1;
    }
}

int CloudClient::performModelsToModelsOperation(std::string serviceName,
                                                std::vector<CloudMesh> &outgoingModels,
                                                std::vector<CloudMesh> &incomingModels)
{
    ClientContext ctx;
    OpenMeshModel outModel;
    OpenMeshModel incModel;
    ModelProcessor modelProcessor;

    outModel.set_operation(serviceName);

    for (auto &model : outgoingModels)
    {
        modelProcessor.serializeModel(outModel, model);
    }

    Status status = stub_->MeshtoMesh(&ctx, outModel, &incModel);

    if (status.ok())
    {
        for (auto model : incModel.mesh())
        {
            CloudMesh mesh;
            modelProcessor.deserializeModel(mesh, model);
            incomingModels.push_back(mesh);
            modelProcessor.saveModel(mesh);
        }

        std::cout << "MeshtoMesh Successful\n";
        return 0;
    }
    else
    {
        std::cout << "MeshtoMesh Failed\n";

        return 1;
    }
}

int CloudClient::performModelsToNumbersOperation(std::string serviceName,
                                                 std::vector<CloudClient::CloudMesh> &outgoingModels,
                                                 std::vector<float> &incomingNumbers)
{
    ClientContext ctx;
    OpenMeshModel outModel;
    VectorofNumbers incNumbers;
    ModelProcessor modelProcessor;

    outModel.set_operation(serviceName);
    for (auto &model : outgoingModels)
    {
        modelProcessor.serializeModel(outModel, model);
    }

    Status status = stub_->MeshtoNumbers(&ctx, outModel, &incNumbers);

    for (auto &number : incNumbers.point())
    {
        incomingNumbers.push_back(number);
    }

    if (status.ok())
    {
        std::cout << "ModelsToNumbers Successful\n";
        return 0;
    }
    else
    {
        std::cout << "ModelsToNumbers Failed\n";

        return 1;
    }
}

ModelProcessor::CloudMesh CloudClient::generateCube()
{
    ModelProcessor processor;
    return processor.generateCube();
}
