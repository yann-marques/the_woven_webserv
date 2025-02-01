#pragma once

#include "Server.hpp"

class httpRequest {
    private:
        std::string _method;
        std::string _version;
        std::string _path;
        std::map<std::string, std::string> _headers;
        std::string _body;

        //METHODS
        void    parse(const std::string &rawRequest);

    public:
        httpRequest(const std::string &rawRequest);
        ~httpRequest(void);

        //GETTERS
        std::string getMethod(void) const;
        std::string getVersion(void) const;
        std::string getPath(void) const;
        std::string getHeader(const std::string &key) const;
        std::string getBody(void) const;
}