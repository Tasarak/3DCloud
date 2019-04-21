//
// Created by Jakub Klemens on 09/01/2019.
//

#include "BalancerSample.h"

int main(int argc, char *argv[])
{
    std::string configFile = std::string(argv[1]);
    
    LoadBalancer balancer(configFile);
    balancer.Run();
    return 0;
}
