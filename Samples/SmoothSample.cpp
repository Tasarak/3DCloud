//
// Created by Jakub Klemens on 27/04/2019.
//

#include "SmoothSample.h"

void meshSmooth(ModelProcessor::CloudMesh &mesh)
{
        OpenMesh::Smoother::JacobiLaplaceSmootherT<ModelProcessor::CloudMesh> smoother(mesh);

        smoother.initialize(smoother.Tangential_and_Normal, smoother.C0);
        smoother.smooth(200);
}

int main(int argc, char *argv[])
{
    ::log4cplus::initialize();
    ::log4cplus::PropertyConfigurator::doConfigure("./Shared/log4cplus_configure.ini");
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("CloudClient"));
    ModelProcessor::CloudMesh mesh;
    std::string filename = argv[1];
    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Start of loading model: ") << filename);
    CloudClient::loadMeshFromFile(filename, mesh);
    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("End of loading model: ") << filename);
    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Start of smooth"));
    meshSmooth(mesh);
    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("End of smooth"));
    filename = argv[2];
    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Start of saving model: ") << filename);
    CloudClient::saveMeshToFile(filename, mesh);
    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("End of saving model: ") << filename);

    return 0;
}
