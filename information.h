#ifndef INFORMATION_H
#define INFORMATION_H

#include <QWidget>

namespace Ui {
class Information;
}

class Information : public QWidget
{
    Q_OBJECT

public:
    Information(QWidget *parent = nullptr)=delete;// Запрещен конструктор без текста
    explicit Information(QWidget *parent,const QString& text);
    ~Information();
    void set_text(const QString& arg){text = arg;}// Сеттер текста
    void set_parent_window(QWidget* window){parent_window = qobject_cast<QWidget*>(window);}//окно из которого открыли
    void paintEvent(QPaintEvent *event)override;//фон
    void getback_clicked();  // Возврат на предыдущее окно
    void on_pushButton_clicked();//слот по кнопке назад
private:
    Ui::Information *ui;
    QString text;
    QWidget* parent_window = nullptr;
};

#endif // INFORMATION_H
