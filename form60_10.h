#ifndef FORM6__1__H
#define FORM6__1__H

#include "digital_clock.h"
#include "myclock.h"
#include <QWidget>
#include "basetracker.h"
#include "ui_form60_10.h"
namespace Ui {
class Form60_10;
}

class Form60_10 : public BaseTracker
{
    Q_OBJECT

public:
    explicit Form60_10(QWidget *parent = nullptr);
    ~Form60_10();
    void on_label_4_linkActivated(const QString &link);// Слот по ссылке (профиль пользователя / «Гость»)
    //Геттер/сеттер текста информации, показываемого в окне Information
    QString get_info_text()const override {return info_text;}
    void set_info_text(const QString& text)  override { this->info_text = text;}
private:
    QString info_text;
protected:
    QString windowTitleText() const override{return "60/10 mode";}// Заголовок окна
    QString backgroundImage()const override {return ":/img/60_10.jpg";}// Путь к фону в ресурсах Qt
    QLabel* avatarLabel() const override { return ui->label_5; }// аватар пользователя
    QLabel* userLabel()   const override { return ui->label_4; }// имя пользователя (ссылка)
     QLabel* get_info_link() const override{ return ui->label_7;}//ссылка «Информация»
    QComboBox* activityComboBox() const override{return ui->comboBox;}//комбобокс с выбором задач
     QLabel* get_activity_label() const override{return ui->label_2;}//заголовок над комбобоксом
private:
    Ui::Form60_10 *ui;

};

#endif // FORM6__1__H
