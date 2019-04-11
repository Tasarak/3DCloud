//
// Created by Jakub Klemens on 10/08/2018.
//

#ifndef INC_3DCLOUD_MODELPROCESSOR_H
#define INC_3DCLOUD_MODELPROCESSOR_H

#include <iostream>
#include <string>
#include <vector>

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

#include <cloud_services.pb.h>
#include "cloud_services.grpc.pb.h"

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>


class ModelProcessor
{
public:
    typedef OpenMesh::PolyMesh_ArrayKernelT<> CloudMesh;

    ModelProcessor() = default;
    void serializeModel(Cloud3D::OpenMeshModel &model, CloudMesh &mesh);
    void deserializeModel(CloudMesh &mesh, Cloud3D::Mesh &model);
    void saveModel(ModelProcessor::CloudMesh &mesh, std::string &outputFile);
    void loadModel(ModelProcessor::CloudMesh &mesh, std::string &inputFile);
};


#endif //INC_3DCLOUD_MODELPROCESSOR_H
