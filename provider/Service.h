//
// Created by Jakub Klemens on 07/03/2019.
//

#ifndef INC_3DCLOUD_SERVICES_H
#define INC_3DCLOUD_SERVICES_H

#include <iostream>
#include <string>

#include "ModelManager.h"

class Service
{
public:

    const std::vector<std::string> &getIncomingModels() const;
    const std::vector<ModelManager::CloudMesh> &getIncomingMeshModels() const;
    void setOutgoingModels(const std::vector<std::string> &outgoingModels);
    const std::string &getName() const;

protected:
    Service() = default;
    ~Service() = default;

    std::string name;

    std::vector<std::string> incomingModels;
    std::vector<std::string> outgoingModels;
    std::vector<ModelManager::CloudMesh> incomingMeshModels;
};

class ModelToModelService : public Service
{
public:
    ModelToModelService(std::string &name_, void (*fp)(ModelToModelService *input))
    : fp(fp) {name = name_;}

    void setOutgoingMeshModels(const std::vector<ModelManager::CloudMesh> &outgoingMeshModels);

private:
    void (*fp)(ModelToModelService *input);
    std::vector<ModelManager::CloudMesh> outgoingMeshModels;

    friend class ServiceProviderImpl;

};

class ModelToNumberService : public Service
{
public:
    ModelToNumberService(std::string &name_, void (*fp)(ModelToNumberService *input))
    : fp(fp) {name = name_;}

    void setOutgoingVector(const std::vector<float> &outgoingVector);

private:
    void (*fp)(ModelToNumberService *input);
    std::vector<float> outgoingVector;

    friend class ServiceProviderImpl;
};


#endif //INC_3DCLOUD_SERVICES_H
