#include "sizer.h"
#include <QApplication>
#include <QDesktopWidget>

Sizer::Sizer(QObject *p) : QObject(p), full(1) {}

int Sizer::width()
{
    if (full)
        return QApplication::desktop()->width();
    return 718;
}

int Sizer::height()
{
    if (full)
        return QApplication::desktop()->height();
    return 534;
}

void Sizer::emitResize()
{
    emit resize();
}
