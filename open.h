#ifndef OPEN_H
#define OPEN_H

#include "qtmetamacros.h"
#include <QWidget>

class MainWindow;

namespace Ui {
class open;
}

class open : public QWidget
{
    Q_OBJECT
    friend class MainWindow;
public:
    explicit open(QWidget *parent = nullptr);
    ~open();
private slots:
    void on_pushButton_2_clicked();// Слот: гостевой режим,испускает сигнал openproject() и скрывает окно
    void on_exit_clicked();// Слот: клик по exit - выход из приложения
    void on_new_user_clicked(); // Слот: клик по new_user - открытие формы регистрации.
    void on_choose_user_clicked();// Слот: клик по choose_user - открытие формы авторизации.
signals:
    void openproject();// Сигнал: «открыть проект» (перейти к главному окну).
private:
    Ui::open *ui;
protected:
    void paintEvent(QPaintEvent *) override;//фон
};

#endif // OPEN_H
