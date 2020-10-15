#include "Timer.h"
#include <iostream>
#include <chrono>


int main()
{
    std::unique_ptr<Timer> chronoTimer = MakeTimer(std::chrono::steady_clock::now, std::chrono::seconds(3), []{ std::cout << "Hello world!" << std::endl; });
    chronoTimer->Start();

    while (true)
        chronoTimer->Update();

    return 0;
}
