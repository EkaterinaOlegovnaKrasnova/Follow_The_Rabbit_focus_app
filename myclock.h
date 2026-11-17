#ifndef MYCLOCK_H
#define MYCLOCK_H

#include "spdlog/spdlog.h"
#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QtMath>

namespace FTR{
enum ClockMode { watch, stopwatch, timer };
enum TimerMode{FlowMode,TimeBlocking,EatTheFrog,Pomodoro,ReversePomodoro,
                 Mode60_10,Mode52_17,Mode90_20};
enum EatTheFrog_dificulties{easy,simple,medium,hard,frog};
};


class MyClock : public QWidget
{
    Q_OBJECT
public:
    explicit MyClock(QWidget *parent = nullptr);
    ~MyClock() override {}
    void setMode(FTR::ClockMode mode);// Переключить режим (watch / stopwatch / timer)
    FTR::ClockMode get_mode() const {// Геттер текущего режима
        SPDLOG_TRACE("Режим {}", static_cast<int>(mode));
        return mode; }
    // Секундомер
    void stopwatch_start();// Запустить секундомер (с нуля или с накопленного sw_baseMs)
    void stopwatch_pause();// Поставить на паузу (сохраняет sw_baseMs)
    void stopwatch_stop();// Остановить и обнулить в 0 (но общее время остаётся накопленным)
    void resetStopwatch();// Сбросить секундомер в 0 (если не запущен — то же самое)
    //Таймер
    void timer_start(unsigned hours, unsigned min, unsigned sec);// Запустить таймер на заданное время
    void timer_pause();// Поставить на паузу (уменьшает tm_leftMs на прошедшее время)
    void timer_stop();// Остановить и вернуть к заданному времени (tm_totalMs)
    void reset_timer();// Вернуть к заданному времени без остановки
    void set_timer_time(unsigned hours, unsigned min, unsigned sec);// Установить время таймера без запуска
    // Общее время активности . сбрасывается resetTotal().
    qint64 get_totalMs() const;// Получить общее накопленное время (сумма всех сессий, мс)
    void resetTotal();// Сбросить общий накопитель в 0
    qint64 tm_displayLeftMs = 0;// Оставшееся время таймера для отрисовки стрелки прогресса(используется в drawHands)
protected:
    void paintEvent(QPaintEvent *event) override; //циферблат и стрелки
private slots:
    void update_time();//обновление текущего времени и перерисовка,вызывается QTimer каждые 250 мс
private:
    void drawFace(QPainter &painter);//Нарисовать циферблат: круг, деления, центр
    void drawHands(QPainter &painter);// Нарисовать стрелки (для timer — одну стрелку прогресса)
    void startTotal(); // Запустить накопление общего времени (если ещё не запущено)
    void pauseTotal();// Приостановить накопление, сохранив прошедший отрезок в totalMs
private:
    int hour = 0;//Часы
    int minute = 0;// Минуты
    int second = 0;// Секунды
    QTimer *obj = nullptr;// Таймер для периодического обновления (интервал 250 мс)
    FTR::ClockMode mode = FTR::ClockMode::watch;// Текущий режим (по умолчанию — обычные часы)
    // секундомер
    bool sw_running = false;// Запущен ли секундомер
    qint64 sw_baseMs = 0;// Накопленное время между паузами (мс)
    QElapsedTimer sw_elapsed;// Точный таймер для измерения времени с момента старта/продолжения
    // таймер
    bool tm_running = false;// Запущен ли таймер
    qint64 tm_totalMs = 0;// Изначально заданное время таймера (мс)
    qint64 tm_leftMs  = 0;// Сколько осталось до нуля (мс)
    QElapsedTimer tm_elapsed;// Точный таймер для измерения времени с момента старта/продолжения
    // общее время
    qint64 totalMs = 0;// Общее накопленное время (сумма всех сессий, мс)
    QElapsedTimer totalElapsed; // Точный таймер для измерения текущего отрезка
    bool totalRunning = false;// Идёт ли сейчас накопление
    QColor c = QColor::fromRgbF(1.0, 1.0, 1.0, 0.9);// Белый (стрелки, деления) с прозрачностью 0.9
    QColor c1 = QColor::fromRgbF(0.0, 0.0, 0.0, 0.3);// Чёрный (фон циферблата) с прозрачностью 0.3
};

#endif // MYCLOCK_H
