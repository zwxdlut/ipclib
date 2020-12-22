#include <chrono>
#include <thread>

#include "myservice_factory.h"

#define TAG "myclient"

class myservice_callback: public imyservice::icallback
{
public:
    void on_info(const std::string &info)
    {
        LOGI("myservice_callback", "%s", info.c_str());
    }
};

int main(int argc, char** argv)
{
    int32_t i32 = 100;
    bool b = true;
    data_a a = {200, {"AAA", "BBB"}, true, false, {65, 650}};
    std::vector<data_a> va;
    double f64 = 100.1;
    uint8_t i8 = 0;
    auto service = myservice_factory::create(myservice_factory::interface_type::PROXY);
    myservice_callback callback;
    
    service->add_callback(&callback);

    LOGI(TAG, "func1 +: %d, %d, %s, %s", i32, b, formater::format(a)->c_str(), formater::format(va)->c_str());
    service->func1(i32, b, a, va);
    LOGI(TAG, "func1 -: %d, %d, %s, %s", i32, b, formater::format(a)->c_str(), formater::format(va)->c_str());

    LOGI(TAG, "func2 +: %lf", f64);
    service->func2(f64);
    LOGI(TAG, "func2 -: %lf", f64);

    LOGI(TAG, "func3 +: %d", i8);
    service->func3(i8);
    LOGI(TAG, "func3 -: %d", i8);

    LOGI(TAG, "func4 +");
    service->func4();
    LOGI(TAG, "func4 -");

    while(1)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}