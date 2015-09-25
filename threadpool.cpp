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
    QObject(parent),
    next_id(tasks.end())
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

    tasks[id].append(task);
    if (tasks[id].count() == 1)
        queue.push_back(id);
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
        const auto task = tasks[id].first();
        l.unlock();

        task->run();

        l.relock();
        tasks[id].takeFirst();
        if (tasks[id].count() > 0)
            queue.append(id);
    }
}
