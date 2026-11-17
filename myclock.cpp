#include "myclock.h"
#include "spdlog/spdlog.h"
#include <QPainter>
#include <QTime>
using namespace FTR;
MyClock::MyClock(QWidget *parent)
    : QWidget(parent)
{
    obj = new QTimer(this);
    connect(obj, &QTimer::timeout, this, &MyClock::update_time);
    obj->start(250);
    update_time();
    SPDLOG_TRACE("Аналоговые часы созданы");
}

// ---------------- paint ----------------

void MyClock::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    drawFace(painter);
    drawHands(painter);

}

void MyClock::drawFace(QPainter &painter)
{
    int side = qMin(width(), height());
    painter.setViewport((width() - side) / 2, (height() - side) / 2, side, side);
    painter.setWindow(-51, -51, 102, 102);
    painter.setBrush(c1);
    painter.drawEllipse(-50, -50, 100, 100);

    painter.setPen(QPen(c, 2));
    painter.drawEllipse(-48, -48, 96, 96);

    for (int i = 1; i <= 12; ++i) {
        double angle = (i * 30 - 90) * M_PI / 180;
        painter.drawLine(38 * cos(angle), 38 * sin(angle),
                         45 * cos(angle), 45 * sin(angle));
    }
    for (int i = 1; i <= 60; ++i) {
        if (i % 5 == 0) continue;
        double angle = (i * 6 - 90) * M_PI / 180;
        painter.drawLine(42 * cos(angle), 42 * sin(angle),
                         45 * cos(angle), 45 * sin(angle));
    }

    painter.setBrush(c);
    painter.drawEllipse(-3, -3, 6, 6);

}

