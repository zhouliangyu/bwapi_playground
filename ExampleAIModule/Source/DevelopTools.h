#pragma once
#include <BWAPI.h>


class DevelopTools
{
    private:
        static const bool IS_DEVELOPING;
        static const int LOG_MESSEGE_INTERVAL;
        static int messegeLastLogged;
    public:
        static void logMessegeOnScreen(const char* s, int i=0);
};


