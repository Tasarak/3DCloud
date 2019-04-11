//
// Created by Jakub Klemens on 09/01/2019.
//

#include "BalancerSample.h"

int main(int argc, char *argv[])
{
    ::log4cplus::initialize();
    ::log4cplus::PropertyConfigurator::doConfigure("./Shared/log4cplus_configure.ini");
    std::string configFile = "./load_balancer/config.txt";
    
    LoadBalancer balancer(configFile);
    balancer.Run();
    return 0;
}
