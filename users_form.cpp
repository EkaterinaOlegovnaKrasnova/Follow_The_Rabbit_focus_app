#include "users_form.h"
#include "data_manager.h"
#include "mainwindow.h"
#include "qpainter.h"
#include "spdlog/spdlog.h"
#include "ui_users_form.h"
#include <QMessageBox>
#include "mainwindow.h"

USERS_FORM::USERS_FORM(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::USERS_FORM)
{
    ui->setupUi(this);
    this->setWindowTitle("Авторизация");
    SPDLOG_TRACE("Создано окно авторизации");
    resize(800,500);
    ui->lineEdit->setEchoMode(QLineEdit::Password);

    // кнопка "показать пароль" — pushButton_eye
    connect(ui->pushButton_2, &QPushButton::clicked, this, [this]() {
        if (ui->lineEdit->echoMode() == QLineEdit::Password) {
            ui->lineEdit->setEchoMode(QLineEdit::Normal);
            ui->pushButton_2->setText("•");
            SPDLOG_TRACE("Пароль показан");
        } else {
            ui->lineEdit->setEchoMode(QLineEdit::Password);
            ui->pushButton_2->setText("👁");
            SPDLOG_TRACE("Пароль скрыт");
        }
    });
}

USERS_FORM::~USERS_FORM()
{
    delete ui;
    SPDLOG_TRACE("Окно авторизации удалено");
}
void USERS_FORM::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QPixmap bg(":/img/exit (3).jpg");
    if (bg.isNull()) {
        SPDLOG_WARN("Изображение не загрузилось");
        return;
    }
    QSize s = bg.size().scaled(size(), Qt::KeepAspectRatioByExpanding);
    QPixmap scaled = bg.scaled(s, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    p.drawPixmap((width()  - scaled.width())  / 2,
                 (height() - scaled.height()) / 2, scaled);
}

void USERS_FORM::getback_clicked()
{
    SPDLOG_TRACE("Возвращение к ...");
    if (openWindow) {
        openWindow->show();
    }
    this->hide();
}

void USERS_FORM::on_pushButton_clicked()
{
    SPDLOG_TRACE("Нажата кнопка назад");
    getback_clicked();
}

void USERS_FORM::on_OK_clicked()
{
    Data_Manager& dm = Data_Manager::instance();
    const QString login    = ui->lineEdit_2->text().trimmed();
    const QString password = ui->lineEdit->text();
    if (login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка",
                             "Введите логин и пароль");
        SPDLOG_ERROR("Логин или пароль пусты");
        return;
    }
    User_Profile user;
    if (!dm.loginUser(login, password, user)) {
        QMessageBox::warning(this, "Ошибка",
                             "Неверный логин или пароль");
        SPDLOG_ERROR("Неверный логин или пароль");
        return;
    }
    SPDLOG_DEBUG("Вход выполнен: user_id = {} login = {}", user.user_id,user.login.toStdString());
    main_window = new MainWindow();
    main_window->setOpenWindow(openWindow);
    main_window->show();
    this->close();
}

