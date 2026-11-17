#ifndef PROFILEW_H
#define PROFILEW_H

#include "qboxlayout.h"
#include "user_settings.h"
#include <QWidget>

namespace Ui {
class profileW;
}

class profileW : public QWidget
{
    Q_OBJECT

public:
    explicit profileW(QWidget *parent = nullptr);
    ~profileW();
    void paintEvent(QPaintEvent *) override;//отрисовка она
    void setOpenWindow(QWidget* openWindow) { this->openWindow = openWindow; }//настройка предыдущего окна
    void getback_clicked();//вохврат назад
    // Загрузка данных текущего пользователя в поля интерфейса.Для гостя — заглушка и блокировка кнопок;
    // для пользователя — имя, «о себе», аватар и список задач
    void loadUserProfile();
    void resizeEvent(QResizeEvent *event) override;// Переопределение: при изменении размера окна
    void updateAvatar();//аватар
    std::shared_ptr<User_Profile> get_me(){return this->me;}
    void loadTasks();   // загрузка задач в скролл-арию
    void showEvent(QShowEvent* event) override;//перезагрузка окна,чтобы были видны обновления
signals:
    void profileUpdated(); // Сигнал: профиль обновлён (после сохранения).
private slots:
    void on_pushButton_3_clicked();//слот назад
    void onChangeAvatarClicked();//слот изменение фотографии
    void onSaveClicked();//слот сохранение
    void on_pushButton_2_clicked();//слот перехлд в настройки аккаунта
    void on_pushButton_6_clicked() ;//слот выйти из профиля
    void on_pushButton_4_clicked();//слот переход на окно аналитика
protected:
    std::shared_ptr<User_Profile> me = nullptr;// Текущий профиль пользователя.
private:
    Ui::profileW *ui;
    QWidget* openWindow = nullptr;   // Окно, из которого открыли профиль.

    QByteArray pendingAvatar;// Новый аватар, выбранный пользователем,
    bool       avatarChanged = false; // Флаг: пользователь выбрал новый аватар.
    void showQuestionMark();// Показать заглушку «?» вместо аватара
    user_settings* settings =nullptr;   // Указатель на окно настроек аккаунта.
    void clearTasksLayout();  // очистка старых лейблов
    QVBoxLayout* tasksLayout() const;  // удобный доступ к layout скролла
};

#endif // PROFILEW_H
