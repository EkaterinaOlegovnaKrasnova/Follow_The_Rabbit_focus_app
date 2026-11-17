#ifndef USER_SETTINGS_H
#define USER_SETTINGS_H
#include "data_manager.h"
#include <QWidget>

class MainWindow;
class profileW;

namespace Ui {
class user_settings;
}

class user_settings : public QWidget
{
    Q_OBJECT

public:
    explicit user_settings(QWidget *parent = nullptr);
    ~user_settings();
    void paintEvent(QPaintEvent *) override;// фон
    void setMainWindow(MainWindow* mainWindow) { m_mainWindow = mainWindow; } // Сеттер указателя на главное окно
    void getback_clicked(); // Возврат на предыдущее окно
    MainWindow* m_mainWindow = nullptr;
    void set_me(std::shared_ptr<User_Profile> user); // Установка профиля текущего пользователя
    void loadCurrentUser(); // Загрузка данных пользователя
    void updateAvatar(); //аватар
protected:
    std::shared_ptr<User_Profile> me = nullptr;// Установка профиля текущего пользователя
    void resizeEvent(QResizeEvent *event) override;
signals:
    void profileUpdated();// Сигнал: профиль обновлён
private slots:
    void on_pushButton_clicked();//слот кнопки назад
    void on_lineEdit_textChanged(const QString &arg1);//слот изменение имени
    void on_pushButton_2_clicked();//слот кнопки сохрагить
    void on_pushButton_3_clicked();//слот кнопки изменить аватар
private:
    Ui::user_settings *ui;
};

#endif // USER_SETTINGS_H
