#include "timer.h"
#include "qaction.h"
#include "ui_timer.h"
#include <QToolButton>
using namespace FTR;
Timer::Timer(QWidget *parent)
    : BaseTracker(parent,FlowMode)
    , ui(new Ui::Timer)
{
    ui->setupUi(this);
    this->setWindowTitle(windowTitleText());
    placeClocksInLayout(analogClock,digitalClock,ui->verticalLayout_2, ui->buttonlayout);
    add_mode_interface(ui->verticalLayout_3);
    if (ui->label_7){
    ui->label_7->setText(
        "<a href=\"#open\" style=\"color: white;\">Информация</a>");
    ui->label_7->setTextFormat(Qt::RichText);
    ui->label_7->setTextInteractionFlags(Qt::LinksAccessibleByMouse);}
    connectInfoLink();
    SPDLOG_TRACE("Страница метода свободный режим создана");
    }

void Timer::on_label_6_linkActivated(const QString &link)
    {if (link == "#guest") {
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
        // остановка последовательности задач
        stopTaskSequence();          // для Flow/EatTheFrog
        //запись незавершённой сессии в БД
        if (sessionStart.isValid())
            finishCurrentSession(false);
        this->hide();
        SPDLOG_TRACE("Пользовательская страница открыта");
    }

Timer::~Timer()
{   SPDLOG_TRACE("Страница метода свободный режим удалена");
    delete ui;
}

void Timer::on_timer_clicked()
{SPDLOG_TRACE("Для часов включен режим таймера");
    analogClock->setMode(timer);
}

