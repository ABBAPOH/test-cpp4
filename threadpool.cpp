#include "threadpool.h"
#include <QException>
#include <QDebug>

class PooledThread : public QThread
{
public:
    explicit PooledThread(ThreadPool *parent = nullptr) :
        QThread(parent),
        pool(parent)
    {
        Q_ASSERT(pool);
        setObjectName("PooledThread");
    }

    void run() override { pool->run(); }

private:
    ThreadPool *pool;
};

ThreadPool::ThreadPool(QObject *parent) :
    QObject(parent)
{
    init();
}

ThreadPool::ThreadPool(int numthreads, QObject *parent) :
    QObject(parent)
{
    init(numthreads);
}

ThreadPool::~ThreadPool()
{
    QMutexLocker l(&mutex);
    quit = true;
    waitCondition.wakeAll();
    l.unlock();
    for (const auto &thread : threads) {
        thread->wait();
    }
}

void ThreadPool::start(const QString &id, const RunnablePointer &task)
{
    QMutexLocker l(&mutex);

    queue.append(id);
    tasks[id].append(task);
    if (waitCount) {
        waitCount--;
        waitCondition.wakeOne();
    }
}

void ThreadPool::init(int numthreads)
{
    if (numthreads <= 0)
        numthreads = QThread::idealThreadCount();

    for (int i = 0; i < numthreads; ++i) {
        threads.emplace_back(new PooledThread(this));
        threads.back()->start();
    }
}

void ThreadPool::run()
{
    while (true) {
        QMutexLocker l(&mutex);
        if (quit && queue.isEmpty())
            break;
        if (queue.isEmpty()) {
            waitCount++;
            waitCondition.wait(&mutex);
            continue;
        }
        const auto id = queue.takeFirst();
        auto task = tasks[id].takeFirst();
        l.unlock();

        task->run();
    }
}
