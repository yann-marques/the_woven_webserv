#include "HTTPRequest.hpp"

HttpRequest::HttpRequest() {
    this->initReasons();
}

HttpRequest::HttpRequest(RequestDirection direction, t_binary &rawRequest) {
    _direction = direction;
    this->initReasons();
    this->parseRequest(rawRequest);
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

t_binary HttpRequest::getBody() const
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

int HttpRequest::getClientFD(void) const {
    return _clientFd;
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

void    HttpRequest::setBody(const t_binary &body) {
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

void    HttpRequest::setClientFD(int fd) {
    _clientFd = fd;
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
    _reasonPhrases[HTTP_FORBIDDEN] = "Forbidden";
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


bool    isBodyChunkSizeLine(std::string str) { //detect if the str is the length of the chunk in hex
    std::size_t acceptedHexChar = 0;
    std::size_t strSize = str.size();

    if (str[strSize - 1] != '\r') return false;
    if (strSize == 0 || strSize > 7) //6-digit hexadecimal number + 1 for '\r' (FFFFFF) is enough
        return (false);
    for (std::size_t i = 0; i < strSize; ++i) {
        char c = str[i];
        if ((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) //its a accepted hex letter
            acceptedHexChar++;
        else if (c >= '0' && c <= '9')
            acceptedHexChar++;
        else if (c == '\r') //only check for \r because std::getline does not return \n char.
            acceptedHexChar++;
        else
            break ; 
    }
    return (acceptedHexChar == strSize);
}

const unsigned char* find_sequence(const t_binary& buffer, const char* sequence) {
    size_t buffer_size = buffer.size();
    size_t sequence_len = std::strlen(sequence);

    if (sequence_len == 0 || sequence_len > buffer_size) {
        return NULL;
    }

    for (size_t i = 0; i <= buffer_size - sequence_len; ++i) {
        if (std::memcmp(&buffer[i], sequence, sequence_len) == 0) {
            return &buffer[i];  // Return pointer to match in vector
        }
    }

    return NULL;
}

bool is_hex_line(const t_binary& buffer, size_t start, size_t end) {
    if (start >= end) return false; // Empty line check

    for (size_t i = start; i < end; ++i) {
        if (!std::isxdigit(buffer[i])) {
            return false;
        }
    }
    return true;
}

size_t find_next_line(const t_binary& buffer, size_t start) {
    while (start < buffer.size() - 1) {
        if (buffer[start] == '\r' && buffer[start + 1] == '\n') {
            return start + 2; // Return position after \r\n
        }
        ++start;
    }
    return buffer.size(); // End of buffer
}


void HttpRequest::parseRequest(const t_binary &rawRequest) {
    std::size_t pos = 0;

    //LINE INFOS PARSING

    const unsigned char* lineInfosPos = find_sequence(rawRequest, "HTTP");
    if (lineInfosPos) { //HTTP INFOS

        for (;pos < rawRequest.size(); ++pos) {
            if (rawRequest[pos] == ' ') {
                pos++;
                break;
            }
            _method += rawRequest[pos];
        }

        for (; pos < rawRequest.size(); ++pos) {
            if (rawRequest[pos] == ' ') {
                pos++;
                break;
            }
            _path += rawRequest[pos];
        }

        for (; pos < rawRequest.size(); ++pos) {
            if (rawRequest[pos] == '\n') {
                pos++;
                break;
            }
            if (rawRequest[pos] != '\r')
                _version += rawRequest[pos];
        }
    }
    //END OF PARSING LINE INFOS


    //PARSING HEADERS
    std::string endHeaderKey = "\r\n\r\n";
    
    const unsigned char* endOfHeadersPos = find_sequence(rawRequest, endHeaderKey.c_str());
    size_t endHeadersIndex = endOfHeadersPos - &rawRequest[0]; // Convert pointer to index
    if (endOfHeadersPos) { // If headers are present

        while (pos < endHeadersIndex) {
            std::string key;
            std::string value;

            // Read key
            while (pos < endHeadersIndex && rawRequest[pos] != ':') {
                key += rawRequest[pos++];
            }

            // Skip ':' and spaces
            if (pos < endHeadersIndex && rawRequest[pos] == ':') pos++;
            while (pos < endHeadersIndex && rawRequest[pos] == ' ') pos++;
            
            while (pos < endHeadersIndex && rawRequest[pos] != '\r') {
                value += rawRequest[pos++];
            }

            // Skip '\r\n'
            if (pos < endHeadersIndex && rawRequest[pos] == '\r') pos++;
            if (pos < endHeadersIndex && rawRequest[pos] == '\n') pos++;

            _headers[key] = value;
        }
    }
    //END OF PARSING HEADERS


    //PARSING BODY
    
    if (lineInfosPos || endOfHeadersPos) { //ITS HTTP REQUEST IN BINARY
        std::string tranfertType = getHeader("Transfer-Encoding");
        bool isChuncked = tranfertType == "chunked";
        while (pos < rawRequest.size()) {
            // Find the end of the current line
            size_t lineEnd = find_next_line(rawRequest, pos);
            
            // Check if it's a chunk size line (hexadecimal)
            if (isChuncked && is_hex_line(rawRequest, pos, lineEnd - 2)) {
                pos = lineEnd; // Skip the chunk size line
                continue;
            }
    
            // Copy body data until \r\n
            while (pos < rawRequest.size() && !(rawRequest[pos] == '\r' && rawRequest[pos + 1] == '\n')) {
                _body.push_back(rawRequest[pos]);
                pos++;
            }
    
            pos += 2; // Skip \r\n after the chunk
        }
    } else { //NOT A REQUEST, JUST A FILE OR OTHER
        _body = rawRequest;
    }

    if (_direction == HTTP_RESPONSE) {
        setDefaultsHeaders();
    }
}

void    HttpRequest::makeError(int httpCode, HttpRequest request){
    t_binary buffer(4096);

    std::stringstream stream;
    stream << "default/errors/" << httpCode << ".html";

    std::string errorPagePath;
    std::map<int, std::string> errorPagesConfig = request.getRules()->getErrorPages();
    if (errorPagesConfig.count(httpCode) > 0) {
        errorPagePath = errorPagesConfig[httpCode];
    } else {
        errorPagePath = stream.str();
    }

	int fd = open(errorPagePath.c_str(), O_RDONLY);
	if (fd < 0) {
        std::string internalErrorString = "HTTP 500 Error: Internal server error";
        t_binary body(internalErrorString.begin(), internalErrorString.end());
        setBody(body);
        setDefaultsHeaders();
        setResponseCode(500);
        std::cerr << "Error: invalid error pages path" << std::endl;
    } else {
        read(fd, buffer.data(), buffer.size());
        close(fd);
        setDefaultsHeaders();
        setResponseCode(httpCode);
        setBody(buffer);
    }
}

void    HttpRequest::generateIndexFile(const std::vector<std::string>& fileNames) {
    std::string html = "<html><nav>\n  <ul>\n";
    
    for (std::vector<std::string>::const_iterator it = fileNames.begin(); it != fileNames.end(); it++) {
        html += "    <li><a href=\"" + (*it) + "\">" + (*it) + "</a></li>\n";
    }
    html += "  </ul>\n</nav></html>\n";
    t_binary body(html.begin(), html.end());
    setBody(body);
}


//Called before the request is send. This is the last step before sending to the client.
t_binary    HttpRequest::makeRawResponse(void) {
    std::ostringstream  httpHeaders;
    t_binary            rawResponse;

    httpHeaders << _version << " " << _responseCode << " " << _reasonPhrases[_responseCode] << "\r\n";

    std::map<std::string, std::string>::const_iterator it;
    for (it = _headers.begin(); it != _headers.end(); ++it) {
        httpHeaders << it->first << ": " << it->second << "\r\n";
    }
    
    std::size_t bodySize = _body.size();

    if (_method == "HEAD") {
        httpHeaders << "Content-Length: " << 0 << "\r\n";
        httpHeaders << "\r\n";
    } else {
        httpHeaders << "Content-Length: " << bodySize << "\r\n";
        httpHeaders << "\r\n";
    }
    
    std::string headersStr = httpHeaders.str();    
    rawResponse.insert(rawResponse.end(), headersStr.begin(), headersStr.end()); //insert the full headersStr.
    rawResponse.insert(rawResponse.end(), _body.begin(), _body.end()); //insert the full binary body.

    return (rawResponse);
}

void HttpRequest::setDefaultsHeaders(void) {
    this->setVersion("HTTP/1.1");
    this->setResponseCode(200);
    this->_headers.insert(std::pair<std::string, std::string>("Server", "TheWovenWebserver/0.0.1"));
    this->_headers.insert(std::pair<std::string, std::string>("Connection", "keep-alive"));
}
