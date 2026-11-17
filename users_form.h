#ifndef USERS_FORM_H
#define USERS_FORM_H

#include "data_manager.h"

#include <QWidget>
class MainWindow;
namespace Ui {
class USERS_FORM;
}

class USERS_FORM : public QWidget
{
    Q_OBJECT

public:
    explicit USERS_FORM(QWidget *parent = nullptr);
    ~USERS_FORM();
    void paintEvent(QPaintEvent *event);//отрисовка
    void setOpenWindow(QWidget* openWindow) { this->openWindow = openWindow; }//сеттер предыдущего окна
    void getback_clicked();//метод возврата на предыдщее окно
private slots:
    void on_pushButton_clicked();//слот кнопки назад
    void on_OK_clicked();//метод авторизации
private:
    Ui::USERS_FORM *ui;
    QWidget* openWindow = nullptr;
    User_Profile profile;
    MainWindow* main_window=nullptr;
};

#endif // USERS_FORM_H
