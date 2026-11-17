#include "digital_clock.h"
#include "qapplication.h"
#include <QPalette>
#include <QMouseEvent>
using namespace FTR;

digital_clock::digital_clock(QWidget *parent)
    : QLCDNumber(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    obj = new QTimer(this);
    connect(obj, &QTimer::timeout, this, &digital_clock::updateDisplay);
    obj->start(250);
    updateDisplay();
    SPDLOG_INFO("Электронные часы созданы");
}

// секундомер
void digital_clock::stopwatch_start()
{
    if (running) {
        SPDLOG_INFO("stopwatch_start: Секундомер уже активен");
        return;}
    running = true;
    elapsed.start();
    updateDisplay();
    SPDLOG_TRACE("stopwatch_start: Секундомер запущен");
}

void digital_clock::stopwatch_pause()
{
    if (!running){
        SPDLOG_INFO("stopwatch_start: Секундомер не активен в данный момент");
        return;}
    mode_time += elapsed.elapsed();
    running = false;
    updateDisplay();
    SPDLOG_TRACE("stopwatch_start: Секундомер на паузе");
}

void digital_clock::stopwatch_stop()
{
    if (running) {
        SPDLOG_INFO("stopwatch_start: Секундомер активен, останавливаем его");
        mode_time += elapsed.elapsed();
        running = false;
    }
    all_time += mode_time;
    mode_time = 0;
    hour = min = sec = 0;
    updateDisplay();
    SPDLOG_TRACE("stopwatch_start: Секундомер остановлен, сброс показателей");
}

// таймер
void digital_clock::timer_start(unsigned hours, unsigned min, unsigned sec)
{
    if (running){
        SPDLOG_INFO("timer_start: Таймер уже активен");
        return;
    }
    qint64 newTime = (qint64)hours * 3600000
                     + (qint64)min   * 60000
                     + (qint64)sec   * 1000;
    SPDLOG_DEBUG("timer_start: запрошено {:02}:{:02}:{:02} "
                 "= {} мс, текущий left_time = {} мс",
                 hours, min, sec, newTime, left_time);
    if (newTime > 0 || left_time == 0) {
        SPDLOG_DEBUG("timer_start: устанавливаем время "
                     "timer_time = left_time = {} мс", newTime);
        timer_time = newTime;
        left_time  = newTime;
    }else {
        SPDLOG_DEBUG("digital_clock::timer_start: newTime = 0, "
                     "но left_time = {} мс — оставляем как есть "
                     "(продолжение с паузы)", left_time);
    }
    elapsed.start();
    running = true;
    updateDisplay();
    SPDLOG_INFO("digital_clock::timer_start: таймер запущен, "
                "left_time = {} мс ({}:{:02}:{:02})",
                left_time,
                left_time / 3600000,
                (left_time / 60000) % 60,
                (left_time / 1000) % 60);
}

void digital_clock::timer_pause()
{
    if (!running) {
        SPDLOG_DEBUG("timer_pause: таймер не запущен");
        return;
    }
    const qint64 elapsedMs = elapsed.elapsed();
    const qint64 before    = left_time;
    left_time -= elapsedMs;
    if (left_time < 0) left_time = 0;
    running = false;
    updateDisplay();
    SPDLOG_INFO("timer_pause: пауза, "
                "elapsed = {} мс, left_time: {} -> {} мс ({}:{:02}:{:02})",
                elapsedMs, before, left_time,
                left_time / 3600000,
                (left_time / 60000) % 60,
                (left_time / 1000) % 60);
}

