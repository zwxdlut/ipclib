#ifndef __MYSERVICE_STUB__H__
#define __MYSERVICE_STUB__H__

#include <memory>

#include "imyservice.h"
#include "server.h"

class myservice_stub : public imyservice::icallback
{
public:
    myservice_stub();

    ~myservice_stub();

    void on_info(const std::string &_info);

private:
    ipc::server ipc_;
    std::unique_ptr<imyservice> service_;
};

#endif // __MYSERVICE_STUB__H__
