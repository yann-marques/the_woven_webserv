#include "HTTPRequest.hpp"

HttpRequest::HttpRequest() {
    this->initReasons();
}

HttpRequest::HttpRequest(RequestDirection direction, std::string &rawRequest) {
    _direction = direction;
    this->initReasons();
    this->parseRequest(rawRequest);
    // rules = NULL;
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

std::string HttpRequest::getHeader(const std::string& key) const
{
    std::map<std::string, std::string>::const_iterator it = this->_headers.find(key);
    return (it != this->_headers.end()) ? it->second : "";
}

std::string HttpRequest::getRawHeaders(void) const {
    std::string fullHeaders;

    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); it++) {
        fullHeaders += (it->first + ": " + it->second + '\n'); 
    }

    return (fullHeaders);
}

std::map<std::string, std::string>  HttpRequest::getHeaders(void) const {
    return (this->_headers);
}

std::string HttpRequest::getBody() const
{ 
    return this->_body;
}

std::string HttpRequest::getRootPath() const {
    return (this->_rootPath);
}

Rules*  HttpRequest::getRules() const {
    return _rules;
}

std::string HttpRequest::getCgiExt(void) const {
    return _cgiExt;
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

void    HttpRequest::setRootPath(std::string &rootPath) {
    _rootPath = rootPath;
}

void    HttpRequest::setRules(Rules* rules) {
    _rules = rules;
}

void    HttpRequest::setCgiExt(std::string ext) {
    _cgiExt = ext;
}

//METHODS

void HttpRequest::initReasons(void) {
    _reasonPhrases[HTTP_OK] = "OK";
    _reasonPhrases[HTTP_CREATED] = "Created";
    _reasonPhrases[HTTP_ACCEPTED] = "Accepted";

    _reasonPhrases[HTTP_MOVED_PERMANENTLY] = "Moved Permanently";
    _reasonPhrases[HTTP_TEMPORARY_REDIRECT] = "Temporary Redirect";
    _reasonPhrases[HTTP_PERMANENT_REDIRECT] = "Permanent Redirect";

    _reasonPhrases[HTTP_BAD_REQUEST] = "Bad Request";
    _reasonPhrases[HTTP_FORBIDDEN] = "403 Forbidden";
    _reasonPhrases[HTTP_NOT_FOUND] = "Not Found";
    _reasonPhrases[HTTP_METHOD_NOT_ALLOWED] = "Method Not Allowed";
    _reasonPhrases[HTTP_NOT_ACCEPTABLE] = "Not Acceptable";
    _reasonPhrases[HTTP_PAYLOAD_TOO_LARGE] = "Payload Too Large";

    _reasonPhrases[HTTP_INTERNAL_SERVER_ERROR] = "Internal Server Error";
    _reasonPhrases[HTTP_BAD_GATEWAY] = "Bad Gateway";
    _reasonPhrases[HTTP_SERVICE_UNAVAILABLE] = "Service Unavailable";
}

void    HttpRequest::log(void) {
    std::string userAgent;

    userAgent = getHeader("User-Agent");  
    std::cout << '"' << this->_method << ' ' << this->_path << ' ' << this->_version << '"' << ' ';
    if (!userAgent.empty())
        std::cout << userAgent << std::endl;
}

void    HttpRequest::parseRequest(const std::string &rawRequest)
{
    std::istringstream stream(rawRequest);
    std::string line;

    std::getline(stream, line);
    if (_direction == HTTP_REQUEST) {
        std::cout << "[" << rawRequest << "]" << std::endl;
        if (!line.empty() && (line.find("HTTP") != std::string::npos)) {
            std::istringstream requestLine(line);
            requestLine >> _method >> _path >> _version;
        }
    
        if (_method.empty() && _path.empty() && _version.empty()) {
            std::cout << "Malformed header" << std::endl;
            throw MalformedHttpHeader();
        }
    }


    if (!line.empty() && (line.find('\r') != std::string::npos)) {
        if (!line.empty() && (line.find("HTTP") != std::string::npos))
            std::getline(stream, line);
        while (line != "\r") {
            size_t pos = line.find(":");
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                size_t jumpSize = (line[pos + 1] == ' ' ? 2 : 1);
                std::string value = line.substr(pos + jumpSize); //skip ":" or ": "
                std::cout << "jumpSize: " << jumpSize << " key:" << key << " value:" << value << std::endl;
                if (!value.empty() && value[value.size() - 1] == '\r') {
                    value.erase(value.size() - 1);
                }
                _headers[key] = value;
            }
            if (stream.eof())
                break;
            std::getline(stream, line);
        }
    }

    while (std::getline(stream, line)) {
        _body += line + "\n";
    }

    if (_direction == HTTP_RESPONSE)
        setDefaultsHeaders();
}

void    HttpRequest::makeError(int httpCode) {
    std::vector<char> buffer(4096);

    std::stringstream stream;
    stream << "default/errors/" << httpCode << ".html";

    std::string errorPagePath = stream.str();
	int fd = open(errorPagePath.c_str(), O_RDONLY);
	if (fd < 0)
		throw OpenFileException();

	ssize_t bytesRead = read(fd, buffer.data(), buffer.size());
	std::string rawResponse(buffer.begin(), buffer.begin() + bytesRead);
	close(fd);
    setDefaultsHeaders();
    setResponseCode(httpCode);
    setBody(rawResponse);
}

void    HttpRequest::generateIndexFile(const std::vector<std::string>& fileNames) {
    std::string html = "<html><nav>\n  <ul>\n";
    
    for (std::vector<std::string>::const_iterator it = fileNames.begin(); it != fileNames.end(); it++) {
        html += "    <li><a href=\"" + (*it) + "\">" + (*it) + "</a></li>\n";
    }
    html += "  </ul>\n</nav></html>\n";
    setBody(html);
    setDefaultsHeaders();
}

std::string HttpRequest::makeRawResponse(void) {
    std::ostringstream rawResponse;

    rawResponse << _version << " " << _responseCode << " " << _reasonPhrases[_responseCode] << "\r\n";

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
}