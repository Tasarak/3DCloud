//
// Created by Jakub Klemens on 27/08/2018.
//

#include "BalancerEstablisher.h"

using grpc::ClientWriter;
using grpc::ClientContext;
using grpc::Status;
using grpc::Channel;

using Cloud3D::LoadBalance;
using Cloud3D::HeartBeat;
using Cloud3D::HeartBeatReply;
using Cloud3D::NewServer;
using Cloud3D::Confirmation;

BalancerEstablisher::BalancerEstablisher(std::string srvAddress, std::shared_ptr<Channel> channel) :
        stub_(LoadBalance::NewStub(channel)), srvAddress_(srvAddress)
{}

int BalancerEstablisher::EstablishServer()
{
    NewServer server;
    Confirmation confirmation;
    ClientContext ctx;

    server.set_serveraddress(srvAddress_);
    server.set_version(1.2);

    ServiceProviderImpl *provider = &ServiceProviderImpl::GetInstance();

    for (const auto &operation : provider->ListOfServiceNames)
    {
        server.add_operation(operation);
    }

    Status status = stub_->EstablishServer(&ctx,server, &confirmation);
    if (status.ok())
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void BalancerEstablisher::SendHeartBeat()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        SendBeat();
    }
}

void BalancerEstablisher::SendBeat()
{
    HeartBeat beat;
    HeartBeatReply reply;
    ClientContext context;
    beat.set_status(true);
    beat.set_usage(35);
    beat.set_serveraddresss(srvAddress_);

    Status status = stub_->SendHeartbeat(&context, beat, &reply);

    if (!status.ok())
    {
        std::cout << "SendHeartbeat rpc failed." << std::endl;
        while (EstablishServer())
        {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            std::cout << "Trying to Establish provider" << std::endl;
        }
    }
}