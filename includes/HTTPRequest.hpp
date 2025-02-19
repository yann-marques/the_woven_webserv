#pragma once

# include <iostream>
# include <map>
# include <sstream>
# include <exception>
# include <vector>
# include <unistd.h>
# include <sys/types.h>
# include <fcntl.h>

//METHODS

#define GET "GET"
#define POST "POST"
#define DELETE "DELETE"
#define PUT "PUT"
#define OPTIONS "OPTIONS"

//ERROR CODES

#define HTTP_OK 200
#define HTTP_CREATED 201
#define HTTP_ACCEPTED 202

#define HTTP_TEMPORARY_REDIRECT 307
#define HTTP_PERMANENT_REDIRECT 308
#define HTTP_MOVED_PERMANENTLY 301

#define HTTP_BAD_REQUEST 400
#define HTTP_FORBIDDEN 403
#define HTTP_NOT_FOUND 404
#define HTTP_METHOD_NOT_ALLOWED 405
#define HTTP_NOT_ACCEPTABLE 406
#define HTTP_PAYLOAD_TOO_LARGE 413

#define HTTP_INTERNAL_SERVER_ERROR 500
#define HTTP_BAD_GATEWAY 502
#define HTTP_SERVICE_UNAVAILABLE 503


class HttpRequest {
    private:
        std::string                         _method;
        std::string                         _version;
        std::string                         _path;
        std::string                         _server;
        int                                 _responseCode;
        std::string                         _body;
        std::string                         _rootPath;

        std::map<std::string, std::string>  _headers;
        std::map<int, std::string>          _reasonPhrases;

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
        std::string getRootPath(void) const;

        //SETTERS
        void    setMethod(std::string &method);
        void    setResponseCode(int code);
        void    setVersion(const std::string &str);
        void    setHeaders(std::map<std::string, std::string> &headers);
        void    setBody(const std::string &body);
        void    setRootPath(std::string &rootPath);

        //METHODS:
        std::string makeRawResponse(void);
        void        setDefaultsHeaders(void);
        void        initReasons(void);
        void        makeError(int httpCode);
        void        generateIndexFile(const std::vector<std::string>& fileNames);

        //EXCETPIONS
        class	OpenFileException: public std::exception {
			public:
				const char*	what() const throw();
		};
};