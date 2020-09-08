#include <chrono>

#include "myservice.h"
#include "myservice_stub.h"

int main(int argc, char** argv)
{
    myservice_stub stub;
    
    while(1)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}