#include "HTTPRequest.hpp"

HttpRequest::HttpRequest() {}

HttpRequest::HttpRequest(const std::string &rawRequest) {
    this->parse(rawRequest);
} 

HttpRequest::~HttpRequest(void) {};

//GETTERS

std::string HttpRequest::getMethod() const
{ 
    return this->_method;
}

std::string HttpRequest::getPath() const
{ 
    return this->_path;
}

std::string HttpRequest::getVersion() const
{
    return this->_version;
}

//SETTERS

void    HttpRequest::setMethod(std::string &method) {
    this->_method = method;
}

void    HttpRequest::setResponseCode(int code) {
    this->_responseCode = code;
}

void    HttpRequest::setVersion(const std::string &version) {
    this->_version = version;
}

void    HttpRequest::setHeaders(std::map<std::string, std::string> &headers) {
    this->_headers = headers;
}

void    HttpRequest::setBody(const std::string &body) {
    this->_body = body;
}


std::string HttpRequest::getHeader(const std::string& key) const
{
    std::map<std::string, std::string>::const_iterator it = this->_headers.find(key);
    return (it != this->_headers.end()) ? it->second : "";
}

std::string HttpRequest::getBody() const
{ 
    return this->_body;
}

void    HttpRequest::parse(const std::string &rawRequest)
{
    std::istringstream stream(rawRequest);
    std::string line;

    // Parse the request line
    if (std::getline(stream, line)) {
        std::istringstream requestLine(line);
        requestLine >> _method >> _path >> _version;
    }

    //Parse HEADERS part
    while (std::getline(stream, line) && line != "\r") {
        size_t pos = line.find(": ");
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 2); // Skip ": "
            if (!value.empty() && value[value.size() - 1] == '\r') {
                value.erase(value.size() - 1);
            }
            _headers[key] = value;
        }
    }

     // Parse body (if any)
    while (std::getline(stream, line)) {
        _body += line + "\n";
    }
}

std::string HttpRequest::makeRawResponse(void) {
    std::ostringstream rawResponse;

    std::map<int, std::string> reasonPhrase;

    reasonPhrase[200] = "OK";
    reasonPhrase[404] = "Not Found";
    reasonPhrase[502] = "Bad Gateway";

    rawResponse << _version << " " << _responseCode << " " << reasonPhrase[_responseCode] << "\r\n";

    std::map<std::string, std::string>::const_iterator it;
    for (it = _headers.begin(); it != _headers.end(); ++it) {
        rawResponse << it->first << ": " << it->second << "\r\n";
    }

    rawResponse << "Content-Length: " << _body.size() << "\r\n";
    rawResponse << "\r\n"; // End of headers
    rawResponse << _body;

    return rawResponse.str();
}

void HttpRequest::setDefaultsHeaders(void) {
    this->setVersion("HTTP/1.1");
    this->setResponseCode(200);
    this->_headers.insert(std::pair<std::string, std::string>("Server", "TheWovenWebserver/0.0.1"));
    this->_headers.insert(std::pair<std::string, std::string>("Connection", "keep-alive"));
    this->_headers.insert(std::pair<std::string, std::string>("Content-Type", "text/html; charset=UTF-8"));
}