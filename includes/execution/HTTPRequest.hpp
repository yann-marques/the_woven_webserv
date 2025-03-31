/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: locharve <locharve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 14:09:25 by locharve          #+#    #+#             */
/*   Updated: 2025/03/31 14:09:27 by locharve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

# include <iostream>
# include <map>
# include <sstream>
# include <exception>
# include <vector>
# include <unistd.h>
# include <sys/types.h>
# include <fcntl.h>
# include <cstring>
# include <ctime>
# include <sys/stat.h>

//# include "VServ.hpp"
# include "Rules.hpp"

//METHODS

#define GET "GET"
#define POST "POST"
#define DELETE "DELETE"
#define PUT "PUT"
#define OPTIONS "OPTIONS"
#define HEAD "HEAD"

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

enum RequestDirection {
    HTTP_REQUEST,
    HTTP_RESPONSE
};

typedef std::vector<unsigned char> t_binary;

class HttpRequest {
    private:
        int                                 _clientFd;
        std::string                         _method;
        std::string                         _version;
        std::string                         _path;
        std::string                         _server;
        int                                 _responseCode;
        t_binary                            _body;
        size_t                              _bodySize;
        std::string                         _rootPath;
        std::multimap<std::string, std::string>  _headers;
        std::map<int, std::string>          _reasonPhrases;
        Rules*                              _rules;
        std::string                         _cgiExt;
        RequestDirection                    _direction;

        //BONUS
        std::map< std::string, std::string >    _cookies;

    public:
        HttpRequest(void);
        HttpRequest(RequestDirection direction, t_binary &rawRequest);
        ~HttpRequest(void);
        
        //GETTERS
        std::string getMethod(void) const;
        std::string getVersion(void) const;
        std::string getPath(void) const;
        std::string getHeader(const std::string &key) const;
        const   t_binary&   getBody(void) const;
        std::string getRootPath(void) const;
        std::string getRawHeaders(void) const;
        std::multimap<std::string, std::string> getHeaders(void) const;
        Rules*      getRules(void) const; 
        std::string getCgiExt(void) const;
        int         getClientFD(void) const;
        int         getResponseCode(void) const;
        size_t      getBodySize(void) const;
        
        //SETTERS
        void    setMethod(std::string &method);
        void    setResponseCode(int code);
        void    setVersion(const std::string &str);
        void    setHeaders(std::multimap<std::string, std::string> &headers);
        void    setBody(const t_binary &body);
        void    setRootPath(std::string &rootPath);
        void    setRules(Rules* rules);
        void    setCgiExt(std::string ext);
        void    setClientFD(int fd);
        void    setBodySize(size_t size); 
        
        //METHODS:
        t_binary    makeRawResponse(void);
        void        parseRequest(const t_binary &rawRequest);
        void        setDefaultsHeaders(void);
        void        initReasons(void);
        void        makeError(int httpCode, HttpRequest &request);
        void        internalError(void);
        void        generateIndexFile(const std::vector<std::string>& fileNames);
        void        log(void);

        // BONUS
# ifdef BONUS
		t_mmap_range< std::string, std::string >::t	getCookiesRange() const;
		std::set< std::string >	parseRequestCookieRange(t_mmap_range< std::string, std::string >::t range);
		std::set< std::string >	getCookieSet();
		void	setResponseCookies(const std::set< std::string >& requestCookies);
# endif
        
		//EXCEPTIONS
        class	OpenFileException: public std::exception {
			public:
				const char*	what() const throw();
		};
        class	MalformedHttpHeader: public std::exception {
			public:
				const char*	what() const throw();
		};
};
