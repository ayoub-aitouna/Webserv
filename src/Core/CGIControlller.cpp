#include "Include/CGIController.hpp"

CGIControlller::CGIControlller(DataPool &data, std::string &ScriptPath, std::string &BodyFile,
                               std::string &RequestMethod) : data(data)
{
    this->RunningProcessId = 0;
    this->BodyFile = BodyFile;
    this->CgiPath = data.ServerConf->GetCgi_path();
    this->RequestMethod = RequestMethod;
    this->ScriptPath = ScriptPath;
    this->Execute();
}

char **FromVectorToArray(std::vector<std::string> vec)
{
    char **Array = new char *[vec.size() + 1];
    for (size_t i = 0; i < (vec.size()); i++)
    {
        Array[i] = new char[vec.at(i).length() + 1];
        std::strcpy(Array[i], vec.at(i).c_str());
    }
    Array[vec.size()] = NULL;
    return (Array);
}

std::string GetFileRoot(std::string FilePath)
{
    size_t index;

    if (FilePath.empty())
        return (FilePath);
    if ((index = FilePath.find_last_of("/")) == std::string::npos)
        return (FilePath);
    return (FilePath.substr(0, ++index));
}

std::string GetFileName(std::string FilePath)
{
    size_t index;

    if (FilePath.empty())
        return (FilePath);
    if ((index = FilePath.find_last_of("/")) == std::string::npos)
        return (FilePath);
    return (FilePath.substr(index + 1));
}

void CGIControlller::FillUpEnv()
{
    av.push_back(CgiPath);
    av.push_back(GetFileName(ScriptPath));

    env.push_back("REDIRECT_STATUS=1");
    env.push_back("REQUEST_METHOD=" + RequestMethod);
    env.push_back("SCRIPT_FILENAME=" + GetFileName(ScriptPath));
    env.push_back("HTTP_COOKIE=" + GetHeaderAttr(data.Headers, "Cookie"));
    env.push_back("QUERY_STRING=" + data.Query);

    if (RequestMethod == "POST")
    {
        env.push_back("CONTENT_LENGTH=" + GetHeaderAttr(data.Headers, "Content-Length"));
        env.push_back("CONTENT_TYPE=" + GetHeaderAttr(data.Headers, "Content-Type"));
    }
}

void CGIControlller::Execute()
{
    OutputFileName = "/tmp/" + Lstring::RandomStr(10);
    FillUpEnv();

    if ((RunningProcessId = fork()) < 0)
        throw std::runtime_error("fork() failed");

    if (RunningProcessId == 0)
    {
        if (chdir(GetFileRoot(ScriptPath).c_str()) < 0)
            exit(1);
        dup2(IO::OpenFile(OutputFileName.c_str(), "w+"), 1);
        dup2(IO::OpenFile((OutputFileName + ErrorPrefix).c_str(), "w+"), 2);
        if (RequestMethod == "POST")
            dup2(IO::OpenFile(BodyFile.c_str(), "r+"), 0);
        if (execve(CgiPath.c_str(), FromVectorToArray(av), FromVectorToArray(env)) < 0)
            exit(1);
    }
    this->CgiStart = clock();
}

pid_t CGIControlller::GetRunningProcessId()
{
    return (this->RunningProcessId);
}

HeadersType CGIControlller::ParseCgiHeaders()
{
    std::ifstream responseFile(OutputFileName.c_str());
    std::ifstream ErrorFile((OutputFileName + ErrorPrefix).c_str());
    std::vector<std::string> HeadersVec;
    std::vector<std::string> HeaderAttr;
    std::string ErrorContent;
    HeadersType Headers;
    std::string line;

    DEBUGOUT(0, "--------------------\n"
                    << COLORED(STREAM_TO_STRING(responseFile.rdbuf()), Green)
                    << "--------------------");
    responseFile.close();
    responseFile.open(OutputFileName.c_str());

    ErrorContent = STREAM_TO_STRING(ErrorFile.rdbuf());
    while (getline(responseFile, line))
    {
        if (line == "\r" || line.empty())
            break;
        HeadersVec.push_back(line);
    }
    BodyContent = ErrorContent + static_cast<std::ostringstream &>(std::ostringstream() << responseFile.rdbuf()).str();
    for (size_t i = 0; i < HeadersVec.size(); i++)
    {
        HeaderAttr = Lstring::Split(HeadersVec.at(i), ": ");
        if (HeaderAttr.size() < 2)
            continue;
        Headers[HeaderAttr.at(0)] = HeaderAttr.at(1);
    }
    return (Headers);
}

bool CGIControlller::ParseCGIOutput(HeadersType &ResponseHeaders)
{
    int status_ptr = 0;
    int FileFd;
    int wait_pid;
    std::string ContentType;

    if (this->RunningProcessId == 0)
        return false;

    if ((wait_pid = waitpid(RunningProcessId, &status_ptr, WNOHANG)) < 0)
        throw std::runtime_error("waitpid() Failed");
    if (wait_pid == 0)
    {
        if (this->CgiStart != -1 && (clock() - this->CgiStart) > 25 * CLOCKS_PER_SEC)
            throw HTTPError(504);
        return false;
    }
    this->RunningProcessId = 0;
    ResponseHeaders = ParseCgiHeaders();
    ContentType = GetHeaderAttr(Headers, "Content-type");
    data.File.ResourceFileType = ContentType.empty() ? "text/html" : ContentType;
    unlink(BodyFile.c_str());
    FileFd = IO::OpenFile(OutputFileName.c_str(), "wt+");
    write(FileFd, BodyContent.c_str(), BodyContent.size());
    close(FileFd);
    data.File.Fd = IO::OpenFile(OutputFileName.c_str(), "r+");
    data.ResponseStatus = OK;
    return true;
}

CGIControlller::~CGIControlller()
{
}
