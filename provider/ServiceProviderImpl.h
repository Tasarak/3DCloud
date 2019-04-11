//
// Created by Jakub Klemens on 15/01/2019.
//

#ifndef INC_3DCLOUD_SERVICEPROVIDERIMPL_H
#define INC_3DCLOUD_SERVICEPROVIDERIMPL_H

#include <memory>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <log4cplus/loglevel.h>
#include <cloud_services.pb.h>
#include <cloud_services.grpc.pb.h>

#include "Service.h"

class ServiceProviderImpl final : public Cloud3D::ServiceProvide::Service
{
public:


    static ServiceProviderImpl& GetInstance();
    ServiceProviderImpl(ServiceProviderImpl const&) = delete;
    void operator=(ServiceProviderImpl const&) = delete;

    std::vector<ModelToModelService> ModelsToModelsServices;
    std::vector<ModelToNumberService> ModelToNumbersServices;
    std::vector<std::string> ListOfServiceNames;

private:
    explicit ServiceProviderImpl() = default;
    ~ServiceProviderImpl() {};
    ::grpc::Status ModelstoModels(::grpc::ServerContext *context,
                                  const ::Cloud3D::Model *request,
                                  ::Cloud3D::Model *response) override;
    ::grpc::Status ModelstoNumbers(::grpc::ServerContext *context,
                                   const ::Cloud3D::Model *request,
                                   ::Cloud3D::VectorofNumbers *response) override;
    ::grpc::Status MeshtoMesh(::grpc::ServerContext *context,
                              const ::Cloud3D::OpenMeshModel *request,
                              ::Cloud3D::OpenMeshModel *response) override;
    ::grpc::Status MeshtoNumbers(::grpc::ServerContext* context,
                                 const ::Cloud3D::OpenMeshModel* request,
                                 ::Cloud3D::VectorofNumbers* response) override;
    /*
    ::grpc::Status StreamModelstoModels(::grpc::ServerContext *context,
                                        ::grpc::ServerReaderWriter<::Cloud3D::Model, ::Cloud3D::Model> *stream) override;
    ::grpc::Status StreamModelstoNumbers(::grpc::ServerContext *context,
                                         ::grpc::ServerReader<::Cloud3D::Model> *reader,
                                         ::Cloud3D::VectorofNumbers *response) override;
    ::grpc::Status StreamMeshtoNumbers(::grpc::ServerContext *context,
                                       ::grpc::ServerReader<::Cloud3D::OpenMeshModel> *reader,
                                       ::Cloud3D::VectorofNumbers *response) override;
                                       */
    ::grpc::Status StreamMeshtoMesh(::grpc::ServerContext *context,
                                    ::grpc::ServerReaderWriter<::Cloud3D::OpenMeshModel, ::Cloud3D::OpenMeshModel> *stream) override;
};


#endif //INC_3DCLOUD_SERVICEPROVIDERIMPL_H
