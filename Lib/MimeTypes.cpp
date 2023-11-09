#include "MimeTypes.hpp"

std::map<std::string, std::string> MimeTypes::GetContenTypes(void)
{
    std::map<std::string, std::string> types;
    // text
    types[".text"] = "text/plain";
    types[".css"] = "text/css";
    types[".csv"] = "text/csv";
    types[".htm"] = "text/html";
    types[".html"] = "text/html";

    // images
    types[".gif"] = "image/gif";
    types[".ico"] = "image/x-icon";
    types[".jpeg"] = "image/jpeg";
    types[".jpg"] = "image/jpeg";
    types[".png"] = "image/png";
    types[".svg"] = "image/svg+xml";

    // Application
    types[".js"] = "application/javascript";
    types[".json"] = "application/json";
    types[".pdf"] = "application/pdf";

    return (types);
}