#pragma once

# include <iostream>
# include <map>
# include <sstream>
# include <exception>

class HttpRequest {
    private:
        std::string _method;
        std::string _version;
        std::string _path;
        std::string _server;
        int         _responseCode;
        std::map<std::string, std::string> _headers;
        std::string _body;

        //METHODS
        void        parse(const std::string &rawRequest);

    public:
        HttpRequest(void);
        HttpRequest(const std::string &rawRequest);
        ~HttpRequest(void);

        //GETTERS
        std::string getMethod(void) const;
        std::string getVersion(void) const;
        std::string getPath(void) const;
        std::string getHeader(const std::string &key) const;
        std::string getBody(void) const;

        //SETTERS
        void    setMethod(std::string &method);
        void    setResponseCode(int code);
        void    setVersion(const std::string &str);
        void    setHeaders(std::map<std::string, std::string> &headers);
        void    setBody(const std::string &body);

        //METHODS:
        std::string makeRawResponse(void);
        void        makeReponse(const std::string &body);
};