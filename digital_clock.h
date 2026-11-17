#ifndef DIGITAL_CLOCK_H
#define DIGITAL_CLOCK_H

#include "myclock.h"
#include <QLCDNumber>
#include <QWidget>
#include <QTime>
#include <QTimer>
#include <QElapsedTimer>

class Data_Manager;

class digital_clock : public QLCDNumber
{
    Q_OBJECT
public:
    explicit digital_clock(QWidget *parent = nullptr);
    ~digital_clock() {SPDLOG_INFO("Электронные часы удалены");}
    qint64 get_timer_time() const { return left_time; }//геттер оставшегося времени

    bool isShowSeconds() const { return showSeconds; }//проверка показа секунд
    bool is24HourFormat() const { return _24HourFormat; }//проверка формата
    bool is_running()const {return running;}//возврат флага работы
    void setMode(FTR::ClockMode newMode);//настройка режима
    FTR::ClockMode getMode() const { return mode; }//показ режима
    void setShowSeconds(bool show);//показ секунд
    void set24HourFormat(bool format24);//настройка 24 формата
    void setColor(const QColor &color);//настройка цвета
    void stopwatch_start();//старт секундомера
    void stopwatch_stop();//стоп секундомера
    void stopwatch_pause();//пауза секундомера
    void timer_start(unsigned hours, unsigned min, unsigned sec);//старт таймера
    void timer_stop();//стоп таймера
    void timer_pause();//пауза таймера
    void set_timer_time(unsigned hours, unsigned min, unsigned sec);//настройка времени для таймера
    qint64 get_all_time() const;// Получить общее накопленное время
    void   reset_all_time();// Сбросить общий накопитель в 0
    void setEditable(bool editable) { this->isEditable = editable; }// Разрешить/запретить редактирование
signals:
    void timerFinished();          // Сигнал: таймер дошёл до нуля (испускается из updateDisplay)
    void timeChanged(int seconds); // Сигнал: время изменено вручную (испускается из endEdit после ввода)
private slots:
    void updateDisplay();//Слот: обновление цифр на дисплее.
protected:
    void mouseDoubleClickEvent(QMouseEvent* event)override;//обработчик 2го клика по таймеру
    void keyPressEvent(QKeyEvent* event) override;//обработчик нажатия клавиш
    bool isBufferValid(const QString& buf) const;//проверка валидности буфера для отображения цифр
private:
    bool editing = false;// Флаг: сейчас идёт ручной ввод времени
    QString edit_buffer; // Буфер введённых цифр (до 6 символов)
    void beginEdit();// Войти в режим редактирования (остановить таймер, установить фокус, подсветить)
    void endEdit(bool apply);// Выйти из режима редактирования. apply = true — применить введённое время
    void refreshEditDisplay();// Обновить дисплей во время редактирования (показать edit_buffer)
    QString formatBuffer()const; // Преобразовать edit_buffer в строку "hh:mm:ss"
    bool showSeconds  = true;//показ секунд
    bool _24HourFormat = true;//формат
    QColor color      = Qt::white;//цвет
    FTR::ClockMode mode = FTR::watch;//режим по умолчанию
    QTimer *obj = nullptr;  // Таймер для периодического обновления дисплея (интервал 250 мс)
    unsigned hour = 0;// Часы (для вывода на дисплей)
    unsigned min  = 0;// Минуты
    unsigned sec  = 0;// Секунды
    bool          running   = false;// Запущен ли сейчас отсчёт (секундомер или таймер)
    qint64        mode_time = 0;   // для секундомера: накопленное
    QElapsedTimer elapsed; // Точный таймер для измерения времени с момента старта/продолжения
    qint64        all_time  = 0;   // общий накопитель
    qint64        left_time = 0;   // для таймера: остаток
    qint64        timer_time= 0;   // для таймера: заданное
private:
    bool isEditable = true;//флаг редактирования таймера
};

#endif // DIGITAL_CLOCK_H
