#include "HTTPRequest.hpp"

httpRequest::httpRequest(const std::string &rawRequest) {
    this->parse(rawRequest);
} 

std::string httpRequest::getMethod() const
{ 
    return this->_method;
}

std::string httpRequest::getPath() const
{ 
    return this->_path;
}

std::string httpRequest::getVersion() const
{
    return this->_version;
}

std::string httpRequest::getHeader(const std::string& key) const
{
    std::map<std::string, std::string>::const_iterator it = this->_headers.find(key);
    return (it != this->_headers.end()) ? it->second : "";
}

std::string httpRequest::getBody() const
{ 
    return this->_body;
}

void    httpRequest::parse(const std::string &rawRequest)
{
    
}