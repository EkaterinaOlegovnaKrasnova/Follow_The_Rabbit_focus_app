#include "open.h"
#include "mainwindow.h"
#include "new_user_form.h"
#include "spdlog/spdlog.h"
#include "ui_open.h"
#include "users_form.h"
#include <QMessageBox>
#include <QPainter>
#include <QResizeEvent>

open::open(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::open)
{
    ui->setupUi(this);
    this->setWindowTitle("FOLLOW THE RABBIT");
    ui->pushButton_2->setFixedSize(200, 50);
    ui->pushButton_2->setStyleSheet(
        "QPushButton {"
        "    background: rgba(0, 0, 0, 0.6);"
        "    color: white;"
        "    border-radius: 10px;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "}"

        "QPushButton:hover {"
        "    background: rgba(255, 0, 0, 0.7);"
        "}"
        "QPushButton:pressed {"
        "    background: rgba(0, 0, 0, 0.9);"
        "}"
        );
    // Кнопка "Выход" (exit)
    ui->exit->setFixedSize(200, 50);
    ui->exit->setStyleSheet(
        "QPushButton {"
        "    background: rgba(0, 0, 0, 0.6);"
        "    color: white;"
        "    border-radius: 10px;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: rgba(255, 0, 0, 0.7);"
        "}"
        "QPushButton:pressed {"
        "    background: rgba(255, 0, 0, 0.9);"
        "}"
        );
        ui->new_user->setFixedSize(200, 50);
        ui->new_user->setStyleSheet(
            "QPushButton {"
            "    background: rgba(0, 0, 0, 0.6);"
            "    color: white;"
            "    border-radius: 10px;"
            "    font-size: 16px;"
            "    font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "    background: rgba(255, 0, 0, 0.7);"
            "}"
            "QPushButton:pressed {"
            "    background: rgba(255, 0, 0, 0.9);"
            "}"

            );
        ui->choose_user->setFixedSize(200, 50);
        ui->choose_user->setStyleSheet(
            "QPushButton {"
            "    background: rgba(0, 0, 0, 0.6);"
            "    color: white;"
            "    border-radius: 10px;"
            "    font-size: 16px;"
            "    font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "    background: rgba(255, 0, 0, 0.7);"
            "}"
            "QPushButton:pressed {"
            "    background: rgba(255, 0, 0, 0.9);"
            "}");
        resize(800, 500);
        SPDLOG_TRACE("Окно входа создано");
}

open::~open()
{
    delete ui;
}

void open::on_pushButton_2_clicked()
{
    SPDLOG_TRACE("Нажата кнопка гостевого режима");
    emit openproject();
    this->hide();

}

void open::on_exit_clicked()
{
    SPDLOG_TRACE("Нажата кнопка выхода");
    QApplication::quit();
}

void open::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QPixmap bg(":/img/1st.jpeg");
    if (bg.isNull()) {
        SPDLOG_WARN("Изображение не загрузилось");
        return;}
    QSize s = bg.size().scaled(size(), Qt::KeepAspectRatioByExpanding);
    QPixmap scaled = bg.scaled(s, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    p.drawPixmap((width()  - scaled.width())  / 2,
                 (height() - scaled.height()) / 2, scaled);
}

void open::on_new_user_clicked()
{
    SPDLOG_TRACE("Нажата кнопка регистрации");
    NEW_USER_FORM* newUserForm = new NEW_USER_FORM;
    newUserForm->setOpenWindow(this);
    newUserForm->setAttribute(Qt::WA_DeleteOnClose);
    newUserForm->setOpenWindow(this);
    newUserForm->show();
    this->hide();
}

void open::on_choose_user_clicked()
{
    SPDLOG_TRACE("Нажата кнопка авторизации");
    USERS_FORM* users_form=new USERS_FORM;
    users_form->setOpenWindow(this);
    users_form->setAttribute(Qt::WA_DeleteOnClose);
    users_form->setOpenWindow(this);
    users_form->show();
    this->hide();
}

