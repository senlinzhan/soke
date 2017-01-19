#include "Requests.hpp"
#include "utils.hpp"
#include <stdexcept>
#include <iostream>

using std::string;
using std::unordered_map;

Requests::Requests()
    : curl_(curl_easy_init()),
      userAgent_(string_format("libcurl/%s", curl_version_info(CURLVERSION_NOW)->version))
{
    if (!curl_)
    {
	throw std::runtime_error("cannot create Requests object");	
    }
    curl_easy_setopt(curl_,CURLOPT_USERAGENT, userAgent_.c_str());
}

Requests::~Requests()
{
    if (curl_)
    {
	curl_easy_cleanup(curl_);
    }
}

std::string Requests::userAgent() const
{
    return userAgent_;    
}


size_t Requests::callback(void *inputContent, size_t size, size_t nmemb, void *userbuffer)
{   
    // size of the input buffer
    auto inputSize = size * nmemb;	 
    
    // buffer provided by user
    string *buffer = static_cast<string *>(userbuffer);
    
    // append the content of input buffer to the user buffer
    buffer->append(static_cast<char *>(inputContent), inputSize);

    // return the size of the input buffer
    return inputSize;
};    


Requests::RespType Requests::postJSON(const string &url, const string &payload)
{
    // setting url 
    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());    

    // setting http method
    curl_easy_setopt(curl_, CURLOPT_POST, 1L);
    
    // setting http header
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers);
    
    // setting the data for post
    curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, payload.c_str());

    // setting the data size without having libcurl do a strlen() to measure the data size
    curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE, static_cast<long>(payload.size()));   

    // buffer for save response message
    string buffer;
    
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, static_cast<void *>(&buffer));
 
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, callback);    
    
    CURLcode res = curl_easy_perform(curl_);
    if (res != CURLE_OK)
    {
	curl_slist_free_all(headers);    
	throw std::runtime_error(string("post error: curl_easy_perform(): ") +
				 curl_easy_strerror(res));
    }
    
    long statusCode;    
    curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &statusCode);

    curl_slist_free_all(headers);    
    return {statusCode, buffer};
}


std::ostream &operator<<(std::ostream &os, const Requests::RespType &resp)
{
    os << "status_code: " << resp.code << std::endl
       << "content: " << resp.content << std::endl;   
    return os;    
}
