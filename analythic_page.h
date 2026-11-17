#ifndef ANALYTHIC_PAGE_H
#define ANALYTHIC_PAGE_H

#include "user_settings.h"
#include <QWidget>

namespace Ui {
class Analythic_page;
}
class profileW;
class  Data_Manager;

class Analythic_page : public QWidget
{
    Q_OBJECT

public:
    explicit Analythic_page(QWidget *parent = nullptr);
    ~Analythic_page();
    void get_back();//метод возврата на предыдущее окно
    void paintEvent(QPaintEvent* )override;//отрисовка
    void set_profileW(profileW* window){this->window = window;}//геттер для возврата  предыдущего окна
    void set_profile();//настройка страницы аналитики для конкретного пользователя
    void loadUserProfile();//загрузка режима пользователя
    void updateAvatar();//загрузка аватара
    profileW* window;
private slots:
    void on_comboBox_currentIndexChanged(int index);//слот комбобокса с задачами
private:
    void fill_tasks_combo();                 //заполнение комбобокса
    void refresh_analytics(qint64 task_id);  //пересчёт и вывод
private slots:
    void on_pushButton_clicked();//слот кнопки назад
protected:
    std::shared_ptr<User_Profile> me = nullptr;
    void showEvent(QShowEvent* event) override;//обработчик для пересчета аналитики
private:
    Ui::Analythic_page *ui;
    user_settings* settings =nullptr;
    QPixmap pic;
};

#endif // ANALYTHIC_PAGE_H
