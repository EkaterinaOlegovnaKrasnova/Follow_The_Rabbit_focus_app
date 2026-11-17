#include "time_blocking.h"
#include "qpainter.h"
#include "ui_time_blocking.h"

time_blocking::time_blocking(QWidget *parent)
    : BaseTracker(parent,FTR::TimeBlocking)
    , ui(new Ui::time_blocking)
{
    ui->setupUi(this);
    this->setWindowTitle(windowTitle());
    placeClocksInLayout(analogClock,digitalClock,ui->verticalLayout, nullptr);
    ui->verticalLayout->insertStretch(0);
    add_mode_interface(ui->verticalLayout_2);
    if (ui->label_7){
    ui->label_7->setText(
        "<a href=\"#open\" style=\"color: white;\">Информация</a>");
    ui->label_7->setTextFormat(Qt::RichText);
    ui->label_7->setTextInteractionFlags(Qt::LinksAccessibleByMouse);}
    connectInfoLink();
    SPDLOG_TRACE("Окно метода TimeBlocking создано");
}

void time_blocking::on_label_4_linkActivated(const QString &link)
{
    if (link == "#guest") {
        SPDLOG_TRACE("Режим гостя");
        on_guest_link_activated(link);
        return;
    }
    if (link != "#open") {
        SPDLOG_ERROR("Режим юзера не подгружен");
        return;}
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
    profileWindow->show();
    profileWindow->raise();
    profileWindow->activateWindow();
    // остановка таймеров
    stopAllTimers();
    //остановка последовательности задач
    stopTaskSequence();          // для Flow/EatTheFrog
    //Запись незавершённой сессии в БД
    if (sessionStart.isValid())  // сессия реально была
        finishCurrentSession(false);
    this->hide();
    SPDLOG_TRACE("Пользовательская страница открыта");
}

time_blocking::~time_blocking()
{   SPDLOG_TRACE("Страница метода TimeBlocking удалена");
    delete ui;
}



