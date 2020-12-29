#include <chrono>
#include <thread>

#include "example_factory.h"

#define TAG "example_client"

class example_callback: public iexample::icallback
{
public:
    void on_info(const std::string &info)
    {
        LOGI("example_callback", "%s", info.c_str());
    }
};

int main(int argc, char** argv)
{
    uint8_t i8 = 0;
    int32_t i32 = 100;
    double f64 = 100.1;
    bool b = true;
    data_a a = {200, {"AAA", "BBB"}, true, false, {65, 650}};
    std::vector<data_a> va;
    auto example = example_factory::create(example_factory::interface_type::PROXY);
    example_callback callback;
    
    example->add_callback(&callback);

    LOGI(TAG, "func1 +: %d, %d, %s, %s", i32, b, formater::format(a)->c_str(), formater::format(va)->c_str());
    example->func1(i32, b, a, va);
    LOGI(TAG, "func1 -: %d, %d, %s, %s", i32, b, formater::format(a)->c_str(), formater::format(va)->c_str());

    LOGI(TAG, "func2 +: %lf", f64);
    example->func2(f64);
    LOGI(TAG, "func2 -: %lf", f64);

    LOGI(TAG, "func3 +: %d", i8);
    example->func3(i8);
    LOGI(TAG, "func3 -: %d", i8);

    LOGI(TAG, "func4 +");
    example->func4();
    LOGI(TAG, "func4 -");

    while(1)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}