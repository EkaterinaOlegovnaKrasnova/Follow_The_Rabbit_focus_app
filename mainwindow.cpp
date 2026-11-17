#include "mainwindow.h"
#include "eatthefrog.h"
#include "form52_17.h"
#include "form60_10.h"
#include "form90_20.h"
#include "information.h"
#include "new_user_form.h"
#include "pomodoro_mode.h"
#include "profilew.h"
#include "qdialogbuttonbox.h"
#include "qmessagebox.h"
#include "reverse_pomodoro.h"
#include "time_blocking.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QResizeEvent>
#include <QPainter>
#include "timer.h"
#include "data_manager.h"
#include "mainwindow.h"
#include "users_form.h"
#include <spdlog/spdlog.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // аналоговые часы
    analogClock = new MyClock(this);
    analogClock->setStyleSheet("background: transparent;");
    analogClock->setAttribute(Qt::WA_TranslucentBackground);
    digitalClock = new digital_clock(this);
    // электронные часы
    digitalClock->setColor(Qt::white);
    digitalClock->setShowSeconds(true);
    digitalClock->set24HourFormat(true);
    digitalClock->setFixedHeight(100);
    digitalClock->setFixedWidth(300);
    digitalClock->setStyleSheet("background: transparent;");
    // кнопки режимов
    QPushButton *timerLeft1 = new QPushButton("Pomodoro", this);
    QPushButton *timerLeft2 = new QPushButton("ReversePomodoro", this);
    QPushButton *timerLeft3 = new QPushButton("FlowMode", this);
    QPushButton *timerLeft4 = new QPushButton("TimeBlocking", this);
    QPushButton *timerRight1 = new QPushButton("60/10", this);
    QPushButton *timerRight2 = new QPushButton("52/17", this);
    QPushButton *timerRight3 = new QPushButton("90/20", this);
    QPushButton *back = new QPushButton("НАЗАД", this);
    QPushButton *timeblocking = new QPushButton("EatTheFrog", this);
    // Общий стиль для всех кнопок таймеров
    QString timerButtonStyle =
        "QPushButton {"
        "   background: rgba(0, 0, 0, 0.6);"
        "   color: white;"
        "   border: 2px solid rgba(255, 255, 255, 0.3);"
        "   border-radius: 10px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   padding: 10px;"
        "}"
        "QPushButton:hover {"
        "   background: rgba(100, 100, 100, 0.7);"
        "   border-color: rgba(255, 255, 255, 0.6);"
        "}"
        "QPushButton:pressed {"
        "   background: rgba(50, 50, 50, 0.8);"
        "}";
    // применение стиля
    timerLeft4->setStyleSheet(timerButtonStyle);
    timerLeft1->setStyleSheet(timerButtonStyle);
    timerLeft2->setStyleSheet(timerButtonStyle);
    timerLeft3->setStyleSheet(timerButtonStyle);
    timerRight1->setStyleSheet(timerButtonStyle);
    timerRight2->setStyleSheet(timerButtonStyle);
    timerRight3->setStyleSheet(timerButtonStyle);
    back->setStyleSheet(timerButtonStyle);
    timeblocking->setStyleSheet(timerButtonStyle);
    // Установка ширины
    int buttonWidth = 150;
    timerLeft4->setFixedWidth(buttonWidth);
    timerLeft1->setFixedWidth(buttonWidth);
    timerLeft2->setFixedWidth(buttonWidth);
    timerLeft3->setFixedWidth(buttonWidth);
    timerRight1->setFixedWidth(buttonWidth);
    timerRight2->setFixedWidth(buttonWidth);
    timerRight3->setFixedWidth(buttonWidth);
    timeblocking->setFixedWidth(buttonWidth);
    back->setFixedWidth(buttonWidth);
    QLabel *label=new QLabel("Выберите режим\nтрекера:");
    QString label_settings =
    "QLabel {"
    "   color: white;"
    "   font-size: 28px;"
    "   font-weight: bold;"
    "}";
    label->setStyleSheet(label_settings);
    label->setFixedWidth(300);
    label->setAlignment(Qt::AlignLeft);
    ui->verticalLayout_2->insertWidget(0,label,0);
    ui->verticalLayout_2->insertStretch(0);
    ui->verticalLayout_2->insertStretch(2);
    ui->formLayout->addRow(timerLeft4, timerRight1);
    ui->formLayout->addRow(timeblocking, timerRight2);
    ui->formLayout->addRow(timerLeft1, timerRight3);
    ui->formLayout->addRow(timerLeft2,timerLeft3);
    ui->verticalLayout_2->addStretch();
    ui->verticalLayout_2->addStretch();
    ui->verticalLayout_2->addStretch();
    ui->verticalLayout_2->addStretch();
    ui->verticalLayout_2->addStretch();
    ui->verticalLayout_2->addStretch();
    ui->verticalLayout_2->addWidget(back,0,Qt::AlignLeft);
    ui->verticalLayout_3->addWidget(analogClock, 0,Qt::AlignHCenter);
    ui->verticalLayout_3->addWidget(digitalClock, 0,Qt::AlignHCenter);
    ui->verticalLayout_3->addStretch();
    resize(1100, 700);
    analogClock->setMinimumSize(300, 300);
    ui->label_3->setTextFormat(Qt::RichText);
    ui->label_3->setStyleSheet(
        "QLabel {"
        "   background: rgba(0, 0, 0, 0.5);"
        "   color: white;"
        "   font-size: 12px;"
        "   border: 2px solid rgba(255, 255, 255, 0.4);"
        "   border-radius: 10px;"
        "   padding: 6px 12px;"
        "   outline: none;"
        "}"
        );
    ui->label_3->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
    connect(timerLeft4, &QPushButton::clicked, this, &MainWindow::time_blocking_clicked);
    connect(timerLeft1, &QPushButton::clicked, this, &MainWindow::pomodoro_clicked);
    connect(timerLeft2, &QPushButton::clicked, this, &MainWindow::pomodoro_reverse_clicked);
    connect(timerLeft3, &QPushButton::clicked, this, &MainWindow::flow_mode_clicked);
    connect(timerRight1, &QPushButton::clicked, this, &MainWindow::mode_60_10_clicked);
    connect(timerRight2, &QPushButton::clicked, this, &MainWindow::mode_52_17_clicked);
    connect(timerRight3, &QPushButton::clicked, this, &MainWindow::mode_90_20_clicked);
    connect(timeblocking, &QPushButton::clicked, this, &MainWindow::eatthefrog_clicked);
    connect(back,&QPushButton::clicked,this,&MainWindow::getback_clicked) ;
    this->setWindowTitle("FOLLOW THE RABBIT");loadCurrentUser();
    loadUserProfile();
    ui->label_4->setTextFormat(Qt::RichText);
    ui->label_4->setText(
        QString("<a href=\"#info\" style=\"color: white; text-decoration: none;\">"
                "Информация</a>"));
    ui->label_4->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->label_4->setOpenExternalLinks(false);
    connect(ui->label_4, &QLabel::linkActivated,
            this, &MainWindow::on_label_4_linkActivated);
    SPDLOG_DEBUG("MainWindow:Главная страница создана");
}
MainWindow::~MainWindow()
{
    SPDLOG_DEBUG("Главная страница удалена");
    delete ui;
}

