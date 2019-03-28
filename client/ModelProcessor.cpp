//
// Created by Jakub Klemens on 10/08/2018.
//


#include "ModelProcessor.h"

/*
int ModelProcessor::prepareModel()
{
    // write mesh to output.obj
    try
    {
        std::ostringstream stream;
        if (!OpenMesh::IO::write_mesh(mesh_, stream, ".OFF", OpenMesh::IO::Options::Default))
        {
            std::cerr << "Cannot write mesh to stream" << std::endl;
            LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("Cannot write mesh to stream"));
            return 1;
        }
        else
        {
            model_.add_data(stream.str());
        }
    }
    catch( std::exception& x )
    {
        std::cerr << x.what() << std::endl;
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("") << x.what());
        return 1;
    }
    return 0;
}
*/


ModelProcessor::CloudMesh ModelProcessor::generateCube()
{
    CloudMesh mesh;
    // generate vertices
    CloudMesh::VertexHandle vhandle[8];
    vhandle[0] = mesh.add_vertex(CloudMesh::Point(-1, -1,  1));
    vhandle[1] = mesh.add_vertex(CloudMesh::Point( 1, -1,  1));
    vhandle[2] = mesh.add_vertex(CloudMesh::Point( 1,  1,  1));
    vhandle[3] = mesh.add_vertex(CloudMesh::Point(-1,  1,  1));
    vhandle[4] = mesh.add_vertex(CloudMesh::Point(-1, -1, -1));
    vhandle[5] = mesh.add_vertex(CloudMesh::Point( 1, -1, -1));
    vhandle[6] = mesh.add_vertex(CloudMesh::Point( 1,  1, -1));
    vhandle[7] = mesh.add_vertex(CloudMesh::Point(-1,  1, -1));
    // generate (quadrilateral) faces
    std::vector<CloudMesh::VertexHandle>  face_vhandles;
    face_vhandles.clear();
    face_vhandles.push_back(vhandle[0]);
    face_vhandles.push_back(vhandle[1]);
    face_vhandles.push_back(vhandle[2]);
    face_vhandles.push_back(vhandle[3]);
    mesh.add_face(face_vhandles);

    face_vhandles.clear();
    face_vhandles.push_back(vhandle[7]);
    face_vhandles.push_back(vhandle[6]);
    face_vhandles.push_back(vhandle[5]);
    face_vhandles.push_back(vhandle[4]);
    mesh.add_face(face_vhandles);
    face_vhandles.clear();
    face_vhandles.push_back(vhandle[1]);
    face_vhandles.push_back(vhandle[0]);
    face_vhandles.push_back(vhandle[4]);
    face_vhandles.push_back(vhandle[5]);
    mesh.add_face(face_vhandles);
    face_vhandles.clear();
    face_vhandles.push_back(vhandle[2]);
    face_vhandles.push_back(vhandle[1]);
    face_vhandles.push_back(vhandle[5]);
    face_vhandles.push_back(vhandle[6]);
    mesh.add_face(face_vhandles);
    face_vhandles.clear();
    face_vhandles.push_back(vhandle[3]);
    face_vhandles.push_back(vhandle[2]);
    face_vhandles.push_back(vhandle[6]);
    face_vhandles.push_back(vhandle[7]);
    mesh.add_face(face_vhandles);
    face_vhandles.clear();
    face_vhandles.push_back(vhandle[0]);
    face_vhandles.push_back(vhandle[3]);
    face_vhandles.push_back(vhandle[7]);
    face_vhandles.push_back(vhandle[4]);
    mesh.add_face(face_vhandles);

    return mesh;

}

int ModelProcessor::saveModel(ModelProcessor::CloudMesh &mesh)
{
    try
    {
        OpenMesh::IO::Options opt = OpenMesh::IO::Options::Default;

        if (!OpenMesh::IO::write_mesh(mesh, "output.obj"))
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

void ModelProcessor::serializeModel(Cloud3D::OpenMeshModel &model, ModelProcessor::CloudMesh &mesh)
{
    Cloud3D::Mesh* meshModel = model.add_mesh();

    for (CloudMesh::VertexIter v_it=mesh.vertices_begin(); v_it!=mesh.vertices_end(); ++v_it)
    {
        Cloud3D::Vertex *vertex = meshModel->add_vertex();
        vertex->set_index(u_int32_t (v_it->idx()));

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