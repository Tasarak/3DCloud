//
// Created by Jakub Klemens on 27/08/2018.
//

#include "LoadBalanceImpl.h"

using grpc::Server;
using grpc::ServerWriter;
using grpc::ServerReader;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerCompletionQueue;
using grpc::Status;

using Cloud3D::LoadBalance;
using Cloud3D::ServerRequest;
using Cloud3D::ServerReply;
using Cloud3D::HeartBeatReply;
using Cloud3D::HeartBeat;
using Cloud3D::NewServer;
using Cloud3D::Confirmation;

LoadBalanceImpl::LoadBalanceImpl(int &heartBeatRate) : heartBeatRate_(heartBeatRate)
{
    updater_ = std::thread(&LoadBalanceImpl::UpdateServerList, this);
}

LoadBalanceImpl::~LoadBalanceImpl()
{
    updater_.join();
}

Status LoadBalanceImpl::ListServer(::grpc::ServerContext* context,
                                   const ::Cloud3D::ServerRequest* request,
                                   ::Cloud3D::ServerReply* response)
{
    ServerNode sNode;
    sNode.version = request->version();

    for (auto operation : request->operation())
    {
        sNode.operations.push_back(operation);
    }

    response->set_serveraddresss(FindBestServer(sNode));

    return Status::OK;
}

Status LoadBalanceImpl::SendHeartbeat(ServerContext *context,
                                      const HeartBeat *request,
                                      HeartBeatReply *reply)
{
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("LoadBalancer"));

    for (auto &server : servers)
    {
        if (server.address == request->serveraddresss())
        {
            server.usage = request->usage();
            server.lastBeat = std::chrono::system_clock::now();
            reply->set_status(true);
            std::cout << "Beat received from provider: " <<request->serveraddresss() << std::endl;
            LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Beat received from provider:" << request->serveraddresss()));
            return Status::OK;
        }
    }

    reply->set_status(false);
    return Status::CANCELLED;
}

Status LoadBalanceImpl::EstablishServer(::grpc::ServerContext* context,
                                        const ::Cloud3D::NewServer* request,
                                        ::Cloud3D::Confirmation* response)
{
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("LoadBalancer"));

    ServerNode sNode;

    sNode.address = request->serveraddress();
    sNode.version = request->version();

    for (auto operation : request->operation())
    {
        sNode.operations.push_back(operation);
    }

    sNode.lastBeat = std::chrono::system_clock::now();

    std::lock_guard<std::mutex> guard(mutex_);
    servers.push_back(sNode);
    std::cout << "Server established" << std::endl;
    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Server Established on address: ") << sNode.address);

    response->set_status(true);

    return Status::OK;
}

void LoadBalanceImpl::UpdateServerList()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        RemoveInactiveServer();
    }
}

void LoadBalanceImpl::RemoveInactiveServer()
{
    std::lock_guard<std::mutex> guard(mutex_);
    servers.erase(std::remove_if(servers.begin(), servers.end(), [this](const ServerNode& server){
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - server.lastBeat);
        return diff.count() > heartBeatRate_;
    }), servers.end());

}

std::string LoadBalanceImpl::FindBestServer(ServerNode sNode)
{
    std::vector<ServerNode> possibleServers;
    for (auto server : servers)
    {
        if (sNode.version >= server.version)
        {
            int opCounter = 0;

            for (auto sOperation : server.operations)
            {
                for (auto rOperation : sNode.operations)
                {
                    if (sOperation == rOperation)
                    {
                        opCounter++;
                        break;
                    }
                }
            }
            if (opCounter == sNode.operations.size())
            {
                possibleServers.push_back(server);
            }
        }
    }
    return MinUsageServer(possibleServers);
}

std::string LoadBalanceImpl::MinUsageServer(std::vector<ServerNode> possibleServers)
{
    if (possibleServers.empty())
    {
        return "";
    }

    int minUsage = possibleServers.front().usage;
    std::string minUsageAddress = possibleServers.front().address;
    std::vector<ServerNode> sameUsageProviders;

    for (auto server : possibleServers)
    {
        if (server.usage < minUsage)
        {
            minUsage = server.usage;
            minUsageAddress = server.address;

            sameUsageProviders.clear();
            sameUsageProviders.push_back(server);
        }
        else if (server.usage == minUsage)
        {
            sameUsageProviders.push_back(server);
        }
    }

    if (sameUsageProviders.size() > 1)
    {
        std::random_device random_device;
        std::mt19937 engine{random_device()};
        std::uniform_int_distribution<int> dist(0, (int)sameUsageProviders.size() - 1);
        ServerNode random_element = sameUsageProviders[dist(engine)];

        return random_element.address;
    }
    else
    {
        return minUsageAddress;
    }
}

