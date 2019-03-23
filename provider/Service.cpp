//
// Created by Jakub Klemens on 07/03/2019.
//

#include "Service.h"

const std::vector<std::string> &Service::getIncomingModels() const
{
    return incomingModels;
}

const std::vector<ModelManager::CloudMesh> &Service::getIncomingMeshModels() const
{
    return incomingMeshModels;
}

void Service::setOutgoingModels(const std::vector<std::string> &outgoingModels)
{
    Service::outgoingModels = outgoingModels;
}

const std::string &Service::getName() const
{
    return name;
}

void ModelToModelService::setOutgoingMeshModels(const std::vector<ModelManager::CloudMesh> &outgoingMeshModels)
{
    ModelToModelService::outgoingMeshModels = outgoingMeshModels;
}

void ModelToNumberService::setOutgoingVector(const std::vector<float> &outgoingVector)
{
    ModelToNumberService::outgoingVector = outgoingVector;
}