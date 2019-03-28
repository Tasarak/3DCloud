//
// Created by Jakub Klemens on 28/03/2019.
//

#include "FileParser.h"

void FileParser::parseClientConfig(const std::string &file, std::string &balancerAddress, float &minVersion,
                                  std::string &certFilename, std::string &keyFilename, std::string &rootFilename)
{
    std::ifstream inputFile(file);

    if (!inputFile.is_open())
    {
        throw std::ios_base::failure("File can't be opened.");
    }

    std::string line;

    while (std::getline(inputFile, line))
    {
        if (line.find("balancerAddress=") == 0)
        {
            balancerAddress = line.substr(16);
        }
        else if (line.find("version=") == 0)
        {
            std::string stringVersion = line.substr(8);
            try
            {
                minVersion = std::stof(stringVersion);
            }
            catch (...)
            {
                throw;
            }
        }
        else if (line.find("cert=") == 0)
        {
            certFilename = line.substr(5);
        }
        else if (line.find("key=") == 0)
        {
            keyFilename = line.substr(4);
        }
        else if (line.find("root=") == 0)
        {
            rootFilename = line.substr(5);
        }
    }

    inputFile.close();
}

void FileParser::parseProviderConfig(const std::string &file,
                                     std::string &providerAddress,
                                     std::string &balancerAddress,
                                     int &heartBeat,
                                     float &version,
                                     std::string &certFilename,
                                     std::string &keyFilename,
                                     std::string &rootFilename)
{
    std::ifstream inputFile(file);

    if (!inputFile.is_open())
    {
        throw std::ios_base::failure("File can't be opened.");
    }

    std::string line;

    while (std::getline(inputFile, line))
    {
        if (line.find("providerAddress=") == 0)
        {
            providerAddress = line.substr(16);
        }
        if (line.find("balancerAddress=") == 0)
        {
            balancerAddress = line.substr(16);
        }
        else if (line.find("heartbeat=") == 0)
        {
            std::string stringHeartbeat = line.substr(10);
            try
            {
                heartBeat = std::stoi(stringHeartbeat);
            }
            catch (...)
            {
                throw;
            }
        }
        else if (line.find("version=") == 0)
        {
            std::string stringVersion = line.substr(8);
            try
            {
                version = std::stof(stringVersion);
            }
            catch (...)
            {
                throw;
            }
        }
        else if (line.find("cert=") == 0)
        {
            certFilename = line.substr(5);
        }
        else if (line.find("key=") == 0)
        {
            keyFilename = line.substr(4);
        }
        else if (line.find("root=") == 0)
        {
            rootFilename = line.substr(5);
        }
    }

    inputFile.close();
}

void FileParser::parseBalancerConfig(const std::string &file,
                                     std::string &balancerAddress,
                                     int &heartBeat,
                                     std::string &certFilename,
                                     std::string &keyFilename,
                                     std::string &rootFilename)
{
    std::ifstream inputFile(file);

    if (!inputFile.is_open())
    {
        throw std::ios_base::failure("File can't be opened.");
    }

    std::string line;

    while (std::getline(inputFile, line))
    {
        if (line.find("balancerAddress=") == 0)
        {
            balancerAddress = line.substr(16);
        }
        else if (line.find("heartbeat=") == 0)
        {
            std::string stringHeartbeat = line.substr(10);
            try
            {
                heartBeat = std::stoi(stringHeartbeat);
            }
            catch (...)
            {
                throw;
            }
        }
        else if (line.find("cert=") == 0)
        {
            certFilename = line.substr(5);
        }
        else if (line.find("key=") == 0)
        {
            keyFilename = line.substr(4);
        }
        else if (line.find("root=") == 0)
        {
            rootFilename = line.substr(5);
        }
    }

    inputFile.close();
}

void FileParser::read(const std::string &filename, std::string &data)
{
    std::ifstream file(filename.c_str(), std::ios::in);

    if (file.is_open())
    {
        std::stringstream ss;
        ss << file.rdbuf();

        file.close();

        data = ss.str();
    }
}