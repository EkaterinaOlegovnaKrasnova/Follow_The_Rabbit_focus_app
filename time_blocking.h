#ifndef TIME_BLOCKING_H
#define TIME_BLOCKING_H

#include "basetracker.h"
#include "digital_clock.h"
#include "myclock.h"
#include "ui_time_blocking.h"
#include <QWidget>

namespace Ui {
class time_blocking;
}

class time_blocking : public BaseTracker
{
    Q_OBJECT

public:
    explicit time_blocking(QWidget *parent = nullptr);
    ~time_blocking();
    void on_label_4_linkActivated(const QString &link);// Слот по ссылке (профиль пользователя / «Гость»)
    //Геттер/сеттер текста информации, показываемого в окне Information
    QString get_info_text()const override {return info_text;}
    void set_info_text(const QString& text) override { this->info_text = text;}
private:
    Ui::time_blocking *ui;
    QString info_text;
protected:
    QString windowTitleText() const override{// Заголовок окна
        return "TimeBlocking mode";}
    QString backgroundImage()const override { // Путь к фону в ресурсах Qt
        return ":/img/90_20_2.jpg";}
    QLabel* avatarLabel() const override { // аватар пользователя
        return ui->label_5; }
    QLabel* userLabel()   const override {// имя пользователя (ссылка)
        return ui->label_4; }
    QLabel* get_info_link() const override{//ссылка «Информация»
        return ui->label_7;}
    // В режиме TimeBlocking нет комбобокса выбора активности,возврат nullptr.
    //Базовый код проверяет указатель и пропускает работу.
    QComboBox* activityComboBox() const override{return nullptr;}
    QLabel* get_activity_label() const override{return nullptr;}
};

#endif // TIME_BLOCKING_H
