#include "user_settings.h"
#include "mainwindow.h"
#include "password_utils.h"
#include "qpainter.h"
#include "spdlog/spdlog.h"
#include "ui_user_settings.h"
#include <QResizeEvent>
#include <qmessagebox.h>
user_settings::user_settings(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::user_settings)
{
    ui->setupUi(this);
   SPDLOG_TRACE("Страница настроек создана");
    ui->lineEdit_3->setInputMask("+7 (999) 999-99-99;_");
}

user_settings::~user_settings()
{   SPDLOG_TRACE("Страница настроек удалена");
    delete ui;
}

void user_settings::paintEvent(QPaintEvent *) {
    QPainter p(this);
    QPixmap bg(":/img/cl1 (1).jpg");
    if (bg.isNull()) {
        SPDLOG_WARN("Изображение фона не загружено");
        return;}
    QSize s = bg.size().scaled(size(), Qt::KeepAspectRatioByExpanding);
    QPixmap scaled = bg.scaled(s, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    p.drawPixmap((width()  - scaled.width())  / 2,
                 (height() - scaled.height()) / 2, scaled);
}

void user_settings::getback_clicked()
{
    SPDLOG_TRACE("m_mainWindow = {}", (void*)m_mainWindow);
    if (m_mainWindow) {
        if (auto* mw = qobject_cast<MainWindow*>(m_mainWindow)) {
            SPDLOG_TRACE("cast OK");
            mw->loadUserProfile();
            mw->loadCurrentUser();
        } else {
            SPDLOG_ERROR("cast FAILED — m_mainWindow не MainWindow*");
        }
        m_mainWindow->show();
        SPDLOG_TRACE("show() called, isVisible = {}", m_mainWindow->isVisible());
    } else {
        SPDLOG_ERROR("m_mainWindow is NULL — некого показывать!");
    }
    this->hide();
    SPDLOG_TRACE("this hidden, isVisible = {}", this->isVisible());
}

void user_settings::loadCurrentUser(){
    if (!me) return;
    ui->lineEdit->setText(me->Name);
    ui->lineEdit_9->setText(me->Middle_Name);
    ui->lineEdit_12->setText(me->Surname);
    ui->lineEdit_2->setText(me->DateBirth.toString("dd.MM.yyyy"));
    ui->lineEdit_3->setText(me->phone_number);
    ui->lineEdit_4->setText(me->email);
    ui->lineEdit_5->setText(me->login);
    ui->lineEdit_6->clear();
    ui->lineEdit_6->setEchoMode(QLineEdit::Password);
    ui->lineEdit_6->setPlaceholderText("Оставьте пустым, чтобы не менять");
    ui->lineEdit_7->setText(me->created_at.toString("dd.MM.yyyy"));
    updateAvatar();
    SPDLOG_TRACE("Загрузка профиля юзера прошла успешно");
}

void user_settings::updateAvatar()
{
    if (!me) return;
    QPixmap src;
    if (!me->Avatar.isEmpty()) {
        if (!src.loadFromData(me->Avatar)) {
            src = QPixmap(":/img/icon_question.png");
        }
    } else {
        src = QPixmap(":/img/icon_question.png");
    }
    if (src.isNull()) {
        ui->label->setText("?");
        ui->label->setStyleSheet(
            "QLabel { color: white; font-size: 100px; font-weight: bold; }");
        return;
    }
    QSize target = ui->label->size();
    if (target.isEmpty()) return;
    ui->label->setPixmap(src.scaled(
        target,
        Qt::KeepAspectRatioByExpanding,
        Qt::SmoothTransformation));
}

void user_settings::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateAvatar();
    SPDLOG_TRACE("Изменение размера окна");
}

void user_settings::on_pushButton_clicked()
{   SPDLOG_TRACE("Нажата кнопка назад");
     getback_clicked();
}

void user_settings::set_me(std::shared_ptr<User_Profile> user){
    this->me = user;
    SPDLOG_TRACE("Использование сеттера для загрузки  профиля юзера");
}

void user_settings::on_lineEdit_textChanged(const QString &arg1)
{
    ui->lineEdit->setText(arg1);
}

void user_settings::on_pushButton_2_clicked()
{
    Data_Manager& dm = Data_Manager::instance();
    if (dm.get_currentUser() < 0) {
        SPDLOG_ERROR("Пользователь - гость");
        return;
    }
    User_Profile me = dm.getCurrentUserProfile();
    me.Name        = ui->lineEdit->text().trimmed();
    me.Middle_Name = ui->lineEdit_9->text().trimmed();
    me.Surname     = ui->lineEdit_12->text().trimmed();
    me.phone_number = extractDigits(ui->lineEdit_3->text());
    me.email       = ui->lineEdit_4->text().trimmed();
    me.login       = ui->lineEdit_5->text().trimmed();
    QString dateStr = ui->lineEdit_2->text().trimmed();
    if (!dateStr.isEmpty()) {
        QDate d = QDate::fromString(dateStr, "dd.MM.yyyy");
        if (d.isValid())
            me.DateBirth = d;
        else {
            QMessageBox::warning(this, tr("Ошибка"),
                                 tr("Неверный формат даты. Нужно: дд.мм.гггг"));
            SPDLOG_ERROR("Неверный формат даты. ");
            return;
        }
    }
    if (me.Name.isEmpty() || me.Surname.isEmpty() || me.login.isEmpty()) {
        QMessageBox::warning(this, tr("Ошибка"),
                             tr("Имя, фамилия и логин не могут быть пустыми."));
        SPDLOG_ERROR("Имя, фамилия и логин не могут быть пустыми.");
        return;
    }
    if (!check_login(this, me.login)){
        SPDLOG_ERROR("Логин некорректный");
        return;
    }
    if (!check_phone(this, me.phone_number)){
        SPDLOG_ERROR("Телефон некорректный");
        return;
    }
    if (!check_email(this,me.email)){
        SPDLOG_ERROR("Email некорректный");
        return;
    }
    QString newPass = ui->lineEdit_6->text().trimmed();
    if (!newPass.isEmpty()) {
        if (!password_check(this, newPass)) {
            SPDLOG_ERROR("Пароль некорректный");
            return;
        }
        QString newHash = hashPassword(newPass, me.salt);
        if (newHash != me.password) {
            dm.updateUserPassword(me.user_id, newPass);
            SPDLOG_TRACE("Изменение пароля");
        }
    }
    if (dm.updateUserData(me)) {
        QMessageBox::information(this, tr("Готово"), tr("Профиль сохранён."));
        emit profileUpdated();
        SPDLOG_TRACE("Профиль сохранён.");
    } else {
        QMessageBox::warning(this, tr("Ошибка"),
                             tr("Не удалось сохранить профиль."));
        SPDLOG_ERROR("Не удалось сохранить профиль.");
    }
}

void user_settings::on_pushButton_3_clicked()
{
    QMessageBox::information(this,"Информация","Пока реализации нет");
}

