#include "analythic_page.h"
#include "qpainter.h"
#include "ui_analythic_page.h"
#include "profileW.h"
#include "spdlog/spdlog.h"

Analythic_page::Analythic_page(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Analythic_page)
{
    ui->setupUi(this);
    resize(800,600);
    this->setWindowTitle("Аналитика");
    this->setStyleSheet(
        "QComboBox { color: white; font-weight: bold; "
        "background-color: rgba(0,0,0,0.5); "
        "border: 1px solid rgba(255,255,255,0.4); "
        "border-radius: 4px; padding: 2px; }"
        "QComboBox:hover { background-color: rgba(255,0,0,0.3); }"
        "QComboBox QAbstractItemView {"
        "   background-color: rgba(30, 30, 30, 0.9);" // Темный фон для списка
        "   color: white;"                            // Белый текст
        "   selection-background-color: rgba(255,0,0,0.5);" // Красный фон при наведении
        "   selection-color: white;"
        "   border: 1px solid rgba(255,255,255,0.4);"
        "}"

        "QLabel { color: white; font-weight: bold; "
        "background-color: transparent; }"

        "QLineEdit { color: white; font-weight: bold; "
        "background-color: rgba(0,0,0,0.5); "
        "border: 1px solid rgba(255,255,255,0.4); "
        "border-radius: 4px; padding: 2px; }"
        "QLineEdit:hover { background-color: rgba(255,0,0,0.3); }"

        "QPushButton { color: white; font-weight: bold; "
        "background-color: rgba(0,0,0,0.5); "
        "border: 1px solid rgba(255,255,255,0.4); "
        "border-radius: 4px; padding: 2px; }"
        "QPushButton:hover { background-color: rgba(255,0,0,0.3); }"
        "QPushButton:pressed { background-color: rgba(255,0,0,0.3); }"

        "QDateEdit{        "
        "color: white;background: rgba(0,0,0,0.5);"
        "   border: 1px solid rgba(255, 255, 255, 0.4);"
        "   border-radius: 8px;}"

        "QFrame {"
        "   background: rgba(0,0,0,0.5);"
        "   border: 1px solid rgba(255, 255, 255, 0.4);"
        "   border-radius: 8px;"
        "}"
        );
    for (QLabel* lbl : this->findChildren<QLabel*>()) {
        lbl->setAutoFillBackground(false);       // автозаполнение отключено
        lbl->setFrameShape(QFrame::NoFrame);     // рамка убрана
        lbl->setStyleSheet("background: transparent;border: none;");
    }
    ui->label->setStyleSheet("background-color: transparent; color: white;border: none;");
    ui->label_7->setStyleSheet("QLabel { color: white; font-weight: bold; "
                               "background-color: rgba(0,0,0,0.5); "
                               "border: 1px solid rgba(255,255,255,0.4); "
                               "border-radius: 4px; padding: 2px; }"
                               "QLabel:hover { background-color: rgba(255,0,0,0.3); }");
    SPDLOG_TRACE("Страница аналитики создана");
}

Analythic_page::~Analythic_page()
{
    delete ui;
    SPDLOG_TRACE("Страница аналитики удалена");
}

void Analythic_page::get_back(){
    if (window) window->show();
    this->hide();
    SPDLOG_TRACE("Возвращение к {}",window->objectName().toStdString());
}

void Analythic_page::paintEvent(QPaintEvent* ){
    QPainter p(this);
    QPixmap pic(":/img/results.jpg");
    if (pic.isNull()){
        SPDLOG_WARN("Изображение не загружено");
        return;}
    QSize s =pic.size().scaled(size(),Qt::KeepAspectRatioByExpanding);
    QPixmap scaled = pic.scaled(s,Qt::KeepAspectRatioByExpanding,Qt::SmoothTransformation);
    p.drawPixmap((width()-scaled.width())/2,
                 (height()-scaled.height())/2,scaled);
}
void Analythic_page::fill_tasks_combo()
{
    ui->comboBox->blockSignals(true);
    ui->comboBox->clear();
    const auto tasks = Data_Manager::instance().getTasksForUser(me->user_id);
    for (const auto& t : tasks) {
        ui->comboBox->addItem(t.task_name, t.task_id);
    }
    if (ui->comboBox->count() > 0)
        ui->comboBox->setCurrentIndex(0);
    ui->comboBox->blockSignals(false);
    SPDLOG_TRACE("Комбо-бокс загружен");
}

void Analythic_page::on_comboBox_currentIndexChanged(int index)
{
    if (!me || index < 0){
        SPDLOG_WARN("Возможно индекс отрицательный или информация о пользователе не загружена");
        return;}
    const qint64 task_id = ui->comboBox->itemData(index).toLongLong();
    refresh_analytics(task_id);
    SPDLOG_TRACE(QString("Выбрана задача: %1").arg(task_id).toStdString());
}

