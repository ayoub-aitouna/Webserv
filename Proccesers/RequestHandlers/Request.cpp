#include "Include/Request.hpp"

Request::Request(DataPool &dataPool) : dataPool(dataPool)
{
    this->BodyReady = false;
    this->BodyReceiver = NULL;
    this->Type = NON;
    this->isCGIRuning = false;
    env.push_back("REDIRECT_STATUS=1");
}

int Request::GetRequestedResource()
{
    struct stat ResourceState;
    ResourceFilePath = "public" + dataPool.Url;
    if (access(ResourceFilePath.c_str(), F_OK) != 0)
        throw HTTPError(404);
    if (stat(ResourceFilePath.c_str(), &ResourceState) < 0)
    {
        DEBUGOUT(1, COLORED("Error Finiding State of File " << ResourceFilePath, Red));
        throw HTTPError(500);
    }
    if (S_ISREG(ResourceState.st_mode))
        this->dataPool.ResourceType = WB_FILE;
    else if (S_ISDIR(ResourceState.st_mode))
        this->dataPool.ResourceType = WB_DIRECTORY;
    else
        this->dataPool.ResourceType = WB_NEITHER;
    return (NONFILE);
}

std::string Request::GetIndex(std::string &path)
{
    dirent *entry;
    DIR *dir;
    std::string CurrentName;
    size_t index;
    dir = opendir(path.c_str());
    if (dir == NULL)
        return ("");
    while ((entry = readdir(dir)) != NULL)
    {
        CurrentName = entry->d_name;
        if ((index = CurrentName.find("index.", 0, 6)) != std::string::npos)
        {
            DEBUGOUT(1, COLORED(CurrentName << " " << CurrentName.substr(index + 6), Green));
            if (Lstring::IsAlNum(CurrentName, index + 6))
                return (closedir(dir), CurrentName);
        }
    }
    return (closedir(dir), "");
}

void Request::PrintfFullRequest()
{
    for (HeadersIterator i = dataPool.Headers.begin(); i != dataPool.Headers.end(); i++)
        DEBUGOUT(0, COLORED(i->first, Magenta) << " : " << COLORED(i->second, Green));
}

std::string Request::GetFileExtention(std::string &FilePath)
{
    size_t LastDotPos;
    if ((LastDotPos = FilePath.find_last_of(".")) != std::string::npos)
        return (FilePath.substr(LastDotPos));
    return ("");
}

void Request::SetBodyController(int Type, u_int64_t Remaining)
{
    if (Type == Chunked)
        this->BodyReceiver = new ChunkController(this->dataPool);
    else if (Type == Lenght)
        this->BodyReceiver = new LenghtController(this->dataPool, Remaining);
}

/**
 * TODO: check if location support php | python Cgi
 * pass body to cgi as redirection
 * & requested file as first argument
 *
 * example:
 * export all_varaibles look at 'cgi rfc'
 *  body_file | ./php-cgi requested_file.php > return_file
 */

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

/**
 * TODO:
 * When Done Running CGI_PROCESS
 * add it's pid to std::vector<std::pair<pid,outputfilefd> > running_processes;
 * after poll run
 * Loop through them and check if a procces is done excuting
 * parse the response it's response then create a ResponseBuilder Object
 */
bool Request::ExecuteCGI(std::string CGIName, std::string Method)
{
    std::string TmpFileName;
    pid_t pid;
    int FileFd;
    int status_ptr;
    int wait_pid;
    std::string responseBuffer;
    std::string line;
    if (!this->isCGIRuning)
    {
        DEBUGOUT(1, COLORED("RUNNING CGI ..", Blue));

        TmpFileName = "/tmp/" + Lstring::RandomStr(10);
        av.push_back(CGIName);
        this->env.push_back("REQUEST_METHOD=" + Method);
        env.push_back("SCRIPT_FILENAME=" + this->ResourceFilePath);
        env.push_back("QUERY_STRING=" + this->dataPool.Query);

        pid = fork();

        if (pid < 0)
            ServerError("fork() Failed");
        if (pid == 0)
        {
            dup2(IO::OpenFile(TmpFileName.c_str(), "w+"), 1);
            if (execve(CGIName.c_str(), FromVectorToArray(av), FromVectorToArray(env)) < 0)
                ServerError("execve() Failed");
            close(1);
        }
    }
    /**
     * *****PART TO MOVE*****
     */
    if ((wait_pid = waitpid(pid, &status_ptr, 0)) < 0)
        ServerError("waitpid() Failed");
    if (wait_pid == 0)
    {
        DEBUGOUT(1, COLORED("STILL RUNNING CGI ..", Blue));
        return true;
    }
    DEBUGOUT(1, COLORED("DONE RUNNING CGI YAY ..", Blue));

    if (WIFEXITED(status_ptr) && WEXITSTATUS(status_ptr) != 0)
        ServerError("exited with error " + SSTR(WEXITSTATUS(status_ptr)));

    std::ifstream responseFile(TmpFileName.c_str());
    while (getline(responseFile, line))
    {
        if (line.find("Content-type") != std::string::npos)
            this->dataPool.File.ResourceFileType = Lstring::ExtractFromString(line, "Content-type: ", "; ");
        else
            responseBuffer.append(line);
    }

    unlink(TmpFileName.c_str());
    FileFd = IO::OpenFile(TmpFileName.c_str(), "w+");
    write(FileFd, responseBuffer.c_str(), responseBuffer.size());
    close(FileFd);
    this->dataPool.File.Fd = IO::OpenFile(TmpFileName.c_str(), "r+");
    return false;
}

Request::~Request()
{
}