void MainWindow::loadCurrentUser()
{
    Data_Manager& dm = Data_Manager::instance();
    User_Profile me = dm.getCurrentUserProfile();
//гость
    if (me.user_id < 0) {
        ui->label_3->setText(
            QString("<a href=\"#guest\" style=\""
                    "color: white; font-weight: bold;\">Гость</a>"));
        SPDLOG_DEBUG("MainWindow:: loadCurrentUser: гостевой режим");
        return;
    }
//юзер
    ui->label_3->setText(
        QString("<a href=\"#open\" style=\"color: white; font-weight: bold;\">"
                "%1 %2</a>")
            .arg(me.Name, me.Surname)
        );
    SPDLOG_DEBUG("MainWindow:: loadCurrentUser: me.user_id ={},me.Name ={} ",me.user_id,me.Name.toStdString());
}

//настройка профиля и загрузка аватарки
void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    loadCurrentUser();
    loadUserProfile();
}

//фон
void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QPixmap bg(":/img/background.jpeg");
    if (bg.isNull()) return;
    QSize s = bg.size().scaled(size(), Qt::KeepAspectRatioByExpanding);
    QPixmap scaled = bg.scaled(s, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    p.drawPixmap((width()  - scaled.width())  / 2,
                 (height() - scaled.height()) / 2, scaled);
}


//перерисовка изображения
void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    update();
}


