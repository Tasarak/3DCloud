//
// Created by Jakub Klemens on 06/08/2018.
//

#include "CloudClient.h"

using Cloud3D::ServiceProvide;
using Cloud3D::Model;
using Cloud3D::VectorofNumbers;
using Cloud3D::OpenMeshModel;

using grpc::Status;
using grpc::ClientContext;

void CloudClient::init()
{
    ProviderFinder finder(balancerAddress_, minVersion_);

    grpc::ChannelArguments ch_args;
    ch_args.SetMaxReceiveMessageSize(-1);
    std::string providerAddress = finder.GetServer(services_);
    auto channel = grpc::CreateCustomChannel(providerAddress, grpc::InsecureChannelCredentials(), ch_args);
    stub_ = Cloud3D::ServiceProvide::NewStub(channel);
}

void CloudClient::initWithSSL()
{
    ProviderFinder finder(balancerAddress_, minVersion_);

    std::string cert;
    std::string key;
    std::string root;

    FileParser parser;

    parser.read(certFilename_, cert);
    parser.read(keyFilename_, key);
    parser.read(rootFilename_, root);

    grpc::SslCredentialsOptions opts = {root, key, cert};

    auto channel_creds = grpc::SslCredentials(grpc::SslCredentialsOptions(opts));

    std::string providerAddress = finder.GetServer(services_);
    auto channel = grpc::CreateChannel(providerAddress, channel_creds);

    stub_ = Cloud3D::ServiceProvide::NewStub(channel);
}

CloudClient::CloudClient(std::string &balancerAddress, float &minVersion, std::vector<std::string> &services)
                        : balancerAddress_(balancerAddress),
                          minVersion_(minVersion),
                          services_(services)
{
    ::log4cplus::initialize();
    ::log4cplus::PropertyConfigurator::doConfigure("./Shared/log4cplus_configure.ini");
    init();
}

CloudClient::CloudClient(std::string &balancerAddress,
                         float &minVersion,
                         std::vector<std::string> &services,
                         std::string &certFilename,
                         std::string &keyFilename,
                         std::string &rootFilename)
                    :    balancerAddress_(balancerAddress),
                         minVersion_(minVersion),
                         services_(services),
                         certFilename_(certFilename),
                         keyFilename_(keyFilename),
                         rootFilename_(rootFilename)
{
    ::log4cplus::initialize();
    ::log4cplus::PropertyConfigurator::doConfigure("./Shared/log4cplus_configure.ini");
    initWithSSL();
}

CloudClient::CloudClient(std::string &configFile, std::vector<std::string> &services)
                        : services_(services)
{
    FileParser parser;

    try
    {
        parser.parseClientConfig(configFile, balancerAddress_, minVersion_, certFilename_, keyFilename_, rootFilename_);
    }
    catch (...)
    {
        throw;
    }

    if (balancerAddress_.empty())
    {
        throw std::invalid_argument("Address of LoadBalancer not defined in config.");
    }

    if (!certFilename_.empty() && !keyFilename_.empty() && !rootFilename_.empty())
    {
        initWithSSL();
    }
    else
    {
        init();
    }
}

int CloudClient::performModelsToModelsOperation(std::string serviceName,
                                                std::vector<std::string> &outgoingModels,
                                                std::vector<std::string> &incomingModels)
{
    ClientContext ctx;
    Model outModel;
    Model incModel;

    outModel.set_operation(serviceName);
    for (auto &model : outgoingModels)
    {
        outModel.add_data(model);
    }

    Status status = stub_->ModelstoModels(&ctx, outModel, &incModel);

    for (auto &model : incModel.data())
    {
        incomingModels.push_back(model);
    }

    if (status.ok())
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int CloudClient::performModelsToNumbersOperation(std::string serviceName,
                                                 std::vector<std::string> &outgoingModels,
                                                 std::vector<double> &incomingNumbers)
{
    ClientContext ctx;
    Model outModel;
    VectorofNumbers incNumbers;

    outModel.set_operation(serviceName);
    for (auto &model : outgoingModels)
    {
        outModel.add_data(model);
    }

    Status status = stub_->ModelstoNumbers(&ctx, outModel, &incNumbers);

    for (auto &number : incNumbers.point())
    {
        incomingNumbers.push_back(number);
    }

    if (status.ok())
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int CloudClient::performModelsToModelsOperation(std::string serviceName,
                                                std::vector<CloudMesh> &outgoingModels,
                                                std::vector<CloudMesh> &incomingModels)
{
    ClientContext ctx;
    OpenMeshModel outModel;
    OpenMeshModel incModel;
    ModelProcessor modelProcessor;

    outModel.set_operation(serviceName);

    for (auto &model : outgoingModels)
    {
        modelProcessor.serializeModel(outModel, model);
    }

    Status status = stub_->MeshtoMesh(&ctx, outModel, &incModel);

    if (status.ok())
    {
        for (auto model : incModel.mesh())
        {
            CloudMesh mesh;
            modelProcessor.deserializeModel(mesh, model);
            incomingModels.push_back(mesh);
        }

        return 0;
    }
    else
    {
        return 1;
    }
}

int CloudClient::performModelsToNumbersOperation(std::string serviceName,
                                                 std::vector<CloudClient::CloudMesh> &outgoingModels,
                                                 std::vector<double> &incomingNumbers)
{
    ClientContext ctx;
    OpenMeshModel outModel;
    VectorofNumbers incNumbers;
    ModelProcessor modelProcessor;

    outModel.set_operation(serviceName);
    for (auto &model : outgoingModels)
    {
        modelProcessor.serializeModel(outModel, model);
    }

    Status status = stub_->MeshtoNumbers(&ctx, outModel, &incNumbers);

    for (auto &number : incNumbers.point())
    {
        incomingNumbers.push_back(number);
    }

    if (status.ok())
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void CloudClient::saveMeshToFile(std::string file, CloudClient::CloudMesh &loadedMesh)
{
    ModelProcessor processor;
    try
    {
        processor.saveModel(loadedMesh, file);
    }
    catch (...)
    {
        throw;
    }
}

void CloudClient::loadMeshFromFile(std::string file, CloudClient::CloudMesh &loadedMesh)
{
    ModelProcessor processor;
    try
    {
        processor.loadModel(loadedMesh, file);
    }
    catch (...)
    {
        throw;
    }
}
