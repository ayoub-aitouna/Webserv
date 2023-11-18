#include "Include/Request.hpp"

Request::Request(DataPool &dataPool) : dataPool(dataPool)
{
    this->BodyReady = false;
    this->BodyReceiver = NULL;
}

void Request::GetRequestedResource()
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
        DEBUGOUT(1, COLORED(i->first, Magenta) << " : " << COLORED(i->second, Green));
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
/**
void RequestParser::RunCgi(std::string &ResourceFilePath)
{
    std::string Name;
    pid_t pid;
    int FileFd;
    int status_ptr;

    Name = Lstring::RandomStr(10).c_str();
    pid = fork();
    if (pid < 0)
        throw HTTPError(500);
    if (pid == 0)
    {
        FileFd = open(Name.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (pid < 0)
            throw HTTPError(500);
        dup2(FileFd, 1);
        char *arg[] = {(char *)"python3", (char *)ResourceFilePath.c_str(), NULL};
        if (execve("/usr/bin/python3", arg, NULL) < 0)
            throw HTTPError(500);
        close(1);
    }
    else
    {
        waitpid(pid, &status_ptr, 0);
        this->dataPool.File.ResourceFileType = this->dataPool.Content_Types["html"];
        this->dataPool.File.Fd = open(Name.c_str(), O_RDONLY);
    }
}

*/

Request::~Request()
{
}