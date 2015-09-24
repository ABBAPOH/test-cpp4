#ifndef RUNNABLE_H
#define RUNNABLE_H

#include <QSharedPointer>

class Runnable
{
    Q_DISABLE_COPY(Runnable)
public:
    Runnable() = default;
    virtual ~Runnable();

    virtual void run() = 0;
};

using RunnablePointer = QSharedPointer<Runnable>;

#endif // RUNNABLE_H
