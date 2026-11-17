#ifndef NEW_USER_FORM_H
#define NEW_USER_FORM_H
#include "data_manager.h"
#include <QWidget>

class MainWindow;

namespace Ui {
class NEW_USER_FORM;
}

class NEW_USER_FORM : public QWidget
{
    Q_OBJECT

public:
    explicit NEW_USER_FORM(QWidget *parent = nullptr);
    ~NEW_USER_FORM();
    void paintEvent(QPaintEvent *event);//исование фона
    void setOpenWindow(QWidget* openWindow) { this->openWindow = openWindow; }// Сеттер на окно, из которого открыли регистрацию.
    void getback_clicked(); // Возврат на предыдущее окно (openWindow).
private slots:
    void on_OK_2_clicked();//слот назад
    void on_OK_clicked();//ок -регистрация. Защищён флагом isSubmitting от повторного клика
    void on_lineEdit_4_textChanged(const QString &arg1);// Имя → profile.Name
    void on_lineEdit_12_textChanged(const QString &arg1); // Отчество → profile.Middle_Name
    void on_lineEdit_2_textChanged(const QString &arg1); // Фамилия → profile.Surname
    void on_lineEdit_5_textChanged(const QString &arg1);// Дата рождения → profile.DateBirth
    void on_lineEdit_textChanged(const QString &arg1); // Логин → profile.login
    void on_lineEdit_3_textChanged(const QString &arg1);// Пароль → profile.password
    void on_lineEdit_6_textChanged(const QString &arg1);// Email → profile.email
    void on_lineEdit_7_textChanged(const QString &arg1);// Телефон → profile.phone_number
private:
    Ui::NEW_USER_FORM *ui;
    QWidget* openWindow = nullptr;
    MainWindow* main_window=nullptr;
    User_Profile profile;
    // Включает/выключает кнопку OK в зависимости от того,заполнены ли все обязательные поля (имя, фамилия,
    //логин,email, пароль, телефон, дата рождения)
    void updateOkButtonState();
    bool isSubmitting = false;  // Флаг защиты от повторного клика по OK.
};

#endif // NEW_USER_FORM_H
