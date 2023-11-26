#ifndef CHUNKCONTROLLER_HPP
#define CHUNKCONTROLLER_HPP
#include "BodyController.hpp"

enum
{
    Size,
    Data,
    End,
    Trailer,
    Error
};

class ChunkController : public BodyController
{
private:
    int ChunkState;

public:
    ChunkController(DataPool &dataPool);
    int Receiver(std::string &data);
    ~ChunkController();
};

#endif