void digital_clock::timer_stop()
{
    if (running) {
        const qint64 elapsedMs = elapsed.elapsed();
        const qint64 before    = left_time;
        left_time -= elapsedMs;
        if (left_time < 0) left_time = 0;
        running = false;
        SPDLOG_DEBUG("timer_stop: остановка, "
                     "elapsed = {} мс, left_time: {} -> {} мс",
                     elapsedMs, before, left_time);
    } else {
        SPDLOG_DEBUG("timer_stop: таймер уже остановлен, "
                     "left_time = {} мс, timer_time = {} мс",
                     left_time, timer_time);
    }
    const qint64 passed = timer_time - left_time;
    all_time += passed;
    SPDLOG_INFO("timer_stop: завершён, "
                "timer_time = {} мс, left_time = {} мс, "
                "passed = {} мс, all_time = {} мс",
                timer_time, left_time, passed, all_time);
    left_time = timer_time;
    hour = min = sec = 0;
    updateDisplay();
    SPDLOG_DEBUG("timer_stop: сброс, "
                 "left_time = {} мс, hour = {}, min = {}, sec = {}",
                 left_time, hour, min, sec);
}

void digital_clock::set_timer_time(unsigned hours, unsigned min, unsigned sec)
{
    if (hours > 23 || min > 59 || sec > 59) {
        SPDLOG_ERROR("Часы / минуты / секунды не валидны");
        return;
    }
    timer_time = (qint64)hours * 3600000
                 + (qint64)min   * 60000
                 + (qint64)sec   * 1000;
    left_time = timer_time;
    running   = false;
    updateDisplay();
    SPDLOG_TRACE("Таймер инициализирован {} мс ({} ч {} мин {} сек)",
                timer_time, hours, min, sec);
}

void digital_clock::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (!isEditable) {
        SPDLOG_TRACE("digital_clock::mouseDoubleClickEvent: редактирование отключено, "
                     "button = {}", static_cast<int>(event->button()));
        QLCDNumber::mouseDoubleClickEvent(event);
        return;
    }
    SPDLOG_TRACE("digital_clock::mouseDoubleClickEvent: button = {}, mode = {}",
                 static_cast<int>(event->button()),
                 static_cast<int>(mode));
    if (event->button()==Qt::LeftButton && mode==FTR::timer){
        SPDLOG_DEBUG("digital_clock::mouseDoubleClickEvent: вход в режим "
                     "редактирования (mode = timer)");
        beginEdit();
        return;
    }
    SPDLOG_TRACE("digital_clock::mouseDoubleClickEvent: не левый клик "
                 "или не timer, проброс в QLCDNumber");

    QLCDNumber::mouseDoubleClickEvent(event);
}

void digital_clock::beginEdit()
{
    SPDLOG_DEBUG("digital_clock::beginEdit: hasFocus перед редактированием = {}",
                 hasFocus());
    if (editing)  {
        SPDLOG_TRACE("Режим редактирования уже подключен");
        return;}
    if (running) {
        timer_stop();
    }
    editing = true;
    edit_buffer.clear();
    setFocus(Qt::MouseFocusReason);
    setStyleSheet("background: rgba(255, 0, 0, 0.6);"
                  "border: 2px solid red;"
                  "border-radius: 8px;");
    SPDLOG_DEBUG("digital_clock::beginEdit: hasFocus после редактирования = {}",
                 hasFocus());
    refreshEditDisplay();
}

