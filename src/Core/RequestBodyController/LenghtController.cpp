#include "Include/LenghtController.hpp"

LenghtController::LenghtController(DataPool &dataPool) : BodyController(dataPool)
{
}

LenghtController::LenghtController(DataPool &dataPool, u_int64_t Remaining) : BodyController(dataPool)
{
    this->Remaining = Remaining;
    if (Remaining > ConfigHandler::GetHttp().GetMaxBodySize())
        throw HTTPError(413);
}

int LenghtController::Receiver(std::string &data)
{
    if (Remaining != 0)
    {
        size_t PartSize = data.size() > (size_t)Remaining ? Remaining : data.size();
        this->dataPool.body.append(data.substr(0, PartSize));
        SaveBodyAsFile();
        Remaining -= PartSize;
        data = data.substr(PartSize);
    }
    if (Remaining == 0)
        return (close(this->FileFd), true);
    return (false);
}

LenghtController::~LenghtController()
{
}