//
// Created by Jakub Klemens on 15/01/2019.
//

#include "ServiceProviderImpl.h"

ServiceProviderImpl& ServiceProviderImpl::getInstance()
{
    static ServiceProviderImpl instance;
    return instance;
}

::grpc::Status ServiceProviderImpl::ModelstoModels(::grpc::ServerContext *context,
                                                   const ::Cloud3D::Model *request,
                                                   ::Cloud3D::Model *response)
{
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("ServiceProvider"));
    for (auto service : ModelsToModelsServices)
    {
        if (request->operation() == service.getName())
        {
            for (auto model : request->data())
            {
                service.incomingModels_.push_back(model);
            }
            LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Start of execution of Service: ") << service.getName());
            service.fp_(&service);
            LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("End of execution of Service: ") << service.getName());
            for (auto model : service.outgoingModels_)
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
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("ServiceProvider"));
    for (auto service : ModelToNumbersServices)
    {
        if (request->operation() == service.name_)
        {
            for (auto model : request->data())
            {
                service.incomingModels_.push_back(model);
            }
            LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Start of execution of Service: ") << service.getName());
            service.fp_(&service);
            LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("End of execution of Service: ") << service.getName());
            for (auto number : service.outgoingVector_)
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
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("ServiceProvider"));
    ModelProcessor modelManager;
    ModelProcessor::CloudMesh mesh;

    for (auto service : ModelsToModelsServices)
    {
        if (request->operation() == service.name_)
        {
            for (auto model : request->mesh())
            {
                mesh.clear();
                modelManager.deserializeModel(mesh, model);
                service.incomingMeshModels_.push_back(mesh);
            }
            LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Start of execution of Service: ") << service.getName());
            service.fp_(&service);
            LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("End of execution of Service: ") << service.getName());
            for (auto meshOut : service.outgoingMeshModels_)
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
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("ServiceProvider"));
    ModelProcessor modelManager;
    ModelProcessor::CloudMesh mesh;

    for (auto service : ModelToNumbersServices)
    {
        if (request->operation() == service.name_)
        {
            for (auto model : request->mesh())
            {
                mesh.clear();
                modelManager.deserializeModel(mesh, model);
                service.incomingMeshModels_.push_back(mesh);
            }
            LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Start of execution of Service: ") << service.getName());
            service.fp_(&service);
            LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("End of execution of Service: ") << service.getName());
            for (auto number : service.outgoingVector_)
            {
                response->add_point(number);
            }
            return ::grpc::Status::OK;
        }
    }
    return ::grpc::Status::CANCELLED;
}