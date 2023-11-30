#include "Headers/EventClass.hpp"

EventClass::EventClass()
{
}

void EventClass::SetRawData(std::string &RawData)
{
     this->RawData = RawData;
}

void EventClass::Parse()
{
     std::stringstream ss(this->RawData);
     std::string Line;
     std::vector<std::string> tokens;

     while (getline(ss, Line))
     {
          tokens = Lstring::SplitByOneOf(std::string(Line), " \t");
          if (tokens.size() == 0 || tokens.at(0) == "}" || tokens.at(0) == "events")
               continue;
          if (tokens.at(0) == "worker_connections")
          {
               ExactSize(tokens.size() != 2, "events");
               this->worker_connections = atoi(tokens.at(1).c_str());
          }
          else
               throw std::runtime_error("Invalide token " + tokens.at(0));
     }
     DisplayValues(false);
}

void EventClass::DisplayValues(bool show)
{
     DEBUGOUT(show, COLORED("\n\n------- EventClass -------\n\n", Magenta));
     DEBUGOUT(show, COLORED("worker_connections : ", Magenta) << COLORED(this->worker_connections, Green));
     DEBUGOUT(show, COLORED("\n\n------- END EventClass -------\n\n", Magenta));
}

EventClass::~EventClass()
{
}