//настройка модулей
void MainWindow::eatthefrog_clicked(){

    EatTheFrog* mode = new EatTheFrog;
    mode->setOpenWindow(this);
    mode->setWindowTitle("Съесть лягушку");
    mode->set_info_text(
"EAT THE FROG - Начни с самой сложной задачи\n"
"**********\n"
"Суть техники\n\n"
"Метод Брайана Трейси: «съешь лягушку» — сделай\n"
"самое неприятное и сложное дело первым. Тогда\n"
"остаток дня пройдёт легче, а чувство выполненного\n"
"долга зарядит энергией.\n"
"**********\n"
"Как работает в приложении\n\n"
"— Выберите одну «лягушку» — самую тяжёлую задачу.\n"
"— Запустите таймер и работайте над ней,\n"
"не отвлекаясь.\n"
"— Отдых — только после завершения блока.\n"
"**********\n"
"Кому подходит\n\n"
"— Тем, кто часто откладывает важные дела.\n"
"— Тем, у кого много «нелюбимых» задач.\n"
"— Для утренней продуктивности.\n"
"**********\n"
"Совет\n\n"
"«Лягушку» лучше съедать в первые 1–2 часа\n"
"после пробуждения, пока мозг свеж. Не открывайте\n"
"почту и соцсети до её завершения.\n");
    mode->show();
     this->hide();
}
void MainWindow::pomodoro_clicked()
{

    Pomodoro_mode *mode = new Pomodoro_mode;
    mode->setOpenWindow(this);
    mode->setWindowTitle("Помидор");
    mode->show();
    mode->set_info_text(
"POMODORO - 25 минут работы / 5 минут отдыха\n"
"**********\n"
"Суть техники\n\n"
"Классический метод Франческо Чирилло:\n"
"работа делится на\n"
"«помидоры» — интервалы по 25 минут, между которыми делается\n"
"короткий перерыв 5 минут. После четырёх помидоров —\n"
"длинный перерыв 15–30 минут.\n"
"**********\n"
"Как работает в приложении\n\n"
"— Вы добавляете задачи и указываете количество «помидоров» на каждую.\n"
"— Нажмите ▶ — запускается первая фаза работы (25 мин).\n"
"— По окончании автоматически включается отдых (5 мин).\n"
"— Цикл повторяется, пока не закончатся помидоры задачи.\n"
"— По завершении задача отмечается как выполненная.\n"
"**********\n"
"Кому подходит\n\n"
"Тем, кто легко отвлекается и нуждается в чётких\n"
"коротких отрезках работы с частыми паузами.\n"
"**********\n"
"Совет\n\n"
"В перерыв не залипайте в телефон — лучше встать,\n"
"размяться, попить воды. Иначе отдых не восстановит внимание.");
    this->hide();
}

void MainWindow::pomodoro_reverse_clicked()
{

    reverse_pomodoro *mode = new reverse_pomodoro;
    mode->setOpenWindow(this);
    mode->setWindowTitle("Обратный помидор");
    mode->set_info_text(
"REVERSE POMODORO - 5 минут работы / 25 минут отдыха\n"
"**********\n"
"Суть техники\n\n"
"Перевёрнутый Pomodoro: короткие всплески работы\n"
"по 5 минут чередуются с длинным отдыхом 25 минут.\n"
"Подходит для тех, кому трудно заставить себя\n"
"начать, или для восстановления после выгорания.\n"
"**********\n"
"Как работает в приложении\n\n"
"— Добавьте задачи и количество циклов.\n"
"— Нажмите ▶ — стартует 5-минутная рабочая фаза.\n"
"— Затем 25 минут отдыха.\n"
"— Цикл повторяется заданное число раз.\n"
"**********\n"
"Кому подходит\n\n"
"— При низкой энергии или усталости.\n"
"— При возвращении к работе после долгого перерыва.\n"
"— Для «раскачки» перед большими задачами.\n"
"**********\n"
"Совет\n\n"
"Даже 5 минут работы — это уже прогресс.\n"
"Не ругайте себя за медленный темп: техника создана\n"
"именно для мягкого входа в работу.");
    mode->show();
    this->hide();
}

