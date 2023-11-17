#ifndef LENGHTCONTROLLER_HPP
#define LENGHTCONTROLLER_HPP
#include "../BodyController/BodyController.hpp"

class LenghtController : public BodyController
{
private:
    /* data */
public:
    LenghtController(DataPool &dataPool);
    int Receiver(std::string &data);
    void SetRemaining(unsigned long Remaining);
    ~LenghtController();
};

#endif