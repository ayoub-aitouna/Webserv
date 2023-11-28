#include "Include/DeleteRequest.hpp"

DeleteRequest::DeleteRequest(DataPool &dataPool) : Request(dataPool)
{
}

bool DeleteRequest::HandleRequest(std::string &data)
{
    (void)data;
    PrintfFullRequest();
    return (this->GetRequestedResource());
}

void DeleteRequest::DeleteDirectory(std::string Dir)
{
    struct stat fileInfo;
    dirent *entry;
    std::string entrypath;
    DIR *dir;

    if (stat(Dir.c_str(), &fileInfo) < 0)
        HTTPError(500);
    if (!(fileInfo.st_mode & S_IWUSR))
        HTTPError(403);

    dir = opendir(Dir.c_str());
    if (dir == NULL)
        return;
    while ((entry = readdir(dir)) != NULL)
    {
        if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..")
            continue;
        entrypath = Lstring::RTrim(Dir, "/") + "/" + entry->d_name;
        if (stat(entrypath.c_str(), &fileInfo) < 0)
            HTTPError(500);
        if (!(fileInfo.st_mode & S_IWUSR))
            HTTPError(403);
        if (S_ISDIR(fileInfo.st_mode))
            DeleteDirectory(entrypath);
        else if (unlink(entrypath.c_str()) < 0)
            throw HTTPError(500);
    }
    if (rmdir(Dir.c_str()) < 0)
    {
        perror("Error removing directory");
        throw HTTPError(500);
    }
}

int DeleteRequest::GetRequestedResource()
{
    Request::GetRequestedResource();

    std::string IndexFileName;
    std::string FileExtention;
    struct stat fileInfo;

    if (this->dataPool.ResourceType == WB_DIRECTORY)
    {
        if (*(this->dataPool.Url.end() - 1) != '/')
            throw HTTPError(409);
        ResourceFilePath.append(IndexFileName);
    }
    FileExtention = GetFileExtention(ResourceFilePath);
    if (Containes(this->dataPool.ServerConf->GetCgi(), FileExtention))
    {
        if (IndexFileName.empty())
            throw HTTPError(403);
        return (Execute(ResourceFilePath, "DELETE"), false);
    }

    if (this->dataPool.ResourceType == WB_DIRECTORY)
        return (DeleteDirectory(ResourceFilePath), dataPool.ResponseStatus = 204, true);
    else if (this->dataPool.ResourceType == WB_FILE)
    {
        if (unlink(ResourceFilePath.c_str()) < 0)
        {
            if (stat(ResourceFilePath.c_str(), &fileInfo) < 0)
                HTTPError(500);
            if (!(fileInfo.st_mode & S_IWUSR))
                HTTPError(403);
            throw HTTPError(500);
        }
        throw HTTPError(204);
    }
    return true;
}

DeleteRequest::~DeleteRequest()
{
}