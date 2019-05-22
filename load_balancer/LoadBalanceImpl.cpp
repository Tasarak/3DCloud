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

/***
 * Constructor create owns thread for looking after Providers
 * @param heartBeatRate
 */
LoadBalanceImpl::LoadBalanceImpl(int &heartBeatRate) : heartBeatRate_(heartBeatRate)
{
    updater_ = std::thread(&LoadBalanceImpl::updateServerList, this);
}

LoadBalanceImpl::~LoadBalanceImpl()
{
    updater_.join();
}
/***
 * Return address of Provider to Client
 * @param context
 * @param request
 * @param response
 * @return Status
 */
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

    response->set_serveraddresss(findBestServer(sNode));

    return Status::OK;
}

/***
 * Update info about Providers
 * @param context
 * @param request
 * @param reply
 * @return Status
 */
Status LoadBalanceImpl::SendHeartbeat(ServerContext *context,
                                      const HeartBeat *request,
                                      HeartBeatReply *reply)
{
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("LoadBalancer"));

    for (auto &server : servers_)
    {
        if (server.address == request->serveraddresss())
        {
            server.usage = request->usage();
            server.lastBeat = std::chrono::system_clock::now();
            reply->set_status(true);
            LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT("Beat received from provider:" << request->serveraddresss()));
            return Status::OK;
        }
    }

    reply->set_status(false);
    return Status::CANCELLED;
}

/***
 * Establish new Service Provider
 * @param context
 * @param request
 * @param response
 * @return Status
 */
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
    servers_.push_back(sNode);
    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Server Established on address: ") << sNode.address);

    response->set_status(true);

    return Status::OK;
}

/***
 * Handle Provider list and remove inactive providers with period
 */
void LoadBalanceImpl::updateServerList()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(2*heartBeatRate_));
        removeInactiveServer();
    }
}

void LoadBalanceImpl::removeInactiveServer()
{
    std::lock_guard<std::mutex> guard(mutex_);
    servers_.erase(std::remove_if(servers_.begin(), servers_.end(), [this](const ServerNode& server){
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - server.lastBeat);
        return diff.count() > heartBeatRate_;
    }), servers_.end());
}

/***
 * Find the best possiblle Provider and return his address
 * @param sNode
 * @return The best possiblle Provider address
 */
std::string LoadBalanceImpl::findBestServer(ServerNode sNode)
{
    std::vector<ServerNode> possibleServers;
    for (auto server : servers_)
    {
        if (server.version >= sNode.version)
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
    return minUsageServer(possibleServers);
}

/***
 * Find server with the lowest usage and return his address.
 * If more Providers have same usage, pick it randomlly.
 * @param possibleServers
 * @return Server with minimal usage
 */
std::string LoadBalanceImpl::minUsageServer(std::vector<ServerNode> possibleServers)
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

