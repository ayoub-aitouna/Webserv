#ifndef RequestParser_HPP
#define RequestParser_HPP

#include "Proccesers.hpp"
#include "AutoIndex.hpp"
#include "../RequestBodyController/Include/BodyController.hpp"
#include "../RequestBodyController/Include/LenghtController.hpp"
#include "../RequestBodyController/Include/ChunkController.hpp"

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
    bool BodyReady;
    std::string buffer;

public:
    DataPool dataPool;
};

std::string CreateRow(int type, std::string Name, u_int64_t size, std::string permissions);
#endif