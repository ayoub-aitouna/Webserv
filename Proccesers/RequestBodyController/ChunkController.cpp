#include "Include/ChunkController.hpp"

ChunkController::ChunkController(DataPool &dataPool) : BodyController(dataPool)
{
    this->ChunkState = Size;
    this->Remaining = 0;
}

int ChunkController::Receiver(std::string &data)
{

    size_t index;
    size_t Part;
    std::string RawData;

    while (!data.empty())
    {
        switch (this->ChunkState)
        {
        case Size:
            if ((index = data.find("\r\n")) == std::string::npos)
            {
                if (data.size() < 7)
                    return false;
                this->ChunkState = Error;
                break;
            }
            Remaining = strtol(data.c_str(), 0, 16);
            this->ChunkState = Remaining != 0 ? Data : End;
            data = data.erase(0, index + 2);
            break;
        case Data:
            if (Remaining != 0)
            {
                Part = ((size_t)Remaining) > data.size() ? data.size() : Remaining;
                RawData = data.substr(0, Part);
                dataPool.body.append(RawData);
                Remaining -= Part;
                data = data.erase(0, Part);
            }
            if (data.size() < 2)
                return false;
            if (Remaining == 0)
            {
                if ((data.size() > 2) && (data.at(0) != '\r' || data.at(1) != '\n'))
                    this->ChunkState = Error;
                else
                {
                    data = data.erase(0, 2);
                    this->ChunkState = Size;
                }
            }
            break;
        case End:
            return (true);
            break;

        default:
            this->ChunkState = Error;
            break;
        }
        if (this->ChunkState == Error)
        {
            DEBUGOUT(1, COLORED("ERROR !!!", Red));
            exit(0);
        }
        /**
         *  TODO: if => RequestParser body larger than client max body size in config file
         *        change <1024> with max-body from config file */
        // if (dataPool.body.size() > 90 * 1024 * 1024)
        //     throw HTTPError(413);
    }

    return (false);
}

void ChunkController::SetRemaining(unsigned long Remaining)
{
    (void)Remaining;
}

ChunkController::~ChunkController()
{
}