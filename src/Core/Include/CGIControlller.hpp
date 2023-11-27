#ifndef CGICONTROLLER_HPP
#ifndef CGICONTROLLER_HPP
#include "Core.hpp"

class CGIControlller
{
private:
    pid_t RunningProcessId;
    clock_t CgiStart;
    std::string OutputFileName;
    std::string ScriptPath;
public:
    CGIControlller();
    void Execute(void);
pid_t GetRunningProcessId();
    ~CGIControlller();
};
#endif