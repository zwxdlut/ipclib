#ifndef __MYSERVICE_STUB__H__
#define __MYSERVICE_STUB__H__

#include <memory>

#include "i_myservice.h"
#include "server.h"

class myservice_stub : public i_myservice::i_callback
{
public:
    myservice_stub();

    ~myservice_stub();

    void on_info(const std::string &_info);

private:
    ipc::server ipc_;
    std::unique_ptr<i_myservice> service_;
};

#endif // __MYSERVICE_STUB__H__
