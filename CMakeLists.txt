cmake_minimum_required(VERSION 3.2)

project(3DCloud)

#add_compile_options(-std=c++11)
set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# GRPC and Protocol Buffers libraries location
list(APPEND CMAKE_PREFIX_PATH "/opt/grpc" "/opt/protobuf")

# Cmake find modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(Protobuf REQUIRED)
find_package(GRPC REQUIRED)

set(PROTOS
        ${CMAKE_CURRENT_SOURCE_DIR}/protos/cloud_services.proto
        )


set(PROTO_SRC_DIR ${CMAKE_CURRENT_BINARY_DIR}/proto-src)
file(MAKE_DIRECTORY ${PROTO_SRC_DIR})

include_directories(${PROTO_SRC_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR})


protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SRC_DIR} ${PROTOS})
grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${PROTO_SRC_DIR} ${PROTOS})

# Building provider
set(providerSRC
        provider/ServiceProvider.cpp provider/ServiceProvider.h
        provider/ModelManager.cpp provider/ModelManager.h
        provider/BalancerEstablisher.cpp provider/BalancerEstablisher.h
        provider/ServiceProviderImpl.cpp provider/ServiceProviderImpl.h
        provider/Service.cpp provider/Service.h Shared/FileParser.cpp Shared/FileParser.h)

add_library(ServiceProviderlib SHARED ${providerSRC} ${PROTO_SRCS} ${GRPC_SRCS})

target_link_libraries(ServiceProviderlib
        gRPC::grpc++_reflection
        protobuf::libprotobuf
        OpenMeshCore
        OpenMeshTools
        log4cplus)

add_executable(ServiceProviderTEST provider/ProviderTest.cpp provider/ProviderTest.h)

target_link_libraries(ServiceProviderTEST ServiceProviderlib)

# Building client
set(CloudClientSRC
        client/CloudClient.cpp client/CloudClient.h
        client/ModelProcessor.cpp client/ModelProcessor.h
        client/ProviderFinder.cpp client/ProviderFinder.h Shared/FileParser.cpp Shared/FileParser.h)

add_library(CloudClientlib SHARED ${CloudClientSRC} ${PROTO_SRCS} ${GRPC_SRCS})

target_link_libraries(CloudClientlib
        gRPC::grpc++_reflection
        protobuf::libprotobuf
        OpenMeshCore
        OpenMeshTools
        log4cplus
        )

add_executable(CloudClientTEST client/ClientTest.cpp client/ClientTest.h)

target_link_libraries(CloudClientTEST CloudClientlib)

# Building LoadBalancer
set(LoadBalancerSRC
        load_balancer/LoadBalancer.cpp load_balancer/LoadBalancer.h
        load_balancer/LoadBalanceImpl.cpp load_balancer/LoadBalanceImpl.h Shared/FileParser.cpp Shared/FileParser.h)

add_library(LoadBalancerlib SHARED ${LoadBalancerSRC} ${PROTO_SRCS} ${GRPC_SRCS})

target_link_libraries(LoadBalancerlib
        gRPC::grpc++_reflection
        protobuf::libprotobuf
        log4cplus
        )

add_executable(LoadBalancerTEST
        load_balancer/BalancerTest.cpp load_balancer/BalancerTest.h)

target_link_libraries(LoadBalancerTEST LoadBalancerlib)