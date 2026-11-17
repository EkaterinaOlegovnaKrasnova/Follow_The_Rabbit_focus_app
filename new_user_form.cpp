#include "new_user_form.h"
#include "mainwindow.h"
#include "password_utils.h"
#include "qpainter.h"
#include "spdlog/spdlog.h"
#include "ui_new_user_form.h"
#include <QMessageBox>


NEW_USER_FORM::NEW_USER_FORM(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::NEW_USER_FORM)
{
    ui->setupUi(this);
    this->setWindowTitle("Регистрация");
    updateOkButtonState();
    SPDLOG_TRACE("Создано окно регистрации");
    ui->lineEdit_7->setInputMask("+7 (999) 999-99-99;_");
}

NEW_USER_FORM::~NEW_USER_FORM()
{
    delete ui;
}

void NEW_USER_FORM::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QPixmap bg(":/img/UserF.jpeg");
    if (bg.isNull()) {
        SPDLOG_WARN("Изображение не загрузилось");
        return;
    }
    QSize s = bg.size().scaled(size(), Qt::KeepAspectRatioByExpanding);
    QPixmap scaled = bg.scaled(s, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    p.drawPixmap((width()  - scaled.width())  / 2,
                 (height() - scaled.height()) / 2, scaled);
}

void NEW_USER_FORM::getback_clicked()
{
    if (openWindow) {
        openWindow->show();
    }
    SPDLOG_TRACE("Возвращение к {}",openWindow->objectName().toStdString());
    this->hide();
}

void NEW_USER_FORM::on_OK_2_clicked()
{
    SPDLOG_TRACE("Нажата кнопка назад");
    getback_clicked();
}

void NEW_USER_FORM::on_OK_clicked()
{
    //Защита от повторного клика
    if (isSubmitting) return;
    isSubmitting = true;

    //  сброс флага при выходе
    auto fail = [this]() { isSubmitting = false; };

    // Валидация введённых данных
    if (!check_login(this, profile.login))       { fail(); return; }
    if (!check_phone(this, profile.phone_number)){ fail(); return; }
    if (!check_email(this, profile.email))       { fail(); return; }
    if (!password_check(this, profile.password)) { fail(); return; }

    //  открыта ли база
    Data_Manager& dm = Data_Manager::instance();
    if (!dm.isOpen()) {
        SPDLOG_ERROR("on_OK_clicked: база данных не подключена");
        QMessageBox::critical(this, "Ошибка",
                              "Нет подключения к базе данных.\n"
                              "Проверьте, что MySQL запущен и настройки верны.");
        fail();
        return;
    }
    //копия профиля с солью и хэшем, не вношу в исходный profile
    User_Profile toSave = profile;
    toSave.salt     = generateSalt();
    toSave.password = hashPassword(profile.password, toSave.salt);

    //Сохранение в БД
    qint64 newId = dm.sendNewUserData(toSave);
    if (newId < 0) {
        SPDLOG_ERROR("on_OK_clicked: sendNewUserData вернул {}", newId);
        QMessageBox::warning(this, "Ошибка",
                             "Не удалось создать пользователя.\n"
                             "Возможно, такой логин уже занят.");
        fail();
        return;
    }
    SPDLOG_INFO("on_OK_clicked: пользователь создан, user_id = {}", newId);
    // запоминание профиля
    dm.setCurrentUser(newId);
    //Открытие главного окна
    main_window = new MainWindow();
    main_window->setOpenWindow(openWindow);
    main_window->show();
    this->close();
}

void NEW_USER_FORM::updateOkButtonState()
{
    SPDLOG_TRACE("Проверка на заполнение");
    bool ok = !profile.Name.isEmpty()
              && !profile.Surname.isEmpty()
              && !profile.login.isEmpty()
              && !profile.email.isEmpty()
              && !profile.password.isEmpty()
              && !profile.phone_number.isEmpty()
              && profile.DateBirth.isValid();

    ui->OK->setEnabled(ok);
}

void NEW_USER_FORM::on_lineEdit_4_textChanged(const QString &arg1)//ИМЯ
{
    profile.Name = arg1.trimmed();
    //SPDLOG_TRACE("Введено имя");
    updateOkButtonState();
}

void NEW_USER_FORM::on_lineEdit_12_textChanged(const QString &arg1)//Отчество
{
    profile.Middle_Name=arg1.trimmed();
    //SPDLOG_TRACE("Введено отчество");
     updateOkButtonState();
}

void NEW_USER_FORM::on_lineEdit_2_textChanged(const QString &arg1)//Фамилия
{
    profile.Surname = arg1.trimmed();
    //SPDLOG_TRACE("Введена фамилия");
    updateOkButtonState();
}

void NEW_USER_FORM::on_lineEdit_5_textChanged(const QString &arg1)  // дата
{
    QDate d = QDate::fromString(arg1.trimmed(), "dd.MM.yyyy");
    if (!d.isValid()) {
        profile.DateBirth = QDate();
        updateOkButtonState();
        return;
    }
    profile.DateBirth = d;
    //SPDLOG_TRACE("Введена дата рождения");
    updateOkButtonState();
}

void NEW_USER_FORM::on_lineEdit_textChanged(const QString &arg1)
{
    profile.login=arg1.trimmed();
    //SPDLOG_TRACE("Введен логин");
    updateOkButtonState();
}

void NEW_USER_FORM::on_lineEdit_3_textChanged(const QString &arg1)//пароль
{

    profile.password = arg1.trimmed();

    //SPDLOG_TRACE("Введен пароль");
    updateOkButtonState();
}

void NEW_USER_FORM::on_lineEdit_6_textChanged(const QString &arg1)
{
    profile.email = arg1.trimmed();
    //SPDLOG_TRACE("Введена электронная почта");
    updateOkButtonState();
}

void NEW_USER_FORM::on_lineEdit_7_textChanged(const QString& arg1)
{
    QString digits;
    for (QChar c : arg1) {
        if (c >= '0' && c <= '9')
            digits += c;
    }

    if (digits == "7" && arg1.contains('_'))
        digits.clear();

    profile.phone_number = digits;
    //SPDLOG_TRACE("Введен номер телефона: {}", digits.toStdString());
    updateOkButtonState();
}

