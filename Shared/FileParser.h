//
// Created by Jakub Klemens on 28/03/2019.
//

#ifndef INC_3DCLOUD_FILEPARSER_H
#define INC_3DCLOUD_FILEPARSER_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

class FileParser
{
public:
    FileParser() = default;
    ~FileParser() = default;

    void parseClientConfig(const std::string &file,
                          std::string &balancerAddress,
                          float &minVersion,
                          std::string &certFilename,
                          std::string &keyFilename,
                          std::string &rootFilename);

    void parseBalancerConfig(const std::string &file,
                            std::string &balancerAddress,
                            int &heartBeat,
                            std::string &certFilename,
                            std::string &keyFilename,
                            std::string &rootFilename);

    void parseProviderConfig(const std::string &file,
                            std::string &providerAddress,
                            std::string &balancerAddress,
                            int &heartBeat,
                            float &version,
                            std::string &certFilename,
                            std::string &keyFilename,
                            std::string &rootFilename);
    void read(const std::string &filename, std::string &data);

private:
};


#endif //INC_3DCLOUD_FILEPARSER_H
