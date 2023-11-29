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
            if ((index = data.find(CRLF)) == std::string::npos)
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
                SaveBodyAsFile();
                Remaining -= Part;
                data = data.erase(0, Part);
            }
            if (data.size() < 2)
                return false;
            if (Remaining == 0)
            {
                if ((data.size() > 2) && (data.substr(0, 2) != CRLF))
                    this->ChunkState = Error;
                else
                {
                    data = data.erase(0, 2);
                    this->ChunkState = Size;
                }
            }
            break;
        case End:;
            return (close(this->FileFd), true);

        default:
            this->ChunkState = Error;
            break;
        }
        if (this->ChunkState == Error)
            throw std::runtime_error("ERROR - While Recieving Data As Chunks");
    }

    return (false);
}

ChunkController::~ChunkController()
{
}