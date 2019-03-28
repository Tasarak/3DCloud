//
// Created by Jakub Klemens on 20/08/2018.
//

#ifndef INC_3DCLOUD_MODELMANAGER_H
#define INC_3DCLOUD_MODELMANAGER_H

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/ArrayKernel.hh>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <log4cplus/loglevel.h>

#include "cloud_services.grpc.pb.h"

class ModelManager
{
public:
    typedef OpenMesh::PolyMesh_ArrayKernelT<>  CloudMesh;
    ModelManager() = default;
    void serializeModel(Cloud3D::OpenMeshModel &model, CloudMesh &mesh);
    void deserializeModel(CloudMesh &mesh, Cloud3D::Mesh &model);
};


#endif //INC_3DCLOUD_MODELMANAGER_H
