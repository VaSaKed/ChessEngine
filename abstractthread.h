#ifndef ABSTRACTTHREAD_H
#define ABSTRACTTHREAD_H

#include <thread>
#include <mutex>
#include <condition_variable>

class AbstractThread {

protected:
    // override this function to set thread's payload
    virtual void run() =0;

public:

    AbstractThread();

    void start();

    bool isRunning();

    bool isFinished();

    void waitForFinish();

    ~AbstractThread();

private:
    // thread's private entry point
    void abstract_thread_main();

protected:
    std::thread thread;
    std::mutex mutex;
    std::condition_variable cv;

    volatile bool started;
    volatile bool terminating;
};

#endif // ABSTRACTTHREAD_H
