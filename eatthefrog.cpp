#include "eatthefrog.h"
#include "ui_eatthefrog.h"

EatTheFrog::EatTheFrog(QWidget *parent)
    : BaseTracker(parent,FTR::EatTheFrog)
    , ui(new Ui::EatTheFrog)
{
    ui->setupUi(this);
    setWindowTitle(windowTitle());
    placeClocksInLayout(analogClock,digitalClock,ui->verticalLayout, nullptr);
    placeClocksInLayout(nullptr,digitalClock2,ui->verticalLayout_2, ui->buttonlayout);
    add_mode_interface(ui->verticalLayout_2);
    if (ui->label_7){
    ui->label_7->setText(
        "<a href=\"#open\" style=\"color: white;\">Информация</a>");
    ui->label_7->setTextFormat(Qt::RichText);
    ui->label_7->setTextInteractionFlags(Qt::LinksAccessibleByMouse);}
    connectInfoLink();
    SPDLOG_TRACE("Страница метода съесть лягушку создана");
}

void EatTheFrog::on_label_4_linkActivated(const QString &link)
{
    if (link == "#guest") {
        on_guest_link_activated(link);
        SPDLOG_TRACE("Подключен режим гостя");
        return;
    }
    if (link != "#open"){
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
    //остановка таймеров
    stopAllTimers();
    //Остановка последовательности задач
    stopTaskSequence();          // для Flow/EatTheFrog
    //Запись незавершённой сессии в БД
    if (sessionStart.isValid())
        finishCurrentSession(false);
    this->hide();
    SPDLOG_TRACE("Пользовательская страница открыта");
}

EatTheFrog::~EatTheFrog()
{
    SPDLOG_TRACE("Страница метода съесть лягушку удалена");
    delete ui;
}














