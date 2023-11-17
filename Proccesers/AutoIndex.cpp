#include "Include/AutoIndex.hpp"

std::string CreateRow(int type, std::string Name, u_int64_t size, std::string permissions)
{
    std::ostringstream ss;

    ss << "<tr class='Row'>"
       << "<td class=\"" << (type == 1 ? "folder-icon" : type == 2 ? "file-icon"
                                                                   : "")
       << "\"></td>"
       << "<td> <a href='" << Name << "'>" << Name << "</a></td>"
       << (size == 0 ? "<td>-</td>" : ("<td>" + SSTR(size) + " Mb </td>"))
       << "<td>" << permissions << "</td>"
       << "</tr>";
    return (ss.str());
}

std::string TemplateStart(const std::string &path)
{
    std::string Template =
        "<!DOCTYPE html>"
        "<html lang=\"en\">"
        "<head>"
        "    <meta charset=\"UTF-8\">"
        "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
        "    <title>" +
        path + "</title>"
               "    <style>"
               "        body {"
               "            font-family: Arial, sans-serif;"
               "            margin: 0;"
               "            padding: 0;"
               "            background-color: #e4e4e4;"
               "        }"
               "        table {"
               "            width: 90%;"
               "            background-color: #f5f5f5;"
               "            margin: 20px auto;"
               "            border-collapse: collapse;"
               "            overflow: hidden;"
               "        }"
               "        th, td {"
               "            padding: 15px;"
               "            text-align: left;"
               "            border-bottom: 1px solid #ddd;"
               "        }"
               "        th {"
               "            background-color: #333;"
               "            color: #fff;"
               "        }"
               "        tr:hover {"
               "            background-color: #f5f5f5;"
               "        }"
               "        .Row > td:first-of-type {"
               "            width: 40px;"
               "        }"
               "        .folder-icon::before {"
               "            content: '\\1F4C1';"
               "            font-size: 20px;"
               "            margin-right: 10px;"
               "            color: #2196F3;"
               "        }"
               "        .file-icon::before {"
               "            content: '\\1F4C4';"
               "            font-size: 20px;"
               "            margin-right: 10px;"
               "            color: #4CAF50;"
               "        }"
               "    </style>"
               "</head>"
               "<body>"
               "    <table>"
               "          <thead>"
               "    <tr>"
               "        <th>Type</th>"
               "        <th>Name</th>"
               "        <th>Size</th>"
               "        <th>Permissions</th>"
               "    </tr>"
               "</thead>";
    return (Template);
}

std::string GetPermission(struct stat fileInfo)
{
    std::ostringstream ss;
    ss << (S_ISDIR(fileInfo.st_mode) ? 'd' : '-');
    ss << ((fileInfo.st_mode & S_IRUSR) ? 'r' : '-');
    ss << ((fileInfo.st_mode & S_IWUSR) ? 'w' : '-');
    ss << ((fileInfo.st_mode & S_IXUSR) ? 'x' : '-');
    ss << ((fileInfo.st_mode & S_IRGRP) ? 'r' : '-');
    ss << ((fileInfo.st_mode & S_IWGRP) ? 'w' : '-');
    ss << ((fileInfo.st_mode & S_IXGRP) ? 'x' : '-');
    ss << ((fileInfo.st_mode & S_IROTH) ? 'r' : '-');
    ss << ((fileInfo.st_mode & S_IWOTH) ? 'w' : '-');
    ss << ((fileInfo.st_mode & S_IXOTH) ? 'x' : '-');
    return (ss.str());
}

void AutoIndex(DataPool &dataPool, std::string &dirPath)
{
    dirent *entry;
    std::string buffer;
    std::string cur_path;
    DIR *dir;
    struct stat fileInfo;

    std::string FileName = "/tmp/" + Lstring::RandomStr(16);

    dir = opendir(dirPath.c_str());
    if (dir == NULL)
        return;
    buffer = TemplateStart(dirPath);
    while ((entry = readdir(dir)) != NULL)
    {
        if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..")
            buffer.append(CreateRow(1, entry->d_name, 0, ""));
        else
        {
            cur_path = dirPath + entry->d_name;
            stat(cur_path.c_str(), &fileInfo);
            buffer.append(CreateRow(entry->d_type == DT_DIR ? 1 : 2,
                                    entry->d_name, (long long)fileInfo.st_size / (1024 * 1024),
                                    GetPermission(fileInfo)));
        }
    }
    buffer.append("</tbody></table></body></html>");
    dataPool.File.Fd = IO::OpenFile(FileName.c_str(), "w+");

    write(dataPool.File.Fd, buffer.c_str(), buffer.size());
    close(dataPool.File.Fd);
    dataPool.File.Fd = IO::OpenFile(FileName.c_str(), "r+");
    dataPool.File.ResourceFileType = "text/html";
    closedir(dir);
}
