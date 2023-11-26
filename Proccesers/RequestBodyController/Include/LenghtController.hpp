#ifndef LENGHTCONTROLLER_HPP
#define LENGHTCONTROLLER_HPP
#include "BodyController.hpp"

class LenghtController : public BodyController
{

public:
    LenghtController(DataPool &dataPool);
    LenghtController(DataPool &dataPool, u_int64_t Remaining);
    int Receiver(std::string &data);
    ~LenghtController();
};

#endif