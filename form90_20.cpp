#include "form90_20.h"
#include "myclock.h"
#include "qpainter.h"
#include "ui_form90_20.h"

Form90_20::Form90_20(QWidget *parent)
    : BaseTracker(parent,FTR::Mode90_20)
    , ui(new Ui::Form90_20)
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
    SPDLOG_TRACE("Страница метода 90/20 создана");
}

void Form90_20::on_label_4_linkActivated(const QString &link)
{if (link == "#guest") {
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
        connect(profileWindow, &profileW::profileUpdated,
                this, [this]() {
                    loadUserProfile();
                    loadCurrentUser();
                });
        connect(profileWindow, &QObject::destroyed,
                this, [this]() { profileWindow = nullptr; });
        if (digitalClock) digitalClock->setEditable(false);
        SPDLOG_TRACE("Пользовательская страница создана");
    }
    profileWindow->show();
    profileWindow->raise();
    profileWindow->activateWindow();
    // Остановка таймеров
    stopAllTimers();
    // Остановка последовательности задач
    stopTaskSequence();          // для Flow/EatTheFrog
    // Запись незавершённой сессии в БД
    if (sessionStart.isValid())
        finishCurrentSession(false);
    this->hide();
    SPDLOG_TRACE("Пользовательская страница открыта");
}

Form90_20::~Form90_20()
{
    SPDLOG_TRACE("Страница метода 90/20 удалена");
    delete ui;
}

