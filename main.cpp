#include <QApplication>
#include "mainwindow.h"
#include "open.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    SPDLOG_TRACE("Открытие приложения");
    try{
        auto console_s = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto file_s = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/app.log",
        1024*1024*5,5);
        std::vector<spdlog::sink_ptr> sinks = {console_s,file_s};
        auto logger =  std::make_shared<spdlog::logger>("app_logger",
                                                       sinks.begin(), sinks.end());
        logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
        logger->set_level(spdlog::level::trace);
        logger->flush_on(spdlog::level::warn);
        spdlog::set_default_logger(logger);
        SPDLOG_INFO("Логирование успешно инициализировано");
    }
    catch(const spdlog::spdlog_ex& e){
         qCritical() << "Ошибка инициализации логгера:" << e.what();
    }
    QIcon icon(":/img/icon.ico");
    QApplication::setWindowIcon(icon);
    SPDLOG_TRACE("Загружена иконка приложения");
    class open* openWindow  = new class open();
    MainWindow *main_window = new MainWindow();

    QObject::connect(openWindow, &open::openproject, main_window, &MainWindow::show);
    main_window->setOpenWindow(openWindow);
    openWindow->show();
    qApp->setStyleSheet(
        "QMessageBox { background-color: #2b2b2b; }"
        "QMessageBox QLabel { color: white; }"
        "QMessageBox QPushButton { "
        "  background: rgba(0, 0, 0, 0.6); color: white;font-weight: bold; "
        "  border: 2px solid rgba(255, 255, 255, 0.3); "
        "  border-radius: 10px; padding: 5px 15px; "
        "}"
        );
    QObject::connect(&a, &QCoreApplication::aboutToQuit, []{
        Data_Manager::instance().close();
    });
    QApplication::beep();
    SPDLOG_TRACE("beep #1");
    QTimer::singleShot(1200, qApp, [](){
        SPDLOG_TRACE("beep #2");
        QApplication::beep();
    });



    return a.exec();
}