void Analythic_page::refresh_analytics(qint64 task_id)
{
    if (!me) {
        SPDLOG_WARN("Информация о пользователе не загружена");
        return;}
    auto& dm = Data_Manager::instance();
    const Analytics_Summary s = dm.getAnalyticsSummary(me->user_id, task_id);
    auto fmtMin = [](quint64 sec){
        return sec ? QString::number(sec / 60) + " мин" : QString("-");
    };
    ui->lineEdit_7->setText(s.favorite_technic.isEmpty() ? QString("-") : s.favorite_technic);
    ui->lineEdit_5->setText(fmtMin(s.avg_session_sec));
    ui->lineEdit_6->setText(fmtMin(s.longest_session_sec));
    ui->lineEdit_8->setText(QString::number(s.completion_rate * 100.0, 'f', 1) + " %");
    ui->lineEdit_9->setText(QString("%1% работа - %2% отдых")
                                .arg(s.work_break_ratio.first)
                                .arg(s.work_break_ratio.second));
    ui->lineEdit_10->setText(QString::number(s.procrastination_index) + " %");
    // формат
    ui->dateEdit->setDisplayFormat("dd.MM.yyyy"); // день
    ui->dateEdit_4->setDisplayFormat("MMMM yyyy"); // месяц
    ui->dateEdit_5->setDisplayFormat("yyyy");      // год
    const QDate today = QDate::currentDate();
    ui->dateEdit->setDate(today);
    ui->dateEdit_4->setDate(today);
    ui->dateEdit_5->setDate(today);
    ui->lineEdit->setText(fmtMin(s.total_day_sec));   // Время за день
    ui->lineEdit_2->setText(fmtMin(s.total_month_sec)); // Время за месяц
    ui->lineEdit_3->setText(fmtMin(s.total_year_sec));  // Время за год
    SPDLOG_TRACE("Вывод аналитики");
}

void Analythic_page::on_pushButton_clicked()
{ SPDLOG_TRACE("Нажата кнопка назад");
    get_back();
}

void Analythic_page::set_profile(){
    if (!window || me) {
         SPDLOG_WARN("");
        return;}
    this->me = window->get_me();
    ui->lineEdit_4->setText(QString("%1 %2").arg(me->Name).arg(me->Surname));
    fill_tasks_combo();
    if (ui->comboBox->count() > 0) {
        const qint64 task_id = ui->comboBox->itemData(0).toLongLong();
        refresh_analytics(task_id);
    } else {
        SPDLOG_WARN("В БД задач нет");
        refresh_analytics(-1); // или отдельная очистка
    }
    for (auto* le : {ui->lineEdit_4, ui->lineEdit_5, ui->lineEdit_6, ui->lineEdit_7,
                     ui->lineEdit_8, ui->lineEdit_9, ui->lineEdit_10}) {
        le->setReadOnly(true);
        le->setFocusPolicy(Qt::NoFocus);
        SPDLOG_TRACE(QString("В %1 строке отключен фокус и возможность редактирования")
                .arg(le->objectName()).toStdString());
    }
    SPDLOG_TRACE("Настройка профиля прошла успешно");
}

void Analythic_page::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    if (me && ui->comboBox->count() > 0) {
        int index = ui->comboBox->currentIndex();
        if (index >= 0) {
            qint64 task_id = ui->comboBox->itemData(index).toLongLong();
            refresh_analytics(task_id);
        }
         SPDLOG_TRACE("Пересчет аналитики для выбранной задачи");
    }
}
void Analythic_page::loadUserProfile()
{
    if(!me){
        SPDLOG_WARN("Профиль юзера не загружен");
        return;
    }
    ui->label_7->setText(me->Name + " " + me->Surname);
    updateAvatar();
    SPDLOG_TRACE("Профиль юзера загружен");
}
void Analythic_page::updateAvatar()
{
    if (!me) {
        SPDLOG_WARN("Профиль юзера не загружен");
        return;}
    QPixmap src;
    if (!me->Avatar.isEmpty()) {
        SPDLOG_DEBUG("В профиле есть данные аватара (размер: {} байт)",
                    me->Avatar.size());
        if (!src.loadFromData(me->Avatar)) {
            SPDLOG_WARN("Не удалось загрузить аватар из данных."
                         "Используется заглушка.");
            src = QPixmap(":/img/icon_question.png");
        }
    } else {
        SPDLOG_DEBUG("В профиле нет аватара, использовано фото-заглушка");
        src = QPixmap(":/img/icon_question.png");
    }
    if (src.isNull()) {
        ui->label_6->setText("?");
        ui->label_6->setStyleSheet(
            "QLabel { color: white; font-size: 100px; font-weight: bold; }");
        SPDLOG_DEBUG("Фото-заглушка отсутствует, использова текст");
        return;
    }
    QSize target = ui->label_6->size();
    if (target.isEmpty()) return;
    ui->label_6->setPixmap(src.scaled(
        target,
        Qt::KeepAspectRatioByExpanding,
        Qt::SmoothTransformation));
    SPDLOG_TRACE("Аватар загружен");
}
