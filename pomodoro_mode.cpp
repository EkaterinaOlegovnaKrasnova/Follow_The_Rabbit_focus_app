#include "pomodoro_mode.h"
#include "ui_pomodoro_mode.h"

Pomodoro_mode::Pomodoro_mode(QWidget *parent)
    : BaseTracker(parent,FTR::Pomodoro)
    , ui(new Ui::Pomodoro_mode)
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
    SPDLOG_TRACE("Страница метода помидора создана");
}

void Pomodoro_mode::on_label_4_linkActivated(const QString &link)
{
    if (link == "#guest") {
        on_guest_link_activated(link);
        SPDLOG_TRACE("Подключен режим гостя");
        return;
    }
    if (link != "#open"){
        SPDLOG_ERROR("Режим юзера не подгружен");
        return;
    }
    if (!profileWindow) {
        SPDLOG_INFO("Необходима загрузка пользовательской страницы");
        profileWindow = new profileW();
        profileWindow->setOpenWindow(this);
        profileWindow->setAttribute(Qt::WA_DeleteOnClose);
        if (digitalClock) digitalClock->setEditable(false);
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
    //Остановка таймеров
    stopAllTimers();
    // Остановка последовательности задач
    stopTaskSequence();          // для Flow/EatTheFrog
    //Запись незавершённой сессии в БД
    if (sessionStart.isValid())
        finishCurrentSession(false);
    this->hide();
    SPDLOG_TRACE("Пользовательская страница открыта");
}

Pomodoro_mode::~Pomodoro_mode()
{
    SPDLOG_TRACE("Страница метода помидора удалена");
    delete ui;
}
