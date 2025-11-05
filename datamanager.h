#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QCoreApplication>
#include <QQueue>
#include <QTimer>
#include <QDebug>

struct DataUnit {
    double x;
    double y;
    double c;
    int flag;  // 0: 合格, 1: 不合格
};
class DataManager : public QObject
{
    Q_OBJECT
public:
    explicit DataManager(QObject *parent = nullptr);
    void addDataUnit(double x, double y,double c, int flag) {
            DataUnit unit = {x, y, c,flag};
            queue.enqueue(unit);
            qDebug() << "Added DataUnit:" << "X:" << x << ",Y:" << y <<",c:"<<c<< ",Flag:" << flag;
        }
signals:
public slots:

    void processQueue() {
            if (!queue.isEmpty()) {
                DataUnit unit = queue.dequeue();
                qDebug() << "Processing DataUnit:" << "X:" << unit.x << "Y:" << unit.y <<"c:"<<unit.c<< "Flag:" << unit.flag;
            } else {
                qDebug() << "Queue is empty.";
            }
        }
private:
    QQueue<DataUnit> queue;
        QTimer* timer;
};

#endif // DATAMANAGER_H
