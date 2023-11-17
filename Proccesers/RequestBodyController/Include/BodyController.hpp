#ifndef BODYCONTROLLER_HPP
#define BODYCONTROLLER_HPP

#include "../../Include/Proccesers.hpp"
#include "../../../HTTPError/HTTPError.hpp"

class BodyController
{
public:
    BodyController(DataPool &dataPool);
    virtual int Receiver(std::string &data) = 0;
    virtual void SetRemaining(unsigned long Remaining) = 0;
    void Parser();
    WBSRVFILE SaveMultiPartFile(std::string &part);
    WBSRVFILE SaveBodyAsFile();
    ~BodyController();

protected:
    unsigned long Remaining;
    int Encoding;
    DataPool &dataPool;
};

#endif