#ifndef __EXAMPLE_STUB__H__
#define __EXAMPLE_STUB__H__

#include <memory>

#include "iexample.h"
#include "server.h"

class example_stub : public iexample::icallback
{
public:
    example_stub();

    ~example_stub();

    void on_info(const std::string &_info);

private:
    ipc::server ipc_;
    std::unique_ptr<iexample> example_;
};

#endif // __EXAMPLE_STUB__H__
