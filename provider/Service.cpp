//
// Created by Jakub Klemens on 07/03/2019.
//

#include "Service.h"

const std::vector<std::string> &Service::getIncomingModels() const
{
    return incomingModels;
}

const std::vector<ModelProcessor::CloudMesh> &Service::getIncomingMeshModels() const
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

void ModelToModelService::setOutgoingMeshModels(const std::vector<ModelProcessor::CloudMesh> &outgoingMeshModels)
{
    ModelToModelService::outgoingMeshModels = outgoingMeshModels;
}

void ModelToNumberService::setOutgoingVector(const std::vector<double> &outgoingVector)
{
    ModelToNumberService::outgoingVector = outgoingVector;
}

void Service::addOutgoingModel(const std::string &outgoingModel)
{
    Service::outgoingModels.push_back(outgoingModel);
}

void ModelToModelService::addOutgoingMeshModel(const ModelProcessor::CloudMesh &outgoingMeshModel)
{
    ModelToModelService::outgoingMeshModels.push_back(outgoingMeshModel);
}

void ModelToNumberService::addOutgoingPoint(const double &point)
{
    ModelToNumberService::outgoingVector.push_back(point);
}