void MainWindow::flow_mode_clicked()
{   Timer *timer = new Timer();
    timer->setOpenWindow(this);
    timer->set_info_text(
"FLOWMODE - Работа без перерывов — таймер и секундомер\n"
"**********\n"
"Суть техники\n\n"
"Режим для глубокой концентрации. Никаких принудительных\n"
"перерывов: вы работаете столько, сколько нужно,\n"
"а часы лишь фиксируют время. Помогает войти в\n"
"состояние «потока» — полного погружения в задачу.\n"
"**********\n"
"Как работает в приложении\n\n"
"— Доступны аналоговые и цифровые часы.\n"
"— Кнопка ▶ запускает секундомер (счёт вверх)\n"
"или таймер (счёт вниз, если задать время).\n"
"— ❚❚ — пауза, ■ — сброс.\n"
"**********\n"
"Кому подходит\n\n"
"— Тем, кто легко входит в поток и не любит,\n"
"когда его прерывают.\n"
"— Для творческих и сложных задач, требующих\n"
"длительной концентрации.\n"
"**********\n"
"Совет\n\n"
"Ставьте будильник отдельно (на телефоне) на\n"
"напоминание встать и размяться, иначе легко\n"
"просидеть 3–4 часа без движения.");
    timer->show();
    this->hide();
}

void MainWindow::mode_60_10_clicked()
{

    Form60_10 *mode = new Form60_10;
    mode->setOpenWindow(this);
    mode->setWindowTitle("60/10");
    mode->set_info_text(
"60/10 - 60 минут работы / 10 минут отдыха\n"
"**********\n"
"Суть техники\n\n"
"Удлинённый Pomodoro: час глубокой работы и\n"
"10 минут отдыха. Подходит для задач, которые\n"
"требуют более длительного погружения, чем\n"
"классические 25 минут.\n"
"**********\n"
"Как работает в приложении\n\n"
"— Добавьте задачи и количество циклов.\n"
"— ▶ — старт работы (60 мин).\n"
"— По завершении — автоматический отдых (10 мин).\n"
"— Цикл повторяется.\n"
"**********\n"
"Кому подходит\n\n"
"— Тем, кто легко входит в поток, но нуждается\n"
"в напоминаниях об отдыхе.\n"
"— Для программирования, письма, анализа.\n"
"**********\n"
"Совет\n\n"
"10 минут отдыха лучше проводить без экрана:\n"
"прогулка, разминка, стакан воды. Иначе глаза\n"
"и мозг не успеют восстановиться.");
    mode->show();
    this->hide();
}

void MainWindow::mode_52_17_clicked()
{

    Form52_17 *mode = new Form52_17;
    mode->setOpenWindow(this);
    mode->setWindowTitle("52/17");
    mode->set_info_text(
"52/17 - 52 минуты работы / 17 минут отдыха\n"
"**********\n"
"Суть техники\n\n"
"Режим, основанный на исследовании DeskTime:\n"
"самые продуктивные сотрудники работали в среднем\n"
"52 минуты и отдыхали 17. Соотношение даёт\n"
"стабильно высокую концентрацию без переутомления.\n"
"**********\n"
"Как работает в приложении\n\n"
"— Добавьте задачи и количество циклов.\n"
"— ▶ — старт 52-минутной рабочей фазы.\n"
"— Затем 17 минут отдыха.\n"
"— Цикл повторяется.\n"
"**********\n"
"Кому подходит\n\n"
"— Офисным сотрудникам и фрилансерам.\n"
"— Тем, кто хочет сбалансировать работу и отдых\n"
"в течение дня.\n"
"**********\n"
"Совет\n\n"
"17 минут — достаточно, чтобы полноценно\n"
"переключиться: короткая прогулка, еда, разговор.\n"
"Не листайте ленту — это не отдых.");
    mode->show();
this->hide();

}

void MainWindow::mode_90_20_clicked()
{

    Form90_20 *mode = new Form90_20;
    mode->setOpenWindow(this);
    mode->setWindowTitle("90/20");
    mode->set_info_text(
"90/20 - 90 минут работы / 20 минут отдыха\n"
"**********\n"
"Суть техники\n\n"
"Основан на ультрадианных ритмах мозга: примерно\n"
"каждые 90 минут организм проходит цикл подъёма\n"
"и спада активности. Работа в такт этим ритмам\n"
"даёт максимум концентрации и меньше усталости.\n"
"**********\n"
"Как работает в приложении\n\n"
"— Добавьте задачи и количество циклов.\n"
"— ▶ — старт 90-минутного блока.\n"
"— По завершении — 20 минут отдыха.\n"
"— Цикл повторяется.\n"
"**********\n"
"Кому подходит\n\n"
"— Тем, кто работает над сложными проектами.\n"
"— Для творческих задач, требующих погружения.\n"
"— Тем, кто уже привык к длинным сессиям.\n"
"**********\n"
"Совет\n\n"
"90 минут — это максимум без потери качества.\n"
"Если чувствуете спад раньше — сделайте паузу.\n"
"20 минут отдыха используйте на сон, еду или\n"
"прогулку, а не на соцсети.\n");
    mode->show();
    this->hide();
}