void digital_clock::endEdit(bool apply)
{
    if (!editing){
        SPDLOG_TRACE("Режим редактирования уже отключен");
        return;}
    SPDLOG_DEBUG("digital_clock::endEdit: выход из редактирования, "
                 "apply = {}, edit_buffer = '{}'",
                 apply, edit_buffer.toStdString());
    editing =false;
    setStyleSheet("background: transparent;");
    if (apply) {
        QString time  = edit_buffer;
        while (time.size()<6)time.prepend('0');
        if (time.size()>6)time = time.right(6);
        const int hh=time.mid(0,2).toInt();
        const int mm = time.mid(2,2).toInt();
        const int ss = time.mid(4,2).toInt();
        SPDLOG_DEBUG("digital_clock::endEdit: время {:02}:{:02}:{:02} "
                     "(из '{}')",
                     hh, mm, ss, time.toStdString());
        set_timer_time(hh,mm,ss);

        SPDLOG_DEBUG("digital_clock::endEdit: emit timeChanged = {} сек",
                     static_cast<int>(left_time / 1000));
        emit timeChanged(static_cast<int>(left_time / 1000));
    }
    else
    {
        SPDLOG_DEBUG("digital_clock::endEdit: отмена, обновляем дисплей");
        updateDisplay();
    }
}
bool digital_clock::isBufferValid(const QString& buf) const
{
    // buf — только цифры, длина 0..6
    auto pairValue = [&](int idx) -> int {
        // idx = 0,2,4 — начало пары
        if (buf.size() < idx + 2) return -1;   // пара ещё не набрана
        return buf.mid(idx, 2).toInt();
    };
    const int hh = pairValue(0);
    const int mm = pairValue(2);
    const int ss = pairValue(4);
    bool ok = true;
    if (hh >= 0 && hh > 23) ok = false;
    if (mm >= 0 && mm > 59) ok = false;
    if (ss >= 0 && ss > 59) ok = false;
    SPDLOG_TRACE("digital_clock::isBufferValid: строка = '{}' -> "
                 "hh = {}, mm = {}, ss = {} => {}",
                 buf.toStdString(), hh, mm, ss,
                 ok ? "валидный" : "невалидный");
    return ok ;
}

void digital_clock::refreshEditDisplay()
{
    QString shown = formatBuffer();
    int digits = shown.size();
    if (digitCount() != digits) setDigitCount(digits);
    display(shown);
}

QString digital_clock::formatBuffer() const
{
    // m_editBuffer — только цифры
    QString b = edit_buffer;
    while (b.size() < 6) b.prepend('0');

    QString out;
    out += b.mid(0, 2);
    out += ':';
    out += b.mid(2, 2);
    out += ':';
    out += b.mid(4, 2);
    return out;
}

void digital_clock::keyPressEvent(QKeyEvent *event)
{
    if (!editing) {
        QLCDNumber::keyPressEvent(event);
        return;
    }
    const int key = event->key();
    SPDLOG_TRACE("digital_clock::keyPressEvent: key = {} (0x{:X}), "
                 "edit_buffer = '{}'",
                 key, key, edit_buffer.toStdString());
    if (key >= Qt::Key_0 && key <= Qt::Key_9) {
        if (edit_buffer.size() < 6) {
            const QChar digit = QChar('0' + (key - Qt::Key_0));
            const QString candidate = edit_buffer + digit;
            if (isBufferValid(candidate)) {
                edit_buffer = candidate;
                refreshEditDisplay();
                SPDLOG_TRACE("digital_clock::keyPressEvent: цифра '{}' принята, "
                             "edit_buffer = '{}'", digit.toLatin1(),
                             edit_buffer.toStdString());
            } else {
                SPDLOG_TRACE("digital_clock::keyPressEvent: цифра '{}' "
                             "отклонена (невалидный буфер '{}'), beep",
                             digit.toLatin1(), candidate.toStdString());
                QApplication::beep();
            }
        } else {
            SPDLOG_TRACE("digital_clock::keyPressEvent: буфер полон (6 цифр), "
                         "цифра '{}' проигнорирована",
                         QChar('0' + (key - Qt::Key_0)).toLatin1());
        }
        event->accept();
        return;
    }
    if (key == Qt::Key_Backspace) {
        const bool hadDigits = !edit_buffer.isEmpty();
        if (hadDigits)
            edit_buffer.chop(1);
        refreshEditDisplay();
        SPDLOG_TRACE("digital_clock::keyPressEvent: Backspace, "
                     "edit_buffer = '{}' ({})",
                     edit_buffer.toStdString(),
                     hadDigits ? "удалена цифра" : "буфер уже пуст");
        event->accept();
        return;
    }
    if (key == Qt::Key_Return || key == Qt::Key_Enter) {
        SPDLOG_DEBUG("digital_clock::keyPressEvent: Enter, "
                     "применяем edit_buffer = '{}'",
                     edit_buffer.toStdString());
        endEdit(true);
        event->accept();
        return;
    }
    if (key == Qt::Key_Escape) {
        SPDLOG_DEBUG("digital_clock::keyPressEvent: Escape, "
                     "отмена редактирования (edit_buffer = '{}')",
                     edit_buffer.toStdString());
        endEdit(false);
        event->accept();
        return;
    }
    SPDLOG_TRACE("digital_clock::keyPressEvent: клавиша {} проигнорирована",
                 key);
    event->accept();
}

