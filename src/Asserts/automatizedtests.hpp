#pragma once
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

class asserts
{
    void check(bool condition, string& message)
    {
        if(!condition)
        {
            cerr << "Assertion failed: " << message << endl;
            abort();
        }
    }

    void checkresult()
}