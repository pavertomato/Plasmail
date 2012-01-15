#ifndef SIZER_H
#define SIZER_H

#include <QObject>

class Sizer : public QObject
{
    Q_OBJECT

public:
    bool full;

    Sizer(QObject* p=NULL);
    Q_INVOKABLE int width();
    Q_INVOKABLE int height();
    void emitResize();
signals:
    void resize();
};

#endif // SIZER_H