void MainWindow::time_blocking_clicked(){

    time_blocking *mode = new time_blocking;
    mode->setOpenWindow(this);
   // mode->set_me(this->me);
    mode->setWindowTitle("Time_blocking");
    mode->set_info_text("TIME BLOCKING - Работа смысловыми блоками\n"
"**********\n"
"Суть техники"
"День заранее разбивается на блоки времени, каждый\n"
"блок посвящён одной задаче или теме. Переключение\n"
"между блоками — по расписанию, а не по желанию.\n"
"**********\n"
"Как работает в приложении\n\n"
"— Вы работаете в режиме часов с таймером.\n"
"— Задаёте длительность блока и стартуете ▶.\n"
"— По завершении блока — короткий отдых.\n"
"— Затем следующий блок.\n"
"**********\n"
"Кому подходит\n\n"
"— Тем, у кого много разнородных задач.\n"
"— Тем, кто склонен переключаться и терять фокус.\n"
"— Для планирования насыщенного дня.\n"
"**********\n"
"Совет\n\n"
"Планируйте блоки накануне вечером. В каждом блоке\n"
"должна быть одна чёткая цель. Не забывайте\n"
"оставлять «буферные» блоки на непредвиденное.\n");
    mode->show();
    this->hide();
}

//возврат назад
void MainWindow::getback_clicked()
{
    if (openWindow) {
        openWindow->show();
    }
    SPDLOG_DEBUG("MainWindow:: getback_clicked: возврат на окно {}",
                 openWindow->objectName().toStdString());
    this->hide();
}

//загрузка профиля по ссылке, если гость-авторизация
void MainWindow::on_label_3_linkActivated(const QString &link)
{
    SPDLOG_DEBUG("Нажата ссылка {},",
                 openWindow->objectName().toStdString());
    qDebug() << "on_label_3 CALLED, link =" << link;
    //ГОСТЬ
    if (link == "#guest") {
        if (!loginDialog) {
            loginDialog = new QDialog(this);
            loginDialog->setWindowTitle(tr("Окно авторизации"));
            loginDialog->setStyleSheet(R"(
    QDialog {
        background-color: rgba(0, 0, 0, 0.5) ;
    }
    QLabel {
        color: white;
        font-size: 14px;
font-weight: bold;
        padding: 8px 0;
    }
    QPushButton {
        background-color:rgba(0, 0, 0, 0.5) ;
        color: white;
        font-weight: bold;
        border: 1px solid #555;
        border-radius: 6px;
        padding: 8px 16px;
        font-size: 14px;
        min-width: 140px;
    }
    QPushButton:hover {
        background-color: rgba(255, 0, 0, 0.3);
        border-color: #6a9fb5;
    }
    QPushButton:pressed {
        background-color: rgba(255, 0, 0, 0.8);
    }
)");
            auto* layout = new QVBoxLayout(loginDialog);
            layout->addWidget(new QLabel(tr("Выберите действие:"),loginDialog));
            QPushButton* regBtn  = new QPushButton(tr("Регистрация"),loginDialog);
            QPushButton* authBtn = new QPushButton(tr("Авторизация"),loginDialog);
            QPushButton* cancel  = new QPushButton(tr("Отмена"),loginDialog);
            layout->addWidget(regBtn);
            layout->addWidget(authBtn);
            layout->addWidget(cancel);
            // три разных кода: 1 — рег, 2 — авт, 0 — отмена
            connect(regBtn,  &QPushButton::clicked, this, [this]() {loginDialog->done(1); });
            connect(authBtn, &QPushButton::clicked, this, [this]() {loginDialog->done(2); });
            connect(cancel,  &QPushButton::clicked, this, [this]() {loginDialog->done(0); });
            connect(loginDialog, &QObject::destroyed,
                    this, [this]() {loginDialog = nullptr; });
        }
        int choice = loginDialog->exec();
        SPDLOG_DEBUG("MainWindow::on_label_3_linkActivated:dlg closed, choice = {}",choice);
        if (choice == 1) {
            if (!registration) {
                registration = new NEW_USER_FORM();
                registration->setAttribute(Qt::WA_DeleteOnClose);
                registration->setOpenWindow(this);
                connect(registration, &QObject::destroyed,
                        this, [this]() { registration = nullptr; });
            }
            SPDLOG_DEBUG("MainWindow::on_label_3_linkActivated:Открыто окно регистрации");
            registration->show();
            registration->raise();
            registration->activateWindow();
            this->hide();
            return;
        }
        else if (choice == 2) {
            if (!autorisation) {
                autorisation = new USERS_FORM();
                autorisation->setAttribute(Qt::WA_DeleteOnClose);
                autorisation->setOpenWindow(this);
                connect(autorisation, &QObject::destroyed,
                        this, [this]() { autorisation = nullptr; });
            }
            SPDLOG_DEBUG("MainWindow::on_label_3_linkActivated:Открыто окно авторизации");
            autorisation->show();
            autorisation->raise();
            autorisation->activateWindow();
            this->hide();
            return;
        }
    }
    //ПРОФИЛЬ________________________________________________________________
    if (link == "#open") {
        if (!profileWindow) {
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
        }
        SPDLOG_DEBUG("MainWindow::on_label_3_linkActivated:Открыто окно профиля ");
        profileWindow->show();
        profileWindow->raise();
        profileWindow->activateWindow();
        this->hide();
    }
}

