#ifndef FORM9__2__H
#define FORM9__2__H

#include "basetracker.h"
#include "digital_clock.h"
#include "myclock.h"
#include "ui_form90_20.h"
#include <QWidget>

namespace Ui {
class Form90_20;
}

class Form90_20 : public BaseTracker
{
    Q_OBJECT
public:
    explicit Form90_20(QWidget *parent = nullptr);
    ~Form90_20();
    void on_label_4_linkActivated(const QString &link);// Слот по ссылке (профиль пользователя / «Гость»)
    //Геттер/сеттер текста информации, показываемого в окне Information
    QString get_info_text()const override {return info_text;}
    void set_info_text(const QString& text)  override { this->info_text = text;}
private:
    QString info_text;
protected:
    QString windowTitleText() const override{return "90/20 mode";}// Заголовок окна
    QString backgroundImage()const override {return ":/img/clock (2).jpg";}// Путь к фону в ресурсах Qt
    QLabel* avatarLabel() const override { return ui->label_5; }// аватар пользователя
    QLabel* userLabel()   const override { return ui->label_4; }// имя пользователя (ссылка)
     QLabel* get_info_link() const override{ return ui->label_7;}//ссылка «Информация»
    QComboBox* activityComboBox() const override{return ui->comboBox;}//комбобокс с выбором задач
     QLabel* get_activity_label() const override{return ui->label_2;}//заголовок над комбобоксом
private:
    Ui::Form90_20 *ui;
};

#endif // FORM9__2__H
