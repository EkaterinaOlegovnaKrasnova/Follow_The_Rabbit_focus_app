#ifndef REVERSE_POMODORO_H
#define REVERSE_POMODORO_H

#include "basetracker.h"
#include "ui_reverse_pomodoro.h"
#include <QWidget>

namespace Ui {
class reverse_pomodoro;
}

class reverse_pomodoro : public BaseTracker
{
    Q_OBJECT
public:
    explicit reverse_pomodoro(QWidget *parent = nullptr);
    ~reverse_pomodoro();
    void on_label_4_linkActivated(const QString &link);// Слот по ссылке (профиль пользователя / «Гость»)
    //Геттер/сеттер текста информации, показываемого в окне Information
    QString get_info_text()const override {return info_text;}
    void set_info_text(const QString& text)  override { this->info_text = text;}
private:
    QString info_text;
private:
    Ui::reverse_pomodoro *ui;
protected:
    QString windowTitleText() const override{return "Reverse pomodoro mode";}// Заголовок окна
    QString backgroundImage()const override {return ":/img/reverse (2).jpg";}// Путь к фону в ресурсах Qt
    QLabel* avatarLabel() const override { return ui->label_5; }// аватар пользователя
    QLabel* userLabel()   const override { return ui->label_4; }// имя пользователя (ссылка)
     QLabel* get_info_link() const override{ return ui->label_7;}//ссылка «Информация»
     QComboBox* activityComboBox() const override{return ui->comboBox;}//комбобокс с выбором задач
     QLabel* get_activity_label() const override{return ui->label_2;}//заголовок над комбобоксом
};

#endif // REVERSE_POMODORO_H
