#include "threadpool.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QTimer>

class TestTask : public Runnable
{
public:
    TestTask(const QString &text) : _text(text) {}
    void run() override
    {
        QEventLoop loop;
        QTimer::singleShot(qrand() % 100, &loop, &QEventLoop::quit);
        loop.exec();
        qDebug() << "Hello from" << _text << "thread" << QThread::currentThreadId();
    }

private:
    QString _text;
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    ThreadPool pool;

    for (int j = 0; j < 10; ++j) { // tasks
        for (int i = 0; i < 20; ++i) { // clients
            QString id = QStringLiteral("%1").arg(i);
            QString text = QStringLiteral("Client %1, task %2").arg(i).arg(j);
            pool.start(id, RunnablePointer(new TestTask(text)));
        }
    }

    return 0;
}
