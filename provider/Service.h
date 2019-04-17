//
// Created by Jakub Klemens on 07/03/2019.
//

#ifndef INC_3DCLOUD_SERVICES_H
#define INC_3DCLOUD_SERVICES_H

#include <iostream>
#include <string>

#include "Shared/ModelProcessor.h"

class Service
{
public:

    const std::vector<std::string> &getIncomingModels() const;
    const std::vector<ModelProcessor::CloudMesh> &getIncomingMeshModels() const;
    void setOutgoingModels(const std::vector<std::string> &outgoingModels);
    void addOutgoingModel(const std::string &outgoingModel);
    const std::string &getName() const;


protected:
    Service() = default;
    ~Service() = default;

    std::string name_;

    std::vector<std::string> incomingModels_;
    std::vector<std::string> outgoingModels_;
    std::vector<ModelProcessor::CloudMesh> incomingMeshModels_;
};

class ModelToModelService : public Service
{
public:
    ModelToModelService(std::string &name_, void (*fp)(ModelToModelService *input))
    : fp_(fp) {name_ = name_;}

    void setOutgoingMeshModels(const std::vector<ModelProcessor::CloudMesh> &outgoingMeshModels);
    void addOutgoingMeshModel(const ModelProcessor::CloudMesh &outgoingMeshModel);

private:
    void (*fp_)(ModelToModelService *input);
    std::vector<ModelProcessor::CloudMesh> outgoingMeshModels_;

    friend class ServiceProviderImpl;

};

class ModelToNumberService : public Service
{
public:
    ModelToNumberService(std::string &name_, void (*fp)(ModelToNumberService *input))
    : fp_(fp) {name_ = name_;}

    void setOutgoingVector(const std::vector<double> &outgoingVector);
    void addOutgoingPoint(const double &point);

private:
    void (*fp_)(ModelToNumberService *input);
    std::vector<double> outgoingVector_;

    friend class ServiceProviderImpl;
};


#endif //INC_3DCLOUD_SERVICES_H
