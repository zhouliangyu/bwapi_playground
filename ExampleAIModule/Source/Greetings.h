#pragma once
#include <iostream>
#include <string>

using namespace BWAPI;
using namespace Filter;

class Greetings
{
private:
    std::string m_greetingWord;
public:
    greeting(const std::string& s) : m_greetingWorld {s}
    {
    }
    void printGreeting() { Broodwar->sendText(m_greetingWord); }
};
