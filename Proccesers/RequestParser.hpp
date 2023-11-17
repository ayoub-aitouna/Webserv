#ifndef RequestParser_HPP
#define RequestParser_HPP

#include "Proccesers.hpp"
#include "AutoIndex.hpp"
#include "RequestBodyController/BodyController/BodyController.hpp"
#include "RequestBodyController/LenghtController/LenghtController.hpp"
#include "RequestBodyController/ChunkController/ChunkController.hpp"

#include "../HTTPError/HTTPError.hpp"

class RequestParser
{

public:
    RequestParser();
    bool Parse(std::string data);
    void ParseHeaders(std::string data);
    void ParseUrl(std::string &Url);
    void GetResourceFilePath();
    void PrintfFullRequest();

private:
    BodyController *BodyReceiver;
    std::string buffer;
    bool BodyReady;

public:
    DataPool dataPool;
};

std::string CreateRow(int type, std::string Name, u_int64_t size, std::string permissions);
#endif