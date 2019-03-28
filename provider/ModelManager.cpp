//
// Created by Jakub Klemens on 20/08/2018.
//

#include "ModelManager.h"

void ModelManager::serializeModel(Cloud3D::OpenMeshModel &model, CloudMesh &mesh)
{
    Cloud3D::Mesh* meshModel = model.add_mesh();
    /*

    for (CloudMesh::FaceIter f_it=mesh.faces_begin(); f_it!=mesh.faces_end(); ++f_it)
    {
        Cloud3D::Face* face = meshModel->add_face();

        for (CloudMesh::FaceVertexIter fv_it=mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
        {
            CloudMesh::Point point = mesh.point(fv_it);

            Cloud3D::Vertex* vertex = face->add_vertex();

            vertex->set_x(point[0]);
            vertex->set_y(point[1]);
            vertex->set_z(point[2]);
        }
    }
*/

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

void ModelManager::deserializeModel(CloudMesh &mesh, Cloud3D::Mesh &model)
{
    std::vector<CloudMesh::VertexHandle> vhandle;
    /*

    for (auto face : model.face())
    {
        vhandle.clear();

        for (auto vertex : face.vertex())
        {
            vhandle.push_back(mesh.add_vertex(CloudMesh::Point(vertex.x(), vertex.y(), vertex.z())));
        }

        mesh.add_face(vhandle);
    }
     */

    std::vector<Cloud3D::Vertex> verticies;

    for (const auto &vertex : model.vertex())
    {
        verticies.push_back(vertex);
    }

    for (Cloud3D::Face face : model.face())
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