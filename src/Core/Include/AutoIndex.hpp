#ifndef AUTOINDEX_HPP
#define AUTOINDEX_HPP
#include "Core.hpp"

std::string CreateRow(int type, std::string Name, u_int64_t size, std::string permissions);
std::string TemplateStart(const std::string &path);
std::string GetPermission(struct stat fileInfo);
void AutoIndex(DataPool &dataPool,std::string &dirPath);
#endif