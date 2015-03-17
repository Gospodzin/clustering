#pragma once
#include "logging.h"
#include <QThread>

class ComputationThread : public QThread
{

public:
    ComputationThread() {}

private:

    void run()
    {
        while(1) {sleep(1);
        LOG("adad")}
    }
};
