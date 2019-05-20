//
// Created by Jakub Klemens on 07/03/2019.
//

#include "Service.h"

const std::vector<std::string> &Service::getIncomingModels() const
{
    return incomingModels_;
}

const std::vector<ModelProcessor::CloudMesh> &Service::getIncomingMeshModels() const
{
    return incomingMeshModels_;
}

void Service::setOutgoingModels(const std::vector<std::string> &outgoingModels)
{
    Service::outgoingModels_ = outgoingModels;
}

const std::string &Service::getName() const
{
    return name_;
}

void ModelToModelService::setOutgoingMeshModels(const std::vector<ModelProcessor::CloudMesh> &outgoingMeshModels)
{
    ModelToModelService::outgoingMeshModels_ = outgoingMeshModels;
}

void ModelToNumberService::setOutgoingPoints(const std::vector<double> &outgoingVector)
{
    ModelToNumberService::outgoingVector_ = outgoingVector;
}

void Service::addOutgoingModel(const std::string &outgoingModel)
{
    Service::outgoingModels_.push_back(outgoingModel);
}

void ModelToModelService::addOutgoingMeshModel(const ModelProcessor::CloudMesh &outgoingMeshModel)
{
    ModelToModelService::outgoingMeshModels_.push_back(outgoingMeshModel);
}

void ModelToNumberService::addOutgoingPoint(const double &point)
{
    ModelToNumberService::outgoingVector_.push_back(point);
}