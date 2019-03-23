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
/*
void CloudClient::UpdateModel(const std::string operation)
{
    Model model;
    model.set_operation(operation);

    AsyncClientCall *call = new AsyncClientCall;

    ModelProcessor processor(model);
    processor.generateCube();
    processor.prepareModel();

    call->responseReader = stub_->PrepareAsyncModelstoModels(&call->context, model, &cq_);
    call->responseReader->StartCall();
    call->responseReader->Finish(&call->updatedModel, &call->status, (void*)call);

    void * got_tag;
    bool ok = false;

    GPR_ASSERT(cq_.Next(&got_tag, &ok));

    call = static_cast<AsyncClientCall*>(got_tag);

    GPR_ASSERT(ok);

    if (call->status.ok())
    {
        processor.saveModel(call->updatedModel.data(0));
    }
    else
    {
        std::cout << "RPC failed" << std::endl;
    }

    delete call;
}
*/

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
