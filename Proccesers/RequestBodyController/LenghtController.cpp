#include "Include/LenghtController.hpp"

LenghtController::LenghtController(DataPool &dataPool) : BodyController(dataPool)
{
}

int LenghtController::Receiver(std::string &data)
{
    if (Remaining != 0)
    {
        size_t PartSize = data.size() > (size_t)Remaining ? Remaining : data.size();
        DEBUGOUT(0, COLORED(data.substr(0, PartSize), Red));
        this->dataPool.body.append(data.substr(0, PartSize));
        Remaining -= PartSize;
        data = data.substr(PartSize);
    }
    if (Remaining == 0)
        return (true);
    return (false);
}


void LenghtController::SetRemaining(unsigned long Remaining)
{
    DEBUGOUT(1, "HERE Lenght");

    /**
     *  TODO: if => RequestParser body larger than client max body size in config file
     *        change <2024> with max-body from config file */
    if (Remaining > 5 * 1024 * KB)
        throw std::runtime_error("413");
    this->Remaining = Remaining;
}

LenghtController::~LenghtController()
{
}