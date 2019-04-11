//
// Created by Jakub Klemens on 10/08/2018.
//


#include "ModelProcessor.h"

void ModelProcessor::saveModel(ModelProcessor::CloudMesh &mesh, std::string &outputFile)
{
    try
    {
        OpenMesh::IO::Options opt = OpenMesh::IO::Options::Default;

        if (!OpenMesh::IO::write_mesh(mesh, outputFile))
        {
            std::cerr << "Cannot write mesh_ to stream" << std::endl;
        }
    }
    catch( std::exception& x )
    {
        std::cerr << x.what() << std::endl;
       // LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("") << x.what());
    }
}

void ModelProcessor::loadModel(ModelProcessor::CloudMesh &mesh, std::string &inputFile)
{
    try
    {
        OpenMesh::IO::read_mesh(mesh, inputFile);
    }
    catch (std::exception& x)
    {
        throw;
    }

}

void ModelProcessor::serializeModel(Cloud3D::OpenMeshModel &model, ModelProcessor::CloudMesh &mesh)
{
    Cloud3D::Mesh* meshModel = model.add_mesh();

    for (CloudMesh::VertexIter v_it=mesh.vertices_begin(); v_it!=mesh.vertices_end(); ++v_it)
    {
        Cloud3D::Vertex *vertex = meshModel->add_vertex();

        CloudMesh::Point point = mesh.point(v_it);

        vertex->set_x(point[0]);
        vertex->set_y(point[1]);
        vertex->set_z(point[2]);
    }

    for (CloudMesh::FaceIter f_it=mesh.faces_begin(); f_it!=mesh.faces_end(); ++f_it)
    {
        Cloud3D::Face* face = meshModel->add_face();

        for (CloudMesh::FaceVertexIter fv_it=mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
        {
            face->add_index(u_int32_t (fv_it->idx()));
        }
    }
}

void ModelProcessor::deserializeModel(ModelProcessor::CloudMesh &mesh,
                                     Cloud3D::Mesh &model)
{
    std::vector<CloudMesh::VertexHandle> vhandle;
    std::vector<Cloud3D::Vertex> verticies;

    for (const auto &vertex : model.vertex())
    {
        verticies.push_back(vertex);
    }

    for (const Cloud3D::Face &face : model.face())
    {
        vhandle.clear();
        for (auto index : face.index())
        {
            CloudMesh::Point vertex = CloudMesh::Point(verticies[index].x(), verticies[index].y(), verticies[index].z());
            vhandle.push_back(mesh.add_vertex(vertex));
        }
        mesh.add_face(vhandle);
    }
}