#ifndef TIMER_H
#define TIMER_H

#include "basetracker.h"
#include "digital_clock.h"
#include "myclock.h"
#include "ui_timer.h"
#include <QWidget>

namespace Ui {
class Timer;
}

class Timer : public BaseTracker
{
    Q_OBJECT

public:
    explicit Timer(QWidget *parent = nullptr);
    ~Timer();
    void on_label_6_linkActivated(const QString &link);// слот по ссылке (профиль пользователя / «Гость»)
protected:
    QString windowTitleText() const override{return "Flow mode";} // Заголовок окна
    QString backgroundImage()const override {return ":/img/change_userF.jpg";}// Путь к фону
    QLabel* avatarLabel() const override { return ui->label_5; } // аватар пользователя
    QLabel* userLabel()   const override { return ui->label_6; }// имя пользователя (ссылка)
    QLabel* get_info_link() const override{ return ui->label_7;} // ссылка «Информация»
    // В режиме Flowнет комбобокса выбора активности возврат nullptr.
    //Базовый код проверяет указатель и пропускает работу.
    QComboBox* activityComboBox() const override{return nullptr;}
    QLabel* get_activity_label() const override{return nullptr;}
public:
     //Геттер/сеттер текста информации, показываемого в окне Information
    QString get_info_text()const override {return info_text;}
    void set_info_text(const QString& text) override { this->info_text = text;}
private:
    QString info_text; // Хранилище текста информации для этого конкретного модуля
private slots:
    void on_timer_clicked(); //слот на кнопку timer
private:
    Ui::Timer *ui;
    QWidget* m_openWindow = nullptr;
};

#endif // TIMER_H
