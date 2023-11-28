#ifndef CGICONTROLLER_HPP
#define CGICONTROLLER_HPP
#include "Core.hpp"
#include "sys/wait.h"

#define ErrorPrefix "-Error"
class CGIControlller
{
private:
    pid_t RunningProcessId;
    clock_t CgiStart;
    std::string OutputFileName;
    std::string ScriptPath;
    std::vector<std::string> av;
    std::vector<std::string> env;
    HeadersType Headers;
    std::string BodyContent;

private:
    DataPool &data;
    std::string CgiPath;
    std::string RequestMethod;
    std::string BodyFile;

private:
    CGIControlller();

public:
    CGIControlller(DataPool &data, std::string &ScriptPath, std::string &BodyFile, std::string &RequestMethod);
    void FillUpEnv();
    void Execute();
    bool ParseCGIOutput(HeadersType &ResponseHeaders);
    HeadersType ParseCgiHeaders();
    pid_t GetRunningProcessId();
    ~CGIControlller();
};
#endif