#ifndef MEDIUM
#define MEDIUM

#include <string>
#include <map>
#include "ResourceFile.hpp"

enum
{
    Get,
    Post,
    Delete,
    OTHER
};
typedef std::map<std::string, std::string>::iterator HeadersIterator;

class Medium
{
public:
    Medium(ResourceFile File, int ResponseBuilderStatus);
    Medium(int statusCode);
    Medium(Medium &medium);

    Medium();
    ~Medium();

    std::string GetHeaderAttr(std::string Name);
    void AddHeaderAttr(std::string Name, std::string Value);
    std::map<std::string, std::string> GetHeaders() const;

    std::string GetUrl() const;
    void SetUrl(std::string Url);

    std::string GetBody() const;
    void AppendBody(std::string buffer);
    void SetBody(std::string body);

    ResourceFile &GetFile();
    void SetFile(ResourceFile File);

    int GetMethod() const;
    void SetMethod(int Method);

    int GetResponseStatus() const;
    void SetResponseStatus(int ResponseStatus);

private:
    std::map<std::string, std::string> Headers;
    ResourceFile File;
    std::string Url;
    std::string Query;
    std::string body;
    int ResponseStatus;

    int Method;
};

#endif