void MyClock::drawHands(QPainter &painter)
{
    int side = qMin(width(), height());
    painter.setViewport((width() - side) / 2, (height() - side) / 2, side, side);
    painter.setWindow(-50, -50, 100, 100);
    // ── ТАЙМЕР: одна стрелка прогресса по часовой ──
    if (mode == FTR::timer) {

        double progress = 0.0;
        if (tm_totalMs > 0) {
            progress = 1.0 - double(tm_displayLeftMs) / double(tm_totalMs);
            if (progress < 0.0) progress = 0.0;
            if (progress > 1.0) progress = 1.0;
        }

        // угол по часовой от 12 часов
        double angle = progress * 2.0 * M_PI;

        painter.setPen(QPen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(0, 0,
                         33 * sin(angle),
                         -33 * cos(angle));

        double minuteAngle = progress * 6 * M_PI / 180;
         painter.setPen(QPen(Qt::white, 3, Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(0, 0, 34 * sin(minuteAngle), -34 * cos(minuteAngle));
        painter.setPen(QPen(QColor::fromRgbF(0.0,0.0,0.0,0.7), 1, Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(0, 0, 34 * sin(minuteAngle), -34 * cos(minuteAngle));

        return;
    }

    double hourAngle = (hour % 12 + minute / 60.0) * 30 * M_PI / 180;
    painter.setPen(QPen(Qt::white, 5, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(0, 0, 17 * sin(hourAngle), -20 * cos(hourAngle));
    painter.setPen(QPen(QColor::fromRgbF(0.0,0.0,0.0,0.7), 2, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(0, 0, 17 * sin(hourAngle), -20 * cos(hourAngle));

    double minuteAngle = (minute + second / 60.0) * 6 * M_PI / 180;
    painter.setPen(QPen(Qt::white, 3, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(0, 0, 34 * sin(minuteAngle), -34 * cos(minuteAngle));
    painter.setPen(QPen(QColor::fromRgbF(0.0,0.0,0.0,0.7), 1, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(0, 0, 34 * sin(minuteAngle), -34 * cos(minuteAngle));

    double secondAngle = second * 6 * M_PI / 180;
    painter.setPen(QPen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(0, 0, 32 * sin(secondAngle), -32 * cos(secondAngle));

}

// ---------------- update ----------------

void MyClock::update_time()
{
    switch (mode) {
    case watch: {
        QTime c = QTime::currentTime();
        hour   = c.hour();
        minute = c.minute();
        second = c.second();
        break;
    }
    case stopwatch: {
        qint64 ms = sw_baseMs + (sw_running ? sw_elapsed.elapsed() : 0);
        hour   = ms / 3600000;
        minute = (ms % 3600000) / 60000;
        second = (ms % 60000) / 1000;
        break;
    }
    case timer: {
        qint64 left = tm_leftMs - (tm_running ? tm_elapsed.elapsed() : 0);
        if (left < 0) left = 0;
        tm_displayLeftMs = left;
        hour   = left / 3600000;
        minute = (left % 3600000) / 60000;
        second = (left % 60000) / 1000;
        break;
    }
    }
    update();
}

// ---------------- mode ----------------

void MyClock::setMode(ClockMode m)
{
    mode = m;
    switch (mode) {
    case watch:
        update_time();
        SPDLOG_TRACE("Выбран режим часов ");
        break;
    case stopwatch:
        resetStopwatch();
        SPDLOG_TRACE("Выбран режим секундомера ");
        break;
    case timer:
        reset_timer();
        SPDLOG_TRACE("Выбран режим таймера ");
        break;
    }
}

// ---------------- секундомер ----------------

void MyClock::stopwatch_start()
{
    if (sw_running) {
        SPDLOG_ERROR("Секундомер уже активен");
        return;}
    sw_running = true;
    sw_elapsed.start();
    startTotal();
    update_time();
    SPDLOG_TRACE("Секундомер запущен ");
}

void MyClock::stopwatch_pause()
{
    if (!sw_running) {
        SPDLOG_ERROR("Секундомер не активен");
        return;}
    sw_baseMs += sw_elapsed.elapsed();
    sw_running = false;
    pauseTotal();
    update_time();
    SPDLOG_TRACE("Секундомер на паузе");
}

void MyClock::stopwatch_stop()
{
    // обнуляем секундомер в ноль
    sw_running = false;
    sw_baseMs  = 0;
    hour = minute = second = 0;

    // фиксируем общий кусок, но НЕ сбрасываем накопленное
    pauseTotal();
    update();
    SPDLOG_TRACE("Секундомер остановлен.");
}

void MyClock::resetStopwatch()
{
    sw_running = false;
    sw_baseMs  = 0;
    hour = minute = second = 0;
    SPDLOG_TRACE("Данные секундомера сброшены.");
    update();

}

// ---------------- таймер ----------------

void MyClock::timer_start(unsigned hours, unsigned min, unsigned sec)
{
    if (tm_running) {SPDLOG_ERROR("Таймер активен. Выходим");
        return;}

    if (tm_leftMs == 0) {
        tm_totalMs = (qint64)hours * 3600000
                     + (qint64)min   * 60000
                     + (qint64)sec   * 1000;
        tm_leftMs  = tm_totalMs;
        SPDLOG_DEBUG("Таймер инициализирован. "
                     "Установлено общее время: {} мс ({} ч {} мин {} сек)",
                     tm_totalMs, hours, min, sec);
    }

    tm_elapsed.start();
    tm_running = true;
    startTotal();
    update_time();
    SPDLOG_TRACE("Таймер запущен.");
}

void MyClock::timer_pause()
{
    if (!tm_running)  {
        SPDLOG_ERROR("Таймер не активен");
        return;}
    tm_leftMs -= tm_elapsed.elapsed();
    if (tm_leftMs < 0) tm_leftMs = 0;
    tm_running = false;
    pauseTotal();
    update_time();
    SPDLOG_TRACE("Таймер на паузе");
}

void MyClock::timer_stop()
{

    tm_running = false;
    tm_leftMs  = tm_totalMs;
    tm_displayLeftMs = tm_leftMs;
    hour = minute = second = 0;
    pauseTotal();

    update();
    SPDLOG_TRACE("Таймер остановлен. Возвращен к первоначальному времени отсчета");
}

void MyClock::reset_timer()
{
    tm_running = false;
    tm_leftMs  = tm_totalMs;
    tm_displayLeftMs = tm_leftMs;
    update_time();
    SPDLOG_TRACE("Таймер перустановлен");
}

void MyClock::set_timer_time(unsigned hours, unsigned min, unsigned sec)
{
    if (hours > 23 || min > 59 || sec > 59) {
        SPDLOG_ERROR("Часы / минуты / секунды не валидны");
        return;}
    tm_totalMs = (qint64)hours * 3600000
                 + (qint64)min   * 60000
                 + (qint64)sec   * 1000;


    tm_leftMs  = tm_totalMs;
    tm_displayLeftMs = tm_leftMs;
    tm_running = false;
    update_time();
    SPDLOG_TRACE("Таймер инициализирован {} мс ({} ч {} мин {} сек)",
                 tm_totalMs, hours, min, sec);
}

// ---------------- общее время ----------------

void MyClock::startTotal()
{
    if (totalRunning){
        SPDLOG_TRACE("Таймер уже работает. Выход");
        return;
    }
    totalRunning = true;
    totalElapsed.start();
    SPDLOG_DEBUG("Запуск накопления общего времени. "
                 "Накоплено ранее: {} мс", totalMs);
}

void MyClock::pauseTotal()
{
    if (!totalRunning) {
        SPDLOG_TRACE("Таймер не активен. Выход");
        return;}
    totalMs += totalElapsed.elapsed();
    totalRunning = false;
    SPDLOG_DEBUG("Пауза. Текущий отрезок: {} мс. Всего накоплено: {} мс",
                 totalElapsed.elapsed(), totalMs);
}

qint64 MyClock::get_totalMs() const
{
    const qint64 currentTime =  totalMs + (totalRunning ? totalElapsed.elapsed() : 0);
    SPDLOG_TRACE("Общее время {}",currentTime);
    return currentTime;
}

void MyClock::resetTotal()
{
    SPDLOG_DEBUG("Сброс общего времени. Было накоплено: {} мс", totalMs);
    totalRunning = false;
    totalMs = 0;
    SPDLOG_TRACE("Общее время успешно сброшено в 0");
}
