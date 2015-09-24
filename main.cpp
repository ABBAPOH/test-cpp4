#include "threadpool.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QTimer>

class TestTask : public Runnable
{
public:
    TestTask(const QString &id) : _id(id) {}
    void run() override
    {
        QEventLoop loop;
        QTimer::singleShot(qrand() % 100, &loop, &QEventLoop::quit);
        loop.exec();
        qDebug() << "Hello from client" << _id << "thread" << QThread::currentThreadId();
    }

private:
    QString _id;
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    ThreadPool pool;

    for (int j = 0; j < 10; ++j) { // tasks

        for (int i = 0; i < 20; ++i) { // clients
            QString id = QStringLiteral("Client %1").arg(i);
            pool.start(id, RunnablePointer(new TestTask(id)));
        }
    }

    return 0;
}
