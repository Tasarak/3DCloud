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

int CloudClient::init()
{
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("CloudClient"));
    ProviderFinder finder(balancerAddress_, minVersion_);

    grpc::ChannelArguments ch_args;
    ch_args.SetMaxReceiveMessageSize(-1);
    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Getting address from LoadBalancer"));

    std::string providerAddress = finder.GetServer(services_);
    if (providerAddress.empty())
    {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("Requested provider doesn't exist."));
        return 1;
    }

    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Address of Service Provider") << providerAddress);
    auto channel = grpc::CreateCustomChannel(providerAddress, grpc::InsecureChannelCredentials(), ch_args);
    stub_ = Cloud3D::ServiceProvide::NewStub(channel);
    return 0;
}

int CloudClient::initWithSSL()
{
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("CloudClient"));
    ProviderFinder finder(balancerAddress_, minVersion_);

    std::string cert;
    std::string key;
    std::string root;

    FileParser parser;

    parser.read(certFilename_, cert);
    parser.read(keyFilename_, key);
    parser.read(rootFilename_, root);

    grpc::SslCredentialsOptions opts = {root, key, cert};

    grpc::ChannelArguments chArgs;
    chArgs.SetMaxReceiveMessageSize(-1);
    auto channelCreds = grpc::SslCredentials(grpc::SslCredentialsOptions(opts));

    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Getting address from LoadBalancer"));
    std::string providerAddress = finder.GetServer(services_);
    if (providerAddress.empty())
    {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("Requested provider doesn't exist."));
        return 1;
    }

    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Address of Service Provider") << providerAddress);

    auto channel = grpc::CreateCustomChannel(providerAddress, channelCreds, chArgs);

    stub_ = Cloud3D::ServiceProvide::NewStub(channel);
    return 0;
}

CloudClient::CloudClient(std::string &balancerAddress, float &minVersion, std::vector<std::string> &services)
                        : balancerAddress_(balancerAddress),
                          minVersion_(minVersion),
                          services_(services)
{
    ::log4cplus::initialize();
    ::log4cplus::PropertyConfigurator::doConfigure("./Shared/log4cplus_configure.ini");
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("CloudClient"));
    if (init())
    {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("Requested provider doesn't exist."));
        throw std::invalid_argument("Requested provider doesn't exist.");
    }
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
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("CloudClient"));
    if (initWithSSL())
    {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("Requested provider doesn't exist."));
        throw std::invalid_argument("Requested provider doesn't exist.");
    }
}

CloudClient::CloudClient(std::string &configFile, std::vector<std::string> &services)
                        : services_(services)
{
    ::log4cplus::initialize();
    ::log4cplus::PropertyConfigurator::doConfigure("./Shared/log4cplus_configure.ini");
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("CloudClient"));
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
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("Address of LoadBalancer not defined in config."));
        throw std::invalid_argument("Address of LoadBalancer not defined in config.");
    }

    if (!certFilename_.empty() && !keyFilename_.empty() && !rootFilename_.empty())
    {
        if (initWithSSL())
        {
            LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("Requested provider doesn't exist."));
            throw std::invalid_argument("Requested provider doesn't exist.");
        }
    }
    else
    {
        if (init())
        {
            LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("Requested provider doesn't exist."));
            throw std::invalid_argument("Requested provider doesn't exist.");
        }
    }
}

int CloudClient::performModelsToModelsOperation(std::string serviceName,
                                                std::vector<std::string> &outgoingModels,
                                                std::vector<std::string> &incomingModels)
{
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("CloudClient"));
    ClientContext ctx;
    Model outModel;
    Model incModel;

    outModel.set_operation(serviceName);
    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Start of serialization of models"));
    for (auto &model : outgoingModels)
    {
        outModel.add_data(model);
    }
    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("End of serialization of models"));

    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Start of communication with Provider"));
    Status status = stub_->ModelstoModels(&ctx, outModel, &incModel);

    if (status.ok())
    {
        LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("End of communication with Provider"));
        LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Start of deserialization of models"));
        for (auto &model : incModel.data())
        {
            incomingModels.push_back(model);
        }
        LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("End of deserialization of models"));

        return 0;
    }
    else
    {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("Communication was not successful. Error: ") << status.error_message());
        return 1;
    }
}

int CloudClient::performModelsToNumbersOperation(std::string serviceName,
                                                 std::vector<std::string> &outgoingModels,
                                                 std::vector<double> &incomingNumbers)
{
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("CloudClient"));
    ClientContext ctx;
    Model outModel;
    VectorofNumbers incNumbers;

    outModel.set_operation(serviceName);
    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Start of serialization of models"));
    for (auto &model : outgoingModels)
    {
        outModel.add_data(model);
    }
    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("End of serialization of models"));

    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Start of communication with Provider"));
    Status status = stub_->ModelstoNumbers(&ctx, outModel, &incNumbers);

    if (status.ok())
    {
        LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("End of communication with Provider"));
        LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Start of deserialization of models"));
        for (auto &number : incNumbers.point())
        {
            incomingNumbers.push_back(number);
        }
        LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("End of deserialization of models"));

        return 0;
    }
    else
    {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("Communication was not successful. Error: ") << status.error_message());
        return 1;
    }
}

int CloudClient::performModelsToModelsOperation(std::string serviceName,
                                                std::vector<CloudMesh> &outgoingModels,
                                                std::vector<CloudMesh> &incomingModels)
{
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("CloudClient"));
    ClientContext ctx;
    OpenMeshModel outModel;
    OpenMeshModel incModel;
    ModelProcessor modelProcessor;

    outModel.set_operation(serviceName);

    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Start of serialization of models"));
    for (auto &model : outgoingModels)
    {
        modelProcessor.serializeModel(outModel, model);
    }
    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("End of serialization of models"));

    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Start of communication with Provider"));
    Status status = stub_->MeshtoMesh(&ctx, outModel, &incModel);

    if (status.ok())
    {
        LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("End of communication with Provider"));
        LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Start of deserialization of models"));
        for (auto model : incModel.mesh())
        {
            CloudMesh mesh;
            modelProcessor.deserializeModel(mesh, model);
            incomingModels.push_back(mesh);
        }
        LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("End of deserialization of models"));

        return 0;
    }
    else
    {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("Communication was not successful. Error: ") << status.error_message());
        return 1;
    }
}

int CloudClient::performModelsToNumbersOperation(std::string serviceName,
                                                 std::vector<CloudClient::CloudMesh> &outgoingModels,
                                                 std::vector<double> &incomingNumbers)
{
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("CloudClient"));
    ClientContext ctx;
    OpenMeshModel outModel;
    VectorofNumbers incNumbers;
    ModelProcessor modelProcessor;

    outModel.set_operation(serviceName);
    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Start of serialization of models"));
    for (auto &model : outgoingModels)
    {
        modelProcessor.serializeModel(outModel, model);
    }
    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("End of serialization of models"));

    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Start of communication with Provider"));
    Status status = stub_->MeshtoNumbers(&ctx, outModel, &incNumbers);

    if (status.ok())
    {
        LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("End of communication with Provider"));

        for (auto &number : incNumbers.point())
        {
            incomingNumbers.push_back(number);
        }
        return 0;
    }
    else
    {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("Communication was not successful. Error: ") << status.error_message());
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
