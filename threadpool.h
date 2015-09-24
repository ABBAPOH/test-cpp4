#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "runnable.h"
#include <QThread>
#include <QVector>

class ThreadPool : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ThreadPool)
public:
    explicit ThreadPool(QObject *parent = nullptr);
    explicit ThreadPool(int numthreads, QObject *parent = nullptr);
    ~ThreadPool();

    void start(const QString &id, const RunnablePointer &task);

private:
    void init(int numthreads = -1);
    void run();

private:
    // Can't use QVector with unique_ptr's :(
    std::vector<std::unique_ptr<QThread>> threads;

    QMutex mutex;
    bool quit {false};
    QList<QString /*id*/> queue;
    QHash<QString /*id*/, QVector<RunnablePointer>> tasks;
    friend class PooledThread;
};

#endif // THREADPOOL_H
