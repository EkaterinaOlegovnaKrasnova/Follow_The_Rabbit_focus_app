#ifndef FORM52_17_H
#define FORM52_17_H

#include "basetracker.h"
#include "digital_clock.h"
#include "myclock.h"
#include "ui_form52_17.h"
#include <QWidget>

namespace Ui {
class Form52_17;
}

class Form52_17 : public BaseTracker
{
    Q_OBJECT

public:
    explicit Form52_17(QWidget *parent = nullptr);
    ~Form52_17();
    void on_label_4_linkActivated(const QString &link);// Слот по ссылке (профиль пользователя / «Гость»)
    //Геттер/сеттер текста информации, показываемого в окне Information
    QString get_info_text()const override {return info_text;}
    void set_info_text(const QString& text)  override { this->info_text = text;}
private:
    QString info_text;
private slots:

protected:
    QString backgroundImage()const override {return ":/img/52_17.jpg";}// Путь к фону в ресурсах Qt
    QString windowTitleText() const override{return "52/17 mode";}// Заголовок окна
    QLabel* avatarLabel() const override { return ui->label_5; }// аватар пользователя
    QLabel* userLabel()   const override { return ui->label_4; }// имя пользователя (ссылка)
    QLabel* get_info_link() const override{ return ui->label_7;}//ссылка «Информация»
    QComboBox* activityComboBox() const override{return ui->comboBox;}//комбобокс с выбором задач
    QLabel* get_activity_label() const override{return ui->label_2;}//заголовок над комбобоксом
private:
    Ui::Form52_17 *ui;

};

#endif // FORM52_17_H
