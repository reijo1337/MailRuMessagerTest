CONFIG += c++14

TARGET = boost_as

SOURCES += main.cpp \
    server.cpp \
    tcp_connection.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
LIBS += \
       -lboost_system\
       -lboost_chrono \
       -lboost_thread\
       -lboost_timer\
       -lboost_coroutine \
       -lboost_filesystem

HEADERS += \
    server.h \
    tcp_connection.h
