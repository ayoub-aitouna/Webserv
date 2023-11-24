#include "Include/Request.hpp"

Request::Request(DataPool &dataPool) : dataPool(dataPool)
{
    this->BodyReady = false;
    this->BodyReceiver = NULL;
    this->CGIProcessId = 0;
}

int Request::GetRequestedResource()
{
    struct stat ResourceState;
    ResourceFilePath = dataPool.ServerConf->GetRoot(dataPool.Url);

    if (access(ResourceFilePath.c_str(), F_OK) != 0)
        throw HTTPError(404);

    if (stat(ResourceFilePath.c_str(), &ResourceState) < 0)
        throw HTTPError(500);

    if (S_ISREG(ResourceState.st_mode))
        this->dataPool.ResourceType = WB_FILE;
    else if (S_ISDIR(ResourceState.st_mode))
        this->dataPool.ResourceType = WB_DIRECTORY;
    else
        this->dataPool.ResourceType = WB_NEITHER;

    std::pair<int, std::string> redirection =
        this->dataPool.ServerConf->GetRedirection(dataPool.Url);

    if (redirection.first != 0)
    {
        DEBUGOUT(1, "S :" << redirection.first << " Location " << redirection.second);
        this->dataPool.Location = redirection.second;
        throw HTTPError(redirection.first);
    }
    return true;
}

std::string Request::GetIndex(std::string &path)
{
    dirent *entry;
    DIR *dir;
    std::string CurrentName;
    std::vector<std::string> indexList;

    dir = opendir(path.c_str());
    if (dir == NULL)
        return ("");
    indexList = this->dataPool.ServerConf->GetIndex();
    if (indexList.empty())
        return (closedir(dir), "");
    while ((entry = readdir(dir)) != NULL)
    {
        CurrentName = entry->d_name;
        for (size_t i = 0; i < indexList.size(); i++)
        {
            if (CurrentName == indexList.at(i))
                return (closedir(dir), CurrentName);
        }
    }
    return (closedir(dir), "");
}

void Request::PrintfFullRequest()
{
    bool Print = false;
    DEBUGOUT(Print, COLORED("------------------------------", Green));

    DEBUGOUT(Print, COLORED("Requested File  " << this->ResourceFilePath << " Method : " << (this->dataPool.Method == POST ? "POST" : "GET"), Green));
    for (HeadersIterator i = dataPool.Headers.begin(); i != dataPool.Headers.end(); i++)
        DEBUGOUT(Print, COLORED(i->first, Magenta) << " : " << COLORED(i->second, Green));
    DEBUGOUT(Print, COLORED("------------------------------", Green));
}

std::string Request::GetFileExtention(std::string &FilePath)
{
    size_t LastDotPos;
    if ((LastDotPos = FilePath.find_last_of(".")) != std::string::npos)
        return (FilePath.substr(LastDotPos + 1));
    return ("");
}

void Request::SetBodyController(int Type, u_int64_t Remaining)
{
    if (Type == Chunked)
        this->BodyReceiver = new ChunkController(this->dataPool);
    else if (Type == Lenght)
        this->BodyReceiver = new LenghtController(this->dataPool, Remaining);
}

char **FromVectorToArray(std::vector<std::string> vec)
{
    char **Array = new char *[vec.size() + 1];
    for (size_t i = 0; i < vec.size(); i++)
    {
        Array[i] = new char[vec.at(i).size() + 1];
        std::strcpy(Array[i], vec.at(i).c_str());
    }
    Array[vec.size()] = NULL;
    return (Array);
}

void ServerError(std::string Msg)
{
    DEBUGOUT(1, COLORED("SERVER ERROR : " << Msg, Red));
    throw HTTPError(500);
}

// std::string GetFileRoot(std::string FilePath)
// {
//     // if(FilePath.fi)
// }

/**
 * TODO: all values that abtiened from clinet
 * add to them prefix HTTP example:
 * see: https://fr.wikipedia.org/wiki/Variables_d%27environnement_CGI
 */
void Request::ExecuteCGI(std::string CGIName, std::string Method)
{

    CGIFileName = "/tmp/" + Lstring::RandomStr(10);
    DEBUGOUT(1, COLORED("CGI CGIFileName " << CGIFileName, Blue));

    av.push_back(CGIName);
    env.push_back("REDIRECT_STATUS=1");
    this->env.push_back("REQUEST_METHOD=" + Method);
    env.push_back("SCRIPT_FILENAME=" + this->ResourceFilePath);
    env.push_back("QUERY_STRING=" + this->dataPool.Query);
    env.push_back("HTTP_COOKIE=" + GetHeaderAttr(dataPool.Headers, "cockie"));
    if (Method == "POST")
    {
        env.push_back("CONTENT_LENGTH=" + GetHeaderAttr(dataPool.Headers, "Content-Length"));
        env.push_back("CONTENT_TYPE=" + GetHeaderAttr(dataPool.Headers, "Content-Type"));
    }
    if ((CGIProcessId = fork()) < 0)
        ServerError("fork() Failed");
    if (CGIProcessId == 0)
    {
        // if(chdir())
        dup2(IO::OpenFile(CGIFileName.c_str(), "w+"), 1);
        if (Method == "POST")
            dup2(this->BodyReceiver->GetReadFd(), 0);

        if (execve(CGIName.c_str(), FromVectorToArray(av), FromVectorToArray(env)) < 0)
            exit(1);
        close(1);
    }
}

bool Request::ParseCGIOutput()
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

    if ((wait_pid = waitpid(CGIProcessId, &status_ptr, 0)) < 0)
        ServerError("waitpid() Failed");
    if (wait_pid == 0)
    {
        DEBUGOUT(1, COLORED("STILL RUNNING CGI ..", Blue));
        return false;
    }
    this->CGIProcessId = 0;

    if (WIFEXITED(status_ptr) && WEXITSTATUS(status_ptr) != 0)
        ServerError("exited with error " + SSTR(WEXITSTATUS(status_ptr)));

    std::ifstream responseFile(CGIFileName.c_str());
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
        Headers[Attr.at(0)] = Attr.at(1);
    }

    dataPool.File.ResourceFileType = GetHeaderAttr(Headers, "Content-type");
    dataPool.File.ResourceFileType =
        dataPool.File.ResourceFileType
            .substr(0, dataPool.File.ResourceFileType.find_first_of(';'));

    unlink(CGIFileName.c_str());
    FileFd = IO::OpenFile(CGIFileName.c_str(), "w+");
    write(FileFd, responseBuffer.c_str(), responseBuffer.size());
    close(FileFd);
    dataPool.File.Fd = IO::OpenFile(CGIFileName.c_str(), "r+");
    dataPool.ResponseStatus = OK;
    return true;
}

Request::~Request()
{
    delete this->BodyReceiver;
}