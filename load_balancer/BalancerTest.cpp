//
// Created by Jakub Klemens on 09/01/2019.
//

#include "BalancerTest.h"

int main(int argc, char *argv[])
{
    ::log4cplus::initialize();
    ::log4cplus::PropertyConfigurator::doConfigure("./Shared/log4cplus_configure.ini");
    
    LoadBalancer balancer;
    balancer.Run(argv[1]);
    return 0;
}
