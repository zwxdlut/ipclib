#include <chrono>

#include "example.h"
#include "example_stub.h"

int main(int argc, char** argv)
{
    example_stub stub;
    
    while(1)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}