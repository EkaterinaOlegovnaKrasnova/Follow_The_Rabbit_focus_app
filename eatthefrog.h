#ifndef EATTHEFROG_H
#define EATTHEFROG_H

#include "basetracker.h"
#include "qboxlayout.h"
#include "qpushbutton.h"
#include "ui_eatthefrog.h"

namespace Ui {
class EatTheFrog;
}

class EatTheFrog : public BaseTracker
{
    Q_OBJECT

public:
    explicit EatTheFrog(QWidget *parent = nullptr);
    ~EatTheFrog()override;
    void on_label_4_linkActivated(const QString &link);// Слот по ссылке (профиль пользователя / «Гость»)
    //Геттер/сеттер текста информации, показываемого в окне Information
    QString get_info_text()const override {return info_text;}
    void set_info_text(const QString& text)  override { this->info_text = text;}
private:
    QString info_text;
protected:
    QString backgroundImage() const override { return ":/img/background2.1.jpeg"; }// Путь к фону в ресурсах Qt
    QString windowTitleText()  const override { return "EatTheFrog mode"; }// Заголовок окна
    QLabel* avatarLabel() const override { return ui->label_5; }// аватар пользователя
    QLabel* userLabel()   const override { return ui->label_4; }// имя пользователя (ссылка)
    QLabel* get_info_link() const override{ return ui->label_7;}//ссылка «Информация»
    // В режиме EatTheFrog нет комбобокса выбора активности,возврат nullptr.
    //Базовый код проверяет указатель и пропускает работу.
    QComboBox* activityComboBox() const override{return nullptr;}
    QLabel* get_activity_label() const override{return nullptr;}






private:
    Ui::EatTheFrog *ui;
};

#endif // EATTHEFROG_H
