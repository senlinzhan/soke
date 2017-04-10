#ifndef REQUESTS_H
#define REQUESTS_H

#include <curl/curl.h>
#include <tuple>
#include <string>
#include <unordered_map>
#include <iostream>


class CURLGlobal
{
public:
    static CURLGlobal &Instance()
    {
	static CURLGlobal instance;
	return instance;      
    }
    
    CURLGlobal(const CURLGlobal &) = delete;
    CURLGlobal &operator=(const CURLGlobal &) = delete;

private:
    CURLGlobal() {
	curl_global_init(CURL_GLOBAL_ALL);
    }
    
    ~CURLGlobal() {
	curl_global_cleanup();
    }
};


class Requests
{
public:
    struct RespType
    {
	long        code;
	std::string content;	
    };    
    
    Requests();
    ~Requests();

    Requests(const Requests &) = delete;
    Requests &operator=(const Requests &) = delete;    
    
    std::string userAgent() const;    
    
    RespType postJSON(const std::string &url, const std::string &payload);    
    
private:
    static size_t callback(void *inputContent, size_t size, size_t nmemb, void *userbuffer);    
    
    CURL *curl_;
    const std::string userAgent_;
};


std::ostream &operator<<(std::ostream &os, const Requests::RespType &resp);

#endif /* REQUESTS_H */
