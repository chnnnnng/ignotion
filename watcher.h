#ifndef WATCHER_H
#define WATCHER_H

#include <QThread>
#include <QDebug>
#include <directory.h>
#include <page.h>

class Watcher : public QThread
{
    Q_OBJECT
public:
    explicit Watcher(QObject *parent = nullptr);
    void run() override;
signals:

};

#endif // WATCHER_H
