INCLUDEPATH += $$PWD/../spdlog-1.17.0/include
!exists($$PWD/../spdlog-1.17.0/include/spdlog/spdlog.h) {
    error("SPDLOG NOT FOUND! Ищу по пути: $$PWD/../spdlog-1.17.0/include")
}
DEFINES += SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE
QT += core gui sql network widgets
CONFIG += c++17 cmdline  resources_big

SOURCES += \
        analythic_page.cpp \
        basetracker.cpp \
        data_manager.cpp \
        digital_clock.cpp \
        eatthefrog.cpp \
        form52_17.cpp \
        form60_10.cpp \
        form90_20.cpp \
        information.cpp \
        main.cpp \
        mainwindow.cpp \
        myclock.cpp \
        new_user_form.cpp \
        open.cpp \
        password_utils.cpp \
        pomodoro_mode.cpp \
        profilew.cpp \
        reverse_pomodoro.cpp \
        time_blocking.cpp \
        timer.cpp \
        user_settings.cpp \
        users_form.cpp

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    analythic_page.ui \
    eatthefrog.ui \
    form52_17.ui \
    form60_10.ui \
    form90_20.ui \
    information.ui \
    mainwindow.ui \
    new_user_form.ui \
    open.ui \
    pomodoro_mode.ui \
    profilew.ui \
    reverse_pomodoro.ui \
    time_blocking.ui \
    timer.ui \
    user_settings.ui \
    users_form.ui

HEADERS += \
    analythic_page.h \
    basetracker.h \
    data_manager.h \
    digital_clock.h \
    eatthefrog.h \
    form52_17.h \
    form60_10.h \
    form90_20.h \
    information.h \
    mainwindow.h \
    myclock.h \
    new_user_form.h \
    open.h \
    password_utils.h \
    pomodoro_mode.h \
    profilew.h \
    reverse_pomodoro.h \
    time_blocking.h \
    timer.h \
    user_settings.h \
    users_form.h

RESOURCES += \
    res.qrc

DISTFILES += \
    img/clock (2).jpg