//отображение
void digital_clock::updateDisplay()
{
    if (editing) return;
    QString displaystr;
    switch (mode) {
    case FTR::watch: {
        QTime time = QTime::currentTime();
        QString format = _24HourFormat
                             ? (showSeconds ? "hh:mm:ss" : "hh:mm")
                             : (showSeconds ? "hh:mm:ss AP" : "hh:mm AP");
        displaystr = time.toString(format);
        break;
    }
    case FTR::stopwatch: {
        qint64 ms = mode_time + (running ? elapsed.elapsed() : 0);
        hour = ms / 3600000;
        min  = (ms % 3600000) / 60000;
        sec  = (ms % 60000) / 1000;
        displaystr = QString("%1:%2:%3")
                         .arg(hour, 2, 10, QChar('0'))
                         .arg(min,  2, 10, QChar('0'))
                         .arg(sec,  2, 10, QChar('0'));
        break;
    }
    case FTR::timer: {
        qint64 left = left_time - (running ? elapsed.elapsed() : 0);
        if (left < 0) left = 0;

        if (running && left == 0) {
            running = false;
            SPDLOG_INFO("digital_clock::updateDisplay: таймер дошёл до нуля, "
                        "emit timerFinished()");
            emit timerFinished();
        }

        hour = left / 3600000;
        min  = (left % 3600000) / 60000;
        sec  = (left % 60000) / 1000;
        displaystr = QString("%1:%2:%3")
                         .arg(hour, 2, 10, QChar('0'))
                         .arg(min,  2, 10, QChar('0'))
                         .arg(sec,  2, 10, QChar('0'));
        break;
    }
    }

    int neededDigits = showSeconds ? 8 : 5;
    if (!_24HourFormat)
        neededDigits = showSeconds ? 11 : 8;

    if (digitCount() != neededDigits)
        setDigitCount(neededDigits);

    display(displaystr);
}

//режим
void digital_clock::setMode(FTR::ClockMode newMode)
{
    if (mode == newMode) {
        SPDLOG_TRACE("digital_clock::setMode: режим уже {},",
                     static_cast<int>(newMode));
        return;
    }
    SPDLOG_DEBUG("digital_clock::setMode: смена режима {} -> {}",
                 static_cast<int>(mode), static_cast<int>(newMode));
    switch (mode) {
    case FTR::stopwatch:
        SPDLOG_DEBUG("digital_clock::setMode: останавливаем секундомера");
        stopwatch_stop();
        break;
    case FTR::timer:
        SPDLOG_DEBUG("digital_clock::setMode: останавливаем таймер");
        timer_stop();
        break;
    case FTR::watch:
        break;
    }
    mode = newMode;
    updateDisplay();
}

//настройки
void digital_clock::setShowSeconds(bool show)
{
    SPDLOG_DEBUG("digital_clock::setShowSeconds: showSeconds = {}", show);
    showSeconds = show;
    updateDisplay();
}

void digital_clock::set24HourFormat(bool format24)
{
    SPDLOG_DEBUG("digital_clock::set24HourFormat: _24HourFormat = {}", format24);
    _24HourFormat = format24;
    updateDisplay();
}

void digital_clock::setColor(const QColor &color)
{
    SPDLOG_DEBUG("digital_clock::setColor: color = {}",
                 color.name().toStdString());
    this->color = color;
    QPalette pal = palette();
    pal.setColor(QPalette::WindowText, color);
    setPalette(pal);
}

//  общий накопитель
qint64 digital_clock::get_all_time() const
{
    return all_time;
}

void digital_clock::reset_all_time()
{
    all_time = 0;
}
