#include "reverse_pomodoro.h"
#include "qpainter.h"
#include "ui_reverse_pomodoro.h"

reverse_pomodoro::reverse_pomodoro(QWidget *parent)
    : BaseTracker(parent,FTR::ReversePomodoro)
    , ui(new Ui::reverse_pomodoro)
{
    ui->setupUi(this);
    this->setWindowTitle(windowTitle());
    placeClocksInLayout(analogClock,digitalClock,ui->verticalLayout_2, ui->buttonlayout);
    add_mode_interface(ui->pomodoro_layout);
    if (ui->label_7){
    ui->label_7->setText(
        "<a href=\"#open\" style=\"color: white;\">Информация</a>");
    ui->label_7->setTextFormat(Qt::RichText);
    ui->label_7->setTextInteractionFlags(Qt::LinksAccessibleByMouse);}
    connectInfoLink();
    hide_combo();
    SPDLOG_TRACE("Страница метода обратный помидор создана");
}

void reverse_pomodoro::on_label_4_linkActivated(const QString &link)
{
    if (link == "#guest") {
        on_guest_link_activated(link);
        return;
    }
    if (link != "#open") {
        SPDLOG_ERROR("Режим юзера не подгружен");
        return;
    }
    if (!profileWindow) {
        SPDLOG_INFO("Необходима загрузка пользовательской страницы");
        profileWindow = new profileW();
        profileWindow->setOpenWindow(this);
        profileWindow->setAttribute(Qt::WA_DeleteOnClose);
        connect(profileWindow, &profileW::profileUpdated,
                this, [this]() {
                    loadUserProfile();
                    loadCurrentUser();
                });
        connect(profileWindow, &QObject::destroyed,
                this, [this]() { profileWindow = nullptr; });
        SPDLOG_TRACE("Пользовательская страница создана");
    }
    profileWindow->show();
    profileWindow->raise();
    profileWindow->activateWindow();
    //остановка таймеров
    stopAllTimers();
    //остновка последовательности задач
    stopTaskSequence();          // для Flow/EatTheFrog
    //Запись незавершённой сессии в БД
    if (sessionStart.isValid())  // сессия реально была
        finishCurrentSession(false);
    this->hide();
    SPDLOG_TRACE("Пользовательская страница открыта");
}

reverse_pomodoro::~reverse_pomodoro()
{
    SPDLOG_TRACE("Страница методоа обратный помидор удалена");
    delete ui;
}



