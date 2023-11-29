#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "../../RequestBodyController/Include/BodyController.hpp"
#include "../../RequestBodyController/Include/LenghtController.hpp"
#include "../../RequestBodyController/Include/ChunkController.hpp"
#include "../../Include/AutoIndex.hpp"
#include "../../../Lib/Lstring.hpp"
#include <sys/wait.h>
#include "../../Include/CGIController.hpp"
class Request
{
public:
    DataPool &dataPool;
    BodyController *BodyReceiver;
    std::string ResourceFilePath;
    bool BodyReady;
    std::string CGIFileName;
    pid_t CGIProcessId;
    std::vector<std::string> env;
    std::vector<std::string> av;
    clock_t CgiStart;
    CGIControlller *cgiController;

public:
    Request(DataPool &dataPool);
    virtual bool HandleRequest(std::string &data) = 0;
    virtual int GetRequestedResource();
    void PrintfFullRequest();
    std::string GetIndex(std::string &path);
    std::string GetFileExtention(std::string &FilePath);
    void SetBodyController(int Type, u_int64_t Remaining);
    CGIControlller *GetCGIController();
    pid_t GetRunningProcessId();
    void Execute(std::string ResourceFilePath, std::string Method);
    virtual ~Request();
};
std::string GetFileRoot(std::string FilePath);
#endif