#include "abstractthread.h"
#include <QDebug>

AbstractThread::AbstractThread()
    : thread(), started(false), terminating(false)
{
    thread = std::move(std::thread(&AbstractThread::abstract_thread_main, std::ref(*this)));
}

void AbstractThread::start() {
    std::unique_lock<std::mutex> lck(mutex);
    started = true;
    lck.unlock();

    cv.notify_one(); // notifies abstract_thread_main(), state change IDLE -> RUNNING
}

bool AbstractThread::isRunning() {
    return started;
}

bool AbstractThread::isFinished() {
    return !started;
}

void AbstractThread::waitForFinish()
{
    int count = 0;
    while (isRunning()) {
        if (count++)
            qDebug() << "waitFinish(): wake-up #" << count;
        std::unique_lock<std::mutex> lck(mutex);
        lck.unlock();
    }
}

AbstractThread::~AbstractThread() {
    std::unique_lock<std::mutex> lck(mutex);
    terminating = true;
    lck.unlock();
    cv.notify_one();
    if (thread.joinable())
        thread.join();
}

void AbstractThread::abstract_thread_main() {
    // idle
    while (true) {
        std::unique_lock<std::mutex> lck(mutex);

        // wait for the start() call or dtor
        cv.wait(lck, [this](){return started || terminating;}); // state chane IDLE -> RUNNING

        // terminate the thread
        if (terminating)
            return;

        // start our work
        run();

        started = false;    // state change RUNNING -> IDLE
    }
}
