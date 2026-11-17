#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "data_manager.h"
#include "information.h"
#include "myclock.h"
#include "digital_clock.h"
#include "profilew.h"

class NEW_USER_FORM;
class Data_Manager;
class USERS_FORM;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setOpenWindow(QWidget* openWindow) { this->openWindow = openWindow; }// Сеттер указателя на стартовое окно
    void loadUserProfile();// Загрузка аватара текущего пользователя
    void loadCurrentUser();// Загрузка имени пользователя в label_3
signals:
    void openProject(const QString &mode);// Сигнал с параметром режима
private slots:
    void pomodoro_clicked();// Клик по Pomodoro: создаёт Pomodoro_mode, задаёт текст
    void pomodoro_reverse_clicked();// Клик по ReversePomodoro: создаёт reverse_pomodoro
    void flow_mode_clicked();// Клик по «FlowMode»: создаёт Flow mode
    void mode_60_10_clicked();// Клик по «60/10»: создаёт Form60_10
    void mode_52_17_clicked();// Клик по «52/17»: создаёт Form52_17
    void mode_90_20_clicked();// Клик по «90/20»: создаёт Form90_20
    void getback_clicked();// Клик НАЗАД
    void time_blocking_clicked();// Клик по «TimeBlocking»: создаёт time_blocking
    void eatthefrog_clicked();// Клик по «EatTheFrog»: создаёт EatTheFrog
    void on_label_3_linkActivated(const QString &link);// Клик по ссылке имя пользователя / «Гость»
    void on_label_4_linkActivated(const QString &link);// Открывает окно Information с текстом о приложении
private:
    Ui::MainWindow *ui;
    QWidget* openWindow = nullptr;// Указатель на стартовое окно для кнопки назад
    MyClock *analogClock;      // Аналоговые часы
    digital_clock *digitalClock; // Электронные часы
    profileW* profileWindow = nullptr;// Указатель на окно профиля.
    User_Profile me;// Профиль текущего пользователя (загружается в loadUserProfile)
    NEW_USER_FORM* registration = nullptr;// Указатель на окно регистрации
    USERS_FORM* autorisation = nullptr;// Указатель на окно авторизации
    Information* infoWindow = nullptr;// Указатель на окно с информацией о приложении
    QDialog* loginDialog = nullptr; // Указатель на диалог выбора действия (Регистрация/Авторизация/Отмена)
protected:
    void resizeEvent(QResizeEvent *event) override;// При изменении размера окна — перерисовка фона (update)
    void paintEvent(QPaintEvent *event)override;// Отрисовка фона: загрузка и масштабирование
    void showEvent(QShowEvent *event) override;// При каждом показе окна — обновление имени и аватара
};

#endif // MAINWINDOW_H
