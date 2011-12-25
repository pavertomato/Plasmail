SOURCES += main.cpp \
    receiver/receiver.cpp \
    receiver/base64.cpp \
    receiver/u82w1251.cpp \
    receiver/quoted.cpp
HEADERS += \
    receiver/receiver.h \
    receiver/base64.h \
    receiver/info.h \
    receiver/message.h \
    receiver/u82w1251.hpp \
    receiver/quoted.h
OTHER_FILES += qml/main.qml \
    qml/settings.qml \
    qml/mailList.qml \
    qml/qmldir
QT += declarative \
    network


