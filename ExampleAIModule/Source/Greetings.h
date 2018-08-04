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
    Greetings(const std::string& s) : m_greetingWord {s}
    {
    }
    void printGreeting() { Broodwar->sendText(m_greetingWord.c_str()); }
};
