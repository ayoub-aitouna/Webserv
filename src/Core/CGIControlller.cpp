#include "Include/CGIControlller.hpp"

CGIControlller::CGIControlller()
{
    this->RunningProcessId = 0;
}

void CGIControlller::Execute(std::string CgiPath, std::string RequestMethod)
{
    OutputFileName = "/tmp/" + Lstring::RandomStr(10);

    av.push_back(CgiPath);
    av.push_back(GetFileName(this->ResourceFilePath));

    env.push_back("REDIRECT_STATUS=1");
    env.push_back("REQUEST_METHOD=" + RequestMethod);
    env.push_back("SCRIPT_FILENAME=" + GetFileName(this->ResourceFilePath));
    env.push_back("HTTP_COOKIE=" + GetHeaderAttr(dataPool.Headers, "Cookie"));
    env.push_back("QUERY_STRING=" + this->dataPool.Query);

    if (RequestMethod == "POST")
    {
        env.push_back("CONTENT_LENGTH=" + GetHeaderAttr(dataPool.Headers, "Content-Length"));
        env.push_back("CONTENT_TYPE=" + GetHeaderAttr(dataPool.Headers, "Content-Type"));
    }

    if ((RunningProcessId = fork()) < 0)
        ServerError("fork() Failed");

    if (RunningProcessId == 0)
    {
        if (chdir(GetFileRoot(this->ResourceFilePath).c_str()) < 0)
            exit(1);
        dup2(IO::OpenFile(OutputFileName.c_str(), "w+"), 1);
        dup2(IO::OpenFile((OutputFileName + "-Error").c_str(), "w+"), 2);
        if (RequestMethod == "POST")
            dup2(this->BodyReceiver->GetReadFd(), 0);
        if (execve(CgiPath.c_str(), FromVectorToArray(av), FromVectorToArray(env)) < 0)
            exit(1);
    }
    this->CgiStart = clock();
}


pid_t CGIControlller::GetRunningProcessId()
{
    return (this->RunningProcessId)
}



bool CGIControlller::ParseCGIOutput()
{
    int status_ptr = 0;
    int FileFd;
    int wait_pid;
    bool passedHeader;
    std::string responseBuffer;
    std::string line;
    std::vector<std::string> HeadersBuffer;
    std::map<std::string, std::string> Headers;

    if (this->CGIProcessId == 0)
        return false;

    if ((wait_pid = waitpid(CGIProcessId, &status_ptr, WNOHANG)) < 0)
        ServerError("waitpid() Failed");
    if (wait_pid == 0)
    {
        if (this->CgiStart != -1 && (clock() - this->CgiStart) > 25 * CLOCKS_PER_SEC)
            throw HTTPError(504);
        return false;
    }
    this->CGIProcessId = 0;
    std::ifstream responseFile;
    if (WIFEXITED(status_ptr) && WEXITSTATUS(status_ptr) != 0)
    {
        // dataPool.File.Fd = IO::OpenFile(CGIFileName.c_str(), "r+");
        // dataPool.File.ResourceFileType = "text/html";
        // dataPool.ResponseStatus = OK;
        // // return true; "-Error"
        // responseFile.open((CGIFileName + "-Error").c_str());
    }
    else
        responseFile.open(CGIFileName.c_str());
    passedHeader = false;

    while (getline(responseFile, line))
    {
        if (!passedHeader && line == "\r")
            passedHeader = true;
        else if (!passedHeader)
            HeadersBuffer.push_back(line);
        else
            responseBuffer.append(line);
    }

    for (size_t i = 0; i < HeadersBuffer.size(); i++)
    {
        std::vector<std::string> Attr = Lstring::Split(HeadersBuffer.at(i), ": ");
        if (Attr.size() < 2)
            continue;
        Headers[Attr.at(0)] = Attr.at(1);
    }

    dataPool.File.ResourceFileType = GetHeaderAttr(Headers, "Content-type");
    dataPool.File.ResourceFileType =
        dataPool.File.ResourceFileType
            .substr(0, dataPool.File.ResourceFileType.find_first_of(';'));

    unlink(CGIFileName.c_str());
    if (this->BodyReceiver)
        unlink(this->BodyReceiver->GetFileName().c_str());
    FileFd = IO::OpenFile(CGIFileName.c_str(), "w+");
    write(FileFd, responseBuffer.c_str(), responseBuffer.size());
    close(FileFd);
    dataPool.File.Fd = IO::OpenFile(CGIFileName.c_str(), "r+");
    dataPool.ResponseStatus = OK;
    return true;
}

CGIControlller::~CGIControlller()
{
}