//загрузка фотографии
void MainWindow::loadUserProfile()
{
    Data_Manager& dm = Data_Manager::instance();
    me = dm.getCurrentUserProfile();
    //ГОСТЬ
    if (me.user_id < 0) {
    QPixmap pix(":/img/exit (1).jpg");
        if (!pix.isNull()) {
            ui->label_2->setPixmap(pix.scaled(180, 180,
                                              Qt::KeepAspectRatio,
                                              Qt::SmoothTransformation));
            ui->label_2->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        }
    return;
    }
    //ПРОФИЛЬ
    if (!me.Avatar.isEmpty()) {
        QPixmap avatar;
        avatar.loadFromData(me.Avatar);
        ui->label_2->setPixmap(avatar.scaled(180, 180,
                                                 Qt::KeepAspectRatio,
                                             Qt::SmoothTransformation));
        ui->label_2->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);}
    else {
        ui->label_2->setPixmap(QPixmap(":/img/icon_question.png")
                                   .scaled(180, 180,
                                           Qt::KeepAspectRatio,
                                           Qt::SmoothTransformation));
         ui->label_2->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        }
}

//окно информации
void MainWindow::on_label_4_linkActivated(const QString &link)
{
    if (link != "#info") return;
    if (!infoWindow) {
        QString info="FOLLOW THE RABBIT - Focus-трекер для продуктивной работы\n"
"**********\n"
"О приложении\n\n"

"Приложение объединяет восемь техник тайм-менеджмента в одном интерфейсе,\n"
"помогает отслеживать рабочие сессии и видеть свою продуктивность.\n"
" Все данные сохраняются в базе — вы всегда можете вернуться к своей истории.\n"
"**********\n"
"Режимы\n\n"
"Pomodoro - 25 минут работы / 5 отдыха\n"
"Reverse Pomodoro - 5 минут работы / 25 отдыха\n"
"FlowMode - работа без перерывов, таймер и секундомер\n"
"Time Blocking - работа смысловыми блоками\n"
"Eat the Frog - начни с самой сложной задачи\n"
"60/10 - 60 минут работы / 10 отдыха\n"
"52/17 - 52 минуты работы / 17 отдыха\n"
"90/20 - 90 минут работы / 20 отдыха\n"
"**********\n"
"Возможности\n\n"
"Создание задач с указанием количества «помидоров»\n"
"История рабочих сессий в базе данных\n"
"Профиль пользователя с аватаром и личными данными\n"
"Гостевой режим - работа без регистрации\n"
"Аналоговые и цифровые часы с таймером и секундомером\n"
"**********\n"
"Как пользоваться\n\n"
"1. Выберите подходящую технику на главном экране\n"
"2. Добавьте задачи (если режим это поддерживает)\n"
"3. Нажмите ▶ для старта\n"
"4. Следите за прогрессом по часам\n"
"5. Возвращайтесь к списку задач кнопкой «← К задачам»\n"
"**********\n"
"Версия 1.0.0 · FOLLOW THE RABBIT © 2026";
        infoWindow = new Information(nullptr,info);
        infoWindow->setAttribute(Qt::WA_DeleteOnClose);
        infoWindow->set_parent_window(this);
        connect(infoWindow, &QObject::destroyed,
                this, [this]() {infoWindow = nullptr; });
    }
    infoWindow->show();
    infoWindow->raise();
    infoWindow->activateWindow();
    this->hide();
}
