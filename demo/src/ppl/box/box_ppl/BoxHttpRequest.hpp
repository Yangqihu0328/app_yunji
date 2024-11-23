#ifndef BOX_HTTP_REQUEST_H
#define BOX_HTTP_REQUEST_H

#include <string>

class BoxHttpRequest
{
public:
    static std::string Send(const std::string& method, const std::string& url, const std::string& headers, const std::string& params, long timeout);
};

#endif // BOX_HTTP_REQUEST_H
