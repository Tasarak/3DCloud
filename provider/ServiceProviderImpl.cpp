//
// Created by Jakub Klemens on 15/01/2019.
//

#include "ServiceProviderImpl.h"

ServiceProviderImpl& ServiceProviderImpl::GetInstance()
{
    static ServiceProviderImpl instance;
    return instance;
}

::grpc::Status ServiceProviderImpl::ModelstoModels(::grpc::ServerContext *context,
                                                   const ::Cloud3D::Model *request,
                                                   ::Cloud3D::Model *response)
{
    for (auto service : ModelsToModelsServices)
    {
        if (request->operation() == service.getName())
        {
            for (auto model : request->data())
            {
                service.incomingModels.push_back(model);
            }
            service.fp(&service);
            for (auto model : service.outgoingModels)
            {
                response->add_data(model);
            }
            return ::grpc::Status::OK;
        }
    }
    return ::grpc::Status::CANCELLED;
}

::grpc::Status ServiceProviderImpl::ModelstoNumbers(::grpc::ServerContext *context,
                                                    const ::Cloud3D::Model *request,
                                                    ::Cloud3D::VectorofNumbers *response)
{
    for (auto service : ModelToNumbersServices)
    {
        if (request->operation() == service.name)
        {
            for (auto model : request->data())
            {
                service.incomingModels.push_back(model);
            }
            service.fp(&service);
            for (auto number : service.outgoingVector)
            {
                response->add_point(number);
            }
            return ::grpc::Status::OK;
        }
    }

    return  ::grpc::Status::CANCELLED;
}

::grpc::Status ServiceProviderImpl::MeshtoMesh(::grpc::ServerContext *context,
                                               const ::Cloud3D::OpenMeshModel *request,
                                               ::Cloud3D::OpenMeshModel *response)
{
    ModelManager modelManager;
    ModelManager::CloudMesh mesh;
    for (auto service : ModelsToModelsServices)
    {
        if (request->operation() == service.name)
        {
            for (auto model : request->mesh())
            {
                mesh.clear();
                modelManager.deserializeModel(mesh, model);
                service.incomingMeshModels.push_back(mesh);
            }
            service.fp(&service);
            for (auto meshOut : service.outgoingMeshModels)
            {
                modelManager.serializeModel(*response, meshOut);
            }
            return ::grpc::Status::OK;
        }
    }
    return ::grpc::Status::CANCELLED;
}

::grpc::Status ServiceProviderImpl::MeshtoNumbers(::grpc::ServerContext *context,
                                                  const ::Cloud3D::OpenMeshModel *request,
                                                  ::Cloud3D::VectorofNumbers *response)
{
    return ::grpc::Status::OK;
}
