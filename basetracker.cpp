// #include "basetracker.h"
// #include "qboxlayout.h"
// #include "qcombobox.h"
// #include "qlineedit.h"
// #include "qmessagebox.h"
// #include "qpushbutton.h"
// #include "qtoolbutton.h"
// #include <QCheckBox>
// #include <QStackedWidget>
// #include <QListWidget>
// #include <QScrollArea>
// #include <QScrollBar>
// #include <QPainter>
// #include "data_manager.h"
// #include <QSpinBox>
// #include "new_user_form.h"
// #include "users_form.h"
// #include <QDialogButtonBox>
// #include <QVBoxLayout>

// using namespace FTR;


// BaseTracker::BaseTracker(QWidget *parent,FTR::TimerMode mode)
//     : QWidget(parent),mode(mode)
// {  this->resize(800, 600);
//     switch(mode){
//     case FlowMode:
//     case TimeBlocking:
//     case EatTheFrog:
//         analogClock = new MyClock(this);
//         analogClock->setStyleSheet("background: transparent;");
//         analogClock->setAttribute(Qt::WA_TranslucentBackground);

//         digitalClock = new digital_clock(this);
//         digitalClock->setColor(Qt::white);
//         digitalClock->setShowSeconds(true);
//         digitalClock->set24HourFormat(true);
//         digitalClock->setStyleSheet("background: transparent;");

//         digitalClock2 = new digital_clock(this);
//         digitalClock2->setColor(Qt::white);
//         digitalClock2->setShowSeconds(true);
//         digitalClock2->set24HourFormat(true);
//         digitalClock2->setStyleSheet("background: transparent;");
//         digitalClock2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

//         analogClock->setFixedSize(250,250);
//         digitalClock->setFixedSize(250, 80);
//         digitalClock2->setFixedSize(250, 80);
//         analogClock->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
//         digitalClock->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
//         break;
//     case Pomodoro:
//     case ReversePomodoro:
//     case Mode60_10:
//     case Mode52_17:
//     case Mode90_20:
//         digitalClock = new digital_clock(this);
//         digitalClock->setColor(Qt::white);
//         digitalClock->setShowSeconds(true);
//         digitalClock->set24HourFormat(true);
//         digitalClock->setStyleSheet("background: transparent;");
//         digitalClock->setFixedSize(250, 80);
//         digitalClock->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
//         break;}
//     //СОЗДАНИЕ НЕВИДИМОГО ТАЙМЕРА
//     blockTimer = new QTimer(this);
//     blockTimer->setSingleShot(true);
//     connect(blockTimer, &QTimer::timeout, this, &BaseTracker::onBlockTimerFinished);
//     SPDLOG_TRACE("Драйверы: {}",QSqlDatabase::drivers().join(", ").toStdString());
//     SPDLOG_TRACE("Создание базового виртуального класса");
// }

// //скрыть комбобокс с тасками в зависимости от режима гостя или юзера
// void BaseTracker::hide_combo(){
//     qint64 uid = Data_Manager::instance().get_currentUser();
//     if (uid>=0){
//         SPDLOG_INFO("hide_combo: Режим пользователя. Не скрываем");
//         return;
//     }
//     QComboBox* c = activityComboBox();
//         c->hide();
//     QLabel* l = get_activity_label();
//         l->hide();
//     SPDLOG_INFO("hide_combo: Режим гостя. Комбо-бокс скрыт");
// }

// BaseTracker::~BaseTracker()
// {
//     if (taskTimer && taskTimer->isActive())
//         taskTimer->stop();
//     if (todolist) {
//         delete todolist;
//         todolist = nullptr;
//     }
//     SPDLOG_DEBUG("Удален базовый класс");
// }

// //подключение ссылки на профиль
// void BaseTracker::connectInfoLink()
// {
//     if (profileLinkConnected) {
//         SPDLOG_TRACE("connectInfoLink: ссылки уже подключены, повторное подключение не требуется");
//         return;}
//     QLabel* link = get_info_link();
//     if (link) {
//         SPDLOG_DEBUG("connectInfoLink: ссылка 'Информация' подключена (objectName = {})",
//                       link->objectName().toStdString());
//         connect(link, &QLabel::linkActivated,
//                 this, &BaseTracker::on_label_7_linkActivated);
//     }
//     else{
//         SPDLOG_WARN("connectInfoLink: get_info_link() вернул nullptr - ссылка не будет работать");}
//     // обработка ссылки Гость
//     QLabel* userLbl = userLabel();
//     if (userLbl) {
//         SPDLOG_DEBUG("connectInfoLink: ссылка 'Пользователь/Гость' подключена "
//                      "(objectName = '{}')",
//                      userLbl->objectName().toStdString());
//         connect(userLbl, &QLabel::linkActivated,
//                 this, &BaseTracker::on_guest_link_activated);
//     }
//     profileLinkConnected = true;
//     SPDLOG_TRACE("connectInfoLink: все ссылки подключены, флаг profileLinkConnected = true");
// }

// //старт режима таймера
// void BaseTracker::startPomodoro(digital_clock* clock, int count)
// {
//     if (!clock) {
//         SPDLOG_ERROR("startPomodoro: передан nullptr вместо часов — запуск невозможен");
//         return;
//     }
//     if (count <= 0) {
//         SPDLOG_ERROR("startPomodoro: некорректное количество помидоров (count = {}) — запуск невозможен",
//                      count);
//         return;
//     }
//     this->clock = clock;
//     left  = count * 2 - 1;
//     work  = true;
//     currentPhase = Phase::Work;
//     SPDLOG_INFO("startPomodoro: запуск, count = {}, left (фаз) = {}, "
//                 "workMinutes = {}, breakMinutes = {}",
//                 count, left, workMinutes, breakMinutes);
//     if (sessionStart.isValid()) {
//         SPDLOG_WARN("startPomodoro: обнаружена незакрытая сессия "
//                     "(start = {}), закрываем её",
//                     sessionStart.toString(Qt::ISODate).toStdString());
//     }
//     finishCurrentSession(false);
//     connect(clock, &digital_clock::timerFinished,
//             this, &BaseTracker::nextPhase,
//             Qt::UniqueConnection);
//     SPDLOG_TRACE("startPomodoro: сигнал timerFinished подключён к nextPhase()");
//     nextPhase();
//     SPDLOG_TRACE("startPomodoro: первая фаза запущена, left = {}", left);
// }

// //открытие окошка с информацией
// void BaseTracker::on_label_7_linkActivated(const QString &link){
//     if (link != "#open"){
//             SPDLOG_TRACE("on_label_7_linkActivated: получена ссылка {},не #open - выход",
//                          link.toStdString());
//         return;
//     }
//     SPDLOG_TRACE("on_label_7_linkActivated: клик по ссылке 'Информация'");
//     if (!info){
//         SPDLOG_DEBUG("on_label_7_linkActivated: создаём новое окно Information");
//         info = new Information(nullptr,get_info_text());
//         info->setAttribute(Qt::WA_DeleteOnClose);
//         info->set_parent_window(this);
//         connect(info, &QObject::destroyed,
//                 this, [this]() {
//             SPDLOG_TRACE("on_label_7_linkActivated: окно Information удалено, указатель сброшен");
//             info = nullptr; });
//     }else {
//         SPDLOG_DEBUG("on_label_7_linkActivated: окно Information уже существует, "
//                      "повторный показ");
//     }
//     info->show();
//     SPDLOG_DEBUG("on_label_7_linkActivated: окно Information показано, "
//                  "трекер '{}' скрыт",
//                  windowTitleText().toStdString());
//     this->hide();
// }

// //смена фазы
// void BaseTracker::nextPhase()
// {
//     if (!clock) {
//         SPDLOG_ERROR("nextPhase: clock == nullptr — смена фазы невозможна");
//         return;
//     }
//     SPDLOG_DEBUG("nextPhase: закрываем предыдущую сессию (left = {}, work = {})",
//                  left, work);
//     finishCurrentSession(true);
//     if (left <= 0) {
//         SPDLOG_INFO("nextPhase: все фазы завершены (left = 0) — задача выполнена");
//         if (activeTaskLabel)
//             activeTaskLabel->setText(tr("Задача выполнена"));
//         // ── ЗАЩИТА: проверяем, что чекбокс ещё существует ──
//         if (activeDoneCheck) {
//             SPDLOG_DEBUG("nextPhase: чекбокс текущей задачи отмечен как выполненный");
//             activeDoneCheck->setChecked(true);
//         } else {
//             SPDLOG_WARN("nextPhase: activeDoneCheck == nullptr — "
//                         "чекбокс не отмечен (возможно, строка удалена)");
//         }
//         playNotificationSound();
//         SPDLOG_TRACE("nextPhase: цикл завершён, звуковая нотификация воспроизведена");
//         return;
//     }
//     left--;
//     sessionStart = QDateTime::currentDateTime();
//     if (work) {
//         currentPhase = Phase::Work;
//         if (activeTaskLabel)
//             activeTaskLabel->setText(tr("Работа %1 мин").arg(workMinutes));
//         clock->setMode(FTR::timer);
//         SPDLOG_INFO("nextPhase: запуск РАБОТЫ, {} мин (осталось фаз: {})",
//                     workMinutes, left);
//         clock->timer_start(0, workMinutes, 0);
//     } else {
//         currentPhase = Phase::Break;
//         if (activeTaskLabel)
//             activeTaskLabel->setText(tr("Отдых %1 мин").arg(breakMinutes));
//         clock->setMode(FTR::timer);
//         SPDLOG_INFO("nextPhase: запуск ОТДЫХА, {} мин (осталось фаз: {})",
//                     breakMinutes, left);
//         clock->timer_start(0, breakMinutes, 0);
//     }
//     playNotificationSound();
//     work = !work;
//     SPDLOG_TRACE("nextPhase: фаза запущена, следующий work = {}, left = {}",
//                  work, left);
// }

// void BaseTracker::createTaskRow(QLineEdit*&   outName,
//                                 QLineEdit*&   outCount,
//                                 QCheckBox*&   outDone,
//                                 QPushButton*& outStart)
// {
//     if (!rowsLayout || !rowsContainer) {
//         SPDLOG_ERROR("createTaskRow: rowsLayout или rowsContainer == nullptr — "
//                      "строку создать невозможно (rowsLayout = {}, rowsContainer = {})",
//                      static_cast<const void*>(rowsLayout),
//                      static_cast<const void*>(rowsContainer));
//         return;
//     }
//     SPDLOG_TRACE("createTaskRow: создание новой строки задачи");
//     QHBoxLayout* row = new QHBoxLayout();
//     outName  = new QLineEdit(rowsContainer);
//     outCount = new QLineEdit(rowsContainer);
//     outDone  = new QCheckBox(rowsContainer);
//     outStart = new QPushButton("▶", rowsContainer);
//     //кнопка удаления
//     QPushButton* removeBtn = new QPushButton("✕", rowsContainer);
//     removeBtn->setFixedWidth(30);
//     removeBtn->setStyleSheet(
//         "QPushButton { color: white; background: rgba(0,0,0,0.5); "
//         "border: 1px solid rgba(255,255,255,0.4); border-radius: 4px; padding: 2px; }"
//         "QPushButton:hover { background: rgba(255,0,0,0.5); }");
//     outDone->setEnabled(false);
//     outDone->setStyleSheet(
//         "QCheckBox { color: white; }"
//         "QCheckBox::indicator { width: 16px; height: 16px; }");
//     outName->setPlaceholderText(tr("Название задачи"));
//     outCount->setPlaceholderText(tr("Кол-во"));
//     outCount->setValidator(new QIntValidator(1, 99, outCount));
//     outStart->setFixedWidth(40);
//     row->addWidget(outName);
//     row->addWidget(outCount);
//     row->addWidget(outDone);
//     row->addWidget(outStart);
//     row->addWidget(removeBtn);
//     rowsLayout->insertLayout(rowsLayout->count() - 1, row);
//     SPDLOG_TRACE("createTaskRow: строка добавлена в rowsLayout, "
//                  "всего элементов в layout = {}",
//                  rowsLayout->count());
//     // Обработчик удаления
//     connect(removeBtn, &QPushButton::clicked, this,
//             [this, row, outName, outCount, outDone, outStart, removeBtn]() {
//                 SPDLOG_INFO("createTaskRow: удаление строки задачи");
//                 //если активная задача удалена-сброс указателя
//                 if (activeDoneCheck == outDone) {
//                     SPDLOG_DEBUG("createTaskRow: удаляемая строка — активная "
//                                  "(pomoClock running = {})",
//                                  pomoClock ? pomoClock->is_running() : false);
//                     activeDoneCheck = nullptr;
//                     // Остановка таймера помидора, если он запущен
//                     if (pomoClock) {
//                         pomoClock->timer_stop();
//                         pomoClock->set_timer_time(0, 0, 0);
//                         SPDLOG_TRACE("createTaskRow: pomoClock остановлен и сброшен");
//                     }
//                     if (sessionStart.isValid()) {
//                         SPDLOG_DEBUG("createTaskRow: закрытие активной сессии "
//                                      "из-за удаления активной задачи");
//                     }
//                     // Закрытие сессии
//                     finishCurrentSession(false);
//                     // Возвращение на страницу задач (если сейчас на странице помидора)
//                     if (pomoStack && tasksPage) {
//                         pomoStack->setCurrentWidget(tasksPage);
//                         SPDLOG_TRACE("createTaskRow: переключение на tasksPage");
//                     }
//                     // Показ часов
//                     if (digitalClock) digitalClock->show();
//                     if (analogClock)  analogClock->show();
//                 }
//                 if (rowsLayout) {
//                     SPDLOG_TRACE("createTaskRow: строка удалена из rowsLayout, "
//                                  "осталось элементов = {}",
//                                  rowsLayout->count());
//                     rowsLayout->removeItem(row);
//                 }
//                 delete outName;
//                 delete outCount;
//                 delete outDone;
//                 delete outStart;
//                 delete removeBtn;
//                 delete row;
//                 SPDLOG_TRACE("createTaskRow: все виджеты строки удалены");
//             });
//     if (get_info_link()) {
//         connect(get_info_link(), &QLabel::linkActivated,
//                 this, &BaseTracker::on_label_7_linkActivated);
//         SPDLOG_TRACE("createTaskRow: ссылка 'Информация' подключена");
//     } else {
//         SPDLOG_WARN("createTaskRow: get_info_link() == nullptr — "
//                     "ссылка 'Информация' не будет работать в этой строке");
//     }
// }

// QLineEdit* BaseTracker::addTaskRow()
// {
//     QLineEdit*   taskLine      = nullptr;
//     QLineEdit*   pomodoroCount = nullptr;
//     QCheckBox*   doneCheck     = nullptr;
//     QPushButton* startBtn      = nullptr;
//     createTaskRow(taskLine, pomodoroCount, doneCheck, startBtn);
//     if (!taskLine) return nullptr;
//     connect(startBtn, &QPushButton::clicked, this,
//             [this, taskLine, pomodoroCount, doneCheck]()
//             {
//         //Название задачи не пустое
//         QString name = taskLine->text().trimmed();
//         if (name.isEmpty()) {
//             QMessageBox::information(this, "Внимание",
//                                      "Введите название задачи перед запуском.");
//             taskLine->setFocus();
//             return;
//         }
//         //Количество помидоров > 0
//         int count = pomodoroCount->text().toInt();
//         if (count <= 0) {
//             QMessageBox::information(this, "Внимание",
//                                      "Введите количество помидоров (от 1 до 99).");
//             pomodoroCount->setFocus();
//             return;
//         }
//         QComboBox* combo = activityComboBox();
//         QString activityName = combo ? combo->currentText().trimmed() : QString();
//         currentTaskId = combo ? combo->currentData().toLongLong() : -1;
//         if (p_activeActivityLabel)
//             p_activeActivityLabel->setText(tr("Активность: %1").arg(
//                 activityName.isEmpty() ? tr("—") : activityName));
//         // Подзадача (название строки)
//         if (p_activeSubtaskLabel)
//             p_activeSubtaskLabel->setText(tr("Подзадача: %1").arg(name));
//         qDebug() << "Pomodoro start: activity =" << activityName
//                  << "subtask =" << name<< "task_id =" << currentTaskId;
//         // Закрытие старой сессии
//         finishCurrentSession(false);
//         this->activeDoneCheck = doneCheck;
//         if (activeTaskLabel)
//             activeTaskLabel->setText(tr("Задача: %1").arg(name));
//         if (pomoStack && pomodoroPage)
//             pomoStack->setCurrentWidget(pomodoroPage);
//         if (digitalClock) digitalClock->hide();
//         if (analogClock)  analogClock->hide();
//         work = true;
//         currentPhase = Phase::Work;
//         startPomodoro(pomoClock, count);
//     });
//     return taskLine;
// }

// void BaseTracker::createFlowTaskRow(QVBoxLayout* rowsLayout,
//                                     QWidget* container,
//                                     const QString& text,
//                                     PriorityWidgetBuilder priorityFactory,
//                                     bool isStopwatch)
// {
//     if (!rowsLayout || !container) return;
//     QHBoxLayout* row = new QHBoxLayout();
//     QComboBox* taskCombo = new QComboBox(container);
//     taskCombo->setEditable(true);
//     taskCombo->setInsertPolicy(QComboBox::NoInsert);
//     taskCombo->setStyleSheet(
//         // Основной вид (когда не в фокусе)
//         "QComboBox { "
//         "   color: black; font-weight: bold; "
//         "   background-color: rgba(255,255,255,0.5); "
//         "   border: 1px solid rgba(255,255,255,0.4); "
//         "   border-radius: 4px; padding: 2px; "
//         "}"

//         // При наведении
//         "QComboBox:hover { background-color: rgba(255,0,0,0.3); }"

//         // В фокусе (подсветка красным)
//         "QComboBox:focus { "
//         "   background-color: rgba(255, 0, 0, 0.5); color: black; "
//         "   border: 1px solid rgba(255,255,255,0.8); "
//         "}"

//         // Убираем пунктирную рамку, которая появляется при фокусе
//         "QComboBox:focus { outline: none; }"

//         // ВНУТРЕННИЙ QLineEdit (когда пользователь вводит текст)
//         "QComboBox QLineEdit { "
//         "   background: transparent; border: none; color: black; "
//         "   selection-background-color: rgba(255,0,0,0.5); "
//         "}"

//         // Выпадающий список
//         "QComboBox QAbstractItemView { "
//         "   background-color: rgba(30, 30, 30, 0.95); color: white; "
//         "   selection-background-color: rgba(255,0,0,0.5); "
//         "   border: 1px solid rgba(255,255,255,0.4); "
//         "   outline: none; " // Убираем рамку вокруг выпадающего списка
//         "}"
//         );
//     taskCombo->setToolTip("Введите название задачи\nили выберите его");
//     // Заполнение задачами
//     refreshTaskCombo(taskCombo);
//     // Если передали текст при создании, ставим его
//     if (!text.isEmpty()) {
//         int idx = taskCombo->findText(text, Qt::MatchFixedString);
//         if (idx >= 0) taskCombo->setCurrentIndex(idx);
//         else taskCombo->setCurrentText(text);
//     }
//     //  РЕГИСТРАЦИЯ ЗАДАЧИ В БД СРАЗУ ПРИ ВВОДЕ
//     if (taskCombo->lineEdit()) {
//         connect(taskCombo->lineEdit(), &QLineEdit::returnPressed,
//                 this, [this, taskCombo]() {
//                     // Закрываем popup, иначе Enter обработает сам QComboBox
//                     taskCombo->hidePopup();
//                     ensureTaskRegistered(taskCombo);
//                 });
//     }
//     // Регистрация при выборе из выпадающего списка
//     connect(taskCombo, QOverload<int>::of(&QComboBox::activated),
//             this, [this, taskCombo](int) {
//                 ensureTaskRegistered(taskCombo);
//             });
//     //Синхронизация: когда где-то создана новая задача - обновление списка
//     connect(this, &BaseTracker::tasksChanged, taskCombo, [this, taskCombo]{
//         refreshTaskCombo(taskCombo);
//     });
//     // ПОЛЕ ВРЕМЕНИ
//     QLineEdit* timeLine = new QLineEdit(container);
//     timeLine->setPlaceholderText(tr("Мин"));
//     timeLine->setStyleSheet(
//         "QLineEdit { color: black; font-weight: bold; background-color: rgba(255,255,255,0.5); border: 1px solid rgba(255,255,255,0.4); border-radius: 4px; padding: 2px; }"
//         "QLineEdit:hover { background-color: rgba(255,0,0,0.3); }"
//         "QLineEdit:focus { background-color: rgba(255, 0, 0, 0.5); color: black; border: 1px solid rgba(255,255,255,0.8); }"
//         );
//     if (isStopwatch) timeLine->hide();
//     QCheckBox* done = new QCheckBox(container);
//     done->setStyleSheet("QCheckBox { color: black; } QCheckBox::indicator { width: 16px; height: 16px; subcontrol-position: center center; }");
//     done->setFixedWidth(20);
//     if (isStopwatch) done->hide();
//     QPushButton* startBtn = new QPushButton("▶", container);
//     startBtn->setFixedWidth(30);
//     startBtn->setStyleSheet(
//         "QPushButton { color: black; font-weight: bold; background-color: rgba(255,255,255,0.5); border: 1px solid rgba(255,255,255,0.4); border-radius: 4px; padding: 2px; }"
//         "QPushButton:hover { background-color: rgba(255,0,0,0.5); }");
//     QPushButton* removeBtn = new QPushButton("✕", container);
//     removeBtn->setFixedWidth(30);
//     removeBtn->setStyleSheet(
//         "QPushButton { color: black; font-weight: bold; background-color: rgba(255,255,255,0.5); border: 1px solid rgba(255,255,255,0.4); border-radius: 4px; padding: 2px; }"
//         "QPushButton:hover { background-color: rgba(255,0,0,0.5); }");
//     QComboBox* priorityCombo = nullptr;
//     if (priorityFactory) {
//         priorityCombo = qobject_cast<QComboBox*>(priorityFactory(container));
//         if (priorityCombo) {
//             row->insertWidget(0, priorityCombo);
//             connect(priorityCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this, rowsLayout]() {
//                 sortFlowRows(rowsLayout);
//             });
//         }
//     }
//     row->addWidget(taskCombo, 1);
//     if (!isStopwatch) {
//         row->addWidget(timeLine, 1);
//     }
//     row->addWidget(done);
//     row->addWidget(startBtn);
//     row->addWidget(removeBtn);
//     rowsLayout->insertLayout(rowsLayout->count() - 1, row);
//     // СОХРАНЕНИЕ В СПИСОК
//     int myIndex = flowTasksList.size();
//     FlowTask ft;
//     ft.nameEdit = taskCombo; // Теперь это указатель на QComboBox (проверь, что в .h структура FlowTask изменилась)
//     ft.timeEdit = timeLine;
//     ft.doneCheck = done;
//     ft.priorityCombo = priorityCombo;
//     ft.startBtn = startBtn;
//     flowTasksList.append(ft);
//     if (flowTasksList.size() == 1) selectFlowTask(0);
//     // СОБЫТИЯ ДЛЯ ВЫДЕЛЕНИЯ
//     if (mode == FTR::FlowMode) {
//         taskCombo->setProperty("taskIndex", myIndex);
//         timeLine->setProperty("taskIndex", myIndex);
//         taskCombo->installEventFilter(this);
//         timeLine->installEventFilter(this);
//     }
//     // СИНХРОНИЗАЦИЯ ДАННЫХ
//     connect(timeLine, &QLineEdit::textChanged, this, [this, taskCombo, timeLine]() {
//         int idx = -1;
//         for (int i = 0; i < flowTasksList.size(); ++i) {
//             if (flowTasksList[i].nameEdit == taskCombo) { idx = i; break; }
//         }
//         if (idx < 0 || idx != activeFlowIndex) return;
//         int duration = parseDurationMin(timeLine->text());
//         if (digitalClock2 && flowTabs && flowTabs->currentIndex() == 1) {
//             digitalClock2->set_timer_time(0, duration, 0);
//         }
//     });
//     //КНОПКА СТАРТ
//     connect(startBtn, &QPushButton::clicked, this, [this, taskCombo] {
//         int idx = -1;
//         for (int i = 0; i < flowTasksList.size(); ++i) {
//             if (flowTasksList[i].nameEdit == taskCombo) { idx = i; break; }
//         }
//         if (idx < 0) return;
//         // Гарантия что задача зарегистрирована в БД
//         const qint64 id = ensureTaskRegistered(taskCombo);
//         if (id <= 0) {
//             QMessageBox::information(this, tr("Внимание"),
//                                      tr("Введите название задачи перед запуском."));
//             taskCombo->setFocus();
//             return;
//         }
//         selectFlowTask(idx);
//         // Если EatTheFrog - запуск последовательности
//         if (mode == FTR::EatTheFrog) {
//             startTaskSequence();
//         } else {
//             // Для  FlowMode запуск таймера напрямую
//             FTR::ClockMode clockMode = FTR::stopwatch;
//             if (flowTabs && flowTabs->currentIndex() == 1) clockMode = FTR::timer;
//             applyClockMode(clockMode);
//             // Проверка текста
//             QString name = taskCombo->currentText().trimmed();
//             if (name.isEmpty()) {
//                 QMessageBox::information(this, "Ошибка", "Введите название задачи");
//                 return;
//             }
//             qint64 taskId = taskCombo->currentData().toLongLong();
//             finishCurrentSession(false);
//             currentTaskId = taskId;
//             sessionStart = QDateTime::currentDateTime();
//             digital_clock* c = digitalClock2;
//             if (c) {
//                 c->timer_stop();
//                 c->set_timer_time(0, 0, 0);
//                 if (clockMode == FTR::stopwatch) {
//                     c->stopwatch_start();
//                 } else {
//                     int duration = parseDurationMin(flowTasksList[idx].timeEdit->text());
//                     if (duration == 0) duration = workMinutes;
//                     c->timer_start(0, duration, 0);
//                 }
//             }
//         }
//     });
//     connect(removeBtn, &QPushButton::clicked, this,
//             [this, row, taskCombo, timeLine, done, startBtn, removeBtn,priorityCombo]() {
//         // индекс по указателю nameEdit
//         int idx = -1;
//         for (int i = 0; i < flowTasksList.size(); ++i) {
//             if (flowTasksList[i].nameEdit == taskCombo) { idx = i; break; }
//         }
//         // была ли удаляемая строка активной
//         const bool removingActive = (idx >= 0 && idx == activeFlowIndex);
//         // Удаление из модели и корректировка activeFlowIndex
//         if (idx >= 0) {
//             flowTasksList.removeAt(idx);
//             if (activeFlowIndex == idx) {
//                 activeFlowIndex = -1;
//             } else if (activeFlowIndex > idx) {
//                 activeFlowIndex--;
//             }
//         }
//         //ОСТАНОВКА ТАЙМЕРА/СЕКУНДОМЕРА
//         //список стал пустым — отсчёт не к чему привязать,
//         //удалили активную строку — таймер тикал именно на неё.
//         //Если удалили неактивную и остались другие — таймер не трогаем.
//         if (flowTasksList.isEmpty() || removingActive) {
//             stopFlowClockAndSession();
//         }
//         // АВТО-СОСТОЯНИЕ ПОСЛЕ УДАЛЕНИЯ
//         if (flowTasksList.size() == 1) {
//             if (flowTasksList[0].nameEdit && flowTasksList[0].timeEdit) {
//                 selectFlowTask(0);
//             }
//         } else if (flowTasksList.isEmpty()) {
//             activeFlowIndex = -1;
//             updateClockEditable();
//             if (sequenceStatusLabel) {
//                 sequenceStatusLabel->setText(tr("Список не запущен"));
//             }
//         }
//         else if (removingActive) {
//             if (sequenceStatusLabel) {
//                 sequenceStatusLabel->setText(tr("Список не запущен"));
//             }
//         }
//         // Пересчёт taskIndex
//         for (int i = 0; i < flowTasksList.size(); ++i) {
//             if (flowTasksList[i].nameEdit)
//                 flowTasksList[i].nameEdit->setProperty("taskIndex", i);
//             if (flowTasksList[i].timeEdit)
//                 flowTasksList[i].timeEdit->setProperty("taskIndex", i);
//         }// удаление layout из rowsLayout
//         if (this->rowsLayout && row) {
//             this->rowsLayout->removeItem(row);
//         }
//         // Удаление виджетов
//         delete taskCombo;
//         delete timeLine;
//         delete done;
//         delete startBtn;
//         delete removeBtn;
//         if (priorityCombo) delete priorityCombo;
//         if (row) delete row;
//     });
// }
// QWidget* BaseTracker::buildFlowTabPage(FTR::ClockMode clockMode,
//                                        PriorityWidgetBuilder priorityFactory)
// {
//     QWidget* page = new QWidget();
//     page->setStyleSheet("QWidget { background: rgba(0,0,0,0.6); border-radius: 10px; }");
//     QVBoxLayout* pageLayout = new QVBoxLayout(page);
//     QLabel* header = new QLabel(
//         clockMode == FTR::stopwatch ? tr("Список задач — секундомер") : tr("Список задач — таймер"));
//     header->setAlignment(Qt::AlignHCenter);
//     header->setStyleSheet("QLabel { color: white; font-size: 18px; "
//                           "font-weight: bold;background: transparent; }");
//     pageLayout->addWidget(header);
//     QWidget* container = new QWidget(page);
//     container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//     container->setMinimumWidth(400);
//     container->setMaximumWidth(800);
//     container->setStyleSheet("QWidget { background: transparent; }");
//     QVBoxLayout* listLayout = new QVBoxLayout(container);
//     listLayout->setContentsMargins(0, 0, 0, 0);
//     listLayout->setSpacing(4);
//     QHBoxLayout* columns = new QHBoxLayout();
//     listLayout->addLayout(columns);
//     columns->addStretch();
//     if (mode == FTR::EatTheFrog){
//         QLabel* task_priority = new QLabel("Приоритет");
//         columns->addWidget(task_priority);
//         columns->addStretch();
//     }
//     QLabel* task_name = new QLabel("Название задачи");
//     columns->addWidget(task_name );
//     columns->addStretch();
//     if (clockMode == timer){
//         QLabel* task_time = new QLabel("Время");
//         columns->addWidget(task_time);
//         columns->addStretch();
//         QLabel* done_time = new QLabel("Сделано");
//         columns->addWidget(done_time);
//     }
//     QLabel* start_task = new QLabel("Старт");
//     columns->addWidget(start_task);
//     QLabel* delete_task = new QLabel("Удалить");
//     columns->addWidget(delete_task);
//     listLayout->addStretch();
//     this->rowsLayout = listLayout;
//     this->rowsContainer = container;
//     QScrollArea* area = new QScrollArea(page);
//     area->setWidgetResizable(true);
//     area->setWidget(container);
//     area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//     area->setFrameShape(QFrame::NoFrame);
//     area->setStyleSheet("QScrollArea { background: transparent; border: none; }");
//     pageLayout->addWidget(area, 1);
//     if (priorityFactory) {
//         sequenceStatusLabel = new QLabel(tr("Список не запущен"), page);
//         sequenceStatusLabel->setStyleSheet(
//             "QLabel { color: white; font-weight: bold; background: transparent; }");
//         pageLayout->addWidget(sequenceStatusLabel);
//     }
//     //Строка ввода (Комбобокс)
//     QHBoxLayout* inputRow = new QHBoxLayout();
//     QComboBox* input = new QComboBox(page);
//     input->setEditable(true); // Разрешаем вводить текст
//     input->setInsertPolicy(QComboBox::NoInsert); // Запрещаем Qt автоматически добавлять в конец списка (мы сделаем это сами)
//     input->setPlaceholderText(tr("Введите или выберите задание"));
//     // Заполнение задачами
//     refreshTaskCombo(input);
//     // Синхронизация input с другими комбобоксами
//     connect(this, &BaseTracker::tasksChanged, input, [this, input]{
//         refreshTaskCombo(input);
//     });
//     input->setStyleSheet(
//         "QComboBox {"
//         "  color: black; font-weight: bold;"
//         "  background-color: rgba(255,255,255,0.5);"
//         "  border: 1px solid rgba(255,255,255,0.4);"
//         "  border-radius: 4px;"
//         "  padding: 2px;"
//         "}"
//         "QComboBox:hover {"
//         "  background-color: rgba(255,0,0,0.5);"
//         "}"
//         "QComboBox QLineEdit {"
//         "  background: transparent; border: none; color: black;"
//         "}"
//         "QComboBox QAbstractItemView {"
//         "  background-color: rgba(30, 30, 30, 0.9); color: white;"
//         "  selection-background-color: rgba(255,0,0,0.5);"
//         "}"
//         );
//     QPushButton* addBtn = new QPushButton(tr("Добавить"), page);
//     addBtn->setStyleSheet(
//         "QPushButton {"
//         "  color: black;font-weight: bold;"
//         "  background-color: rgba(255,255,255,0.5);"
//         "  border: 1px solid rgba(255,255,255,0.4);"
//         "  border-radius: 4px;"
//         "  padding: 2px;"
//         "}"
//         "QPushButton:hover {"
//         "  background-color: rgba(255,0,0,0.5);"
//         "}"
//         );
//     inputRow->addWidget(input, 1);
//     inputRow->addWidget(addBtn);
//     pageLayout->addLayout(inputRow);
//     container->installEventFilter(this);
//     bool isStopwatch = (clockMode == FTR::stopwatch);
//     // Коннекты
//     connect(addBtn, &QPushButton::clicked, this, [this, input, listLayout, container,
//                                                   priorityFactory, isStopwatch]() {
//        QString text = input->currentText().trimmed();
//        if (text.isEmpty()) return;
//        // ПРОВЕРКА НА ДУБЛИКАТ
//        for (const auto& ft : flowTasksList) {
//            if (ft.nameEdit && ft.nameEdit->currentText().trimmed() == text) {
//                QMessageBox::information(this, "Внимание",
//                                         QString("Задача \"%1\" уже есть в списке.").arg(text));
//                input->setFocus();
//                if (input->lineEdit()) input->lineEdit()->selectAll();
//                return;
//            }
//        }
//        // РЕГИСТРАЦИЯ ЗАДАЧИ В БД
//        input->setCurrentText(text);
//        ensureTaskRegistered(input);
//        createFlowTaskRow(listLayout, container, text, priorityFactory, isStopwatch);
//        // Очистка ввода
//        input->setCurrentIndex(-1);
//        input->clearEditText();
//        input->setFocus();
//     });
//     if (input->lineEdit()) {
//        connect(input->lineEdit(), &QLineEdit::returnPressed, addBtn, &QPushButton::click);
//    }
//    if (digitalClock2) digitalClock2->setEditable(false);
//    return page;
// }

//  QWidget* BaseTracker::buildPriorityCombo(QWidget* parent)
// {
//     QComboBox* diffCombo = new QComboBox(parent);
//     diffCombo->addItem("🐸 Лягушка", 5);
//     diffCombo->addItem("Сложно", 4);
//     diffCombo->addItem("Обычно", 3);
//     diffCombo->addItem("Просто", 2);
//     diffCombo->addItem("Элементарно", 1);
//     diffCombo->setStyleSheet(
//         "QComboBox { color: black; font-weight: bold; "
//         "background-color: rgba(255,255,255,0.5); "
//         "border: 1px solid rgba(255,255,255,0.4); "
//         "border-radius: 4px; padding: 2px; }"
//         "QComboBox:hover { background-color: rgba(255,0,0,0.3); }"
//         "QComboBox:focus { background-color: rgba(255, 0, 0, 0.5); color: black; "
//         "border: 1px solid rgba(255,255,255,0.8); }");
//     return diffCombo;
// }
// void BaseTracker::addFlowModeInterface(QVBoxLayout* layout,
//                           PriorityWidgetBuilder priorityBuilder )
// {
//     if (!layout) return;
//     QFrame* frame = new QFrame(this);
//     frame->setObjectName("blockFrame");
//     frame->setStyleSheet(
//         "QFrame#blockFrame { background: rgba(1,1,1,0.6); border-radius: 10px; }"
//         "QLabel { color: white; }");
//     frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//     frame->setMinimumWidth(420);
//     layout->addWidget(frame, 1);
//     QVBoxLayout* vbox = new QVBoxLayout(frame);
//     vbox->setContentsMargins(8, 8, 8, 8);
//     vbox->setSpacing(4);
//     QHBoxLayout* btnrow = new QHBoxLayout();
//     vbox->addLayout(btnrow);
//     placeClocksInLayout(nullptr,digitalClock2,vbox,btnrow);
//     setupButtons(btnrow);
//     flowTabs = new QTabWidget(this);
//     flowTabs->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//     flowTabs->setMinimumWidth(400);
//     flowTabs->setMaximumWidth(800);
//     flowTabs->setStyleSheet(
//         "QTabWidget::pane { border: none; background: transparent; }"
//         "QTabBar::tab {"
//         "   background: rgba(0,0,0,0.6); color: white;"
//         "   padding: 8px 16px; border-top-left-radius: 8px;"
//         "   border-top-right-radius: 8px; margin-right: 2px;"
//         "}"
//         "QTabBar::tab:selected { background: rgba(255,0,0,0.6); }");
//     if (digitalClock2) {
//         connect(digitalClock2, &digital_clock::timeChanged, this, [this](int seconds) {
//             if (mode != FTR::FlowMode) return;
//              qDebug() << "DEBUG: Лямбда поймала timeChanged! Секунд:" << seconds;
//             if (activeFlowIndex < 0 || activeFlowIndex >= flowTasksList.size()) return;
//             if (!flowTabs || flowTabs->currentIndex() != 1) return;
//             int minutes = seconds / 60;
//             if (seconds % 60 != 0) minutes++;
//             FlowTask& ft = flowTasksList[activeFlowIndex];
//             ft.timeEdit->blockSignals(true);
//             ft.timeEdit->setText(QString::number(minutes));
//             ft.timeEdit->blockSignals(false);
//         });
//     }
//     if (this->mode == FTR::FlowMode){
//     stopwatchPage = buildFlowTabPage(FTR::stopwatch, priorityBuilder);
//     timerPage     = buildFlowTabPage(FTR::timer, priorityBuilder);
//     flowTabs->addTab(stopwatchPage, tr("Секундомер"));
//     flowTabs->addTab(timerPage, tr("Таймер"));
//     vbox->addWidget(flowTabs, 1);
//     connect(flowTabs, &QTabWidget::currentChanged, this, [this](int idx) {
//         applyClockMode(idx == 0 ? FTR::stopwatch : FTR::timer);
//         updateClockEditable();
//     });
//     applyClockMode(FTR::stopwatch);}
//     else {
//         timerPage     = buildFlowTabPage(FTR::timer, priorityBuilder);
//         flowTabs->addTab(timerPage,tr("Таймер"));
//         vbox->addWidget(flowTabs, 1);
//         applyClockMode(FTR::timer);
//     }
// }

// void BaseTracker::addTaskListInterface(QVBoxLayout* layout, int workMin, int breakMin)
// {
//     if (!layout) return;
//     workMinutes  = workMin;
//     breakMinutes = breakMin;
//     //Стек
//     pomoStack = new QStackedWidget(this);
//     pomoStack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//     //Центрируем стек по горизонтали
//     QHBoxLayout* centerRow = new QHBoxLayout();
//     centerRow->addStretch();
//     centerRow->addWidget(pomoStack, 1);
//     centerRow->addStretch();
//     layout->addLayout(centerRow, 1);   // 1 = забирает всё свободное место
//     //Страница задач
//     tasksPage = new QWidget();
//     QVBoxLayout* tasksLayout = new QVBoxLayout(tasksPage);
//     QHBoxLayout* headers = new QHBoxLayout();
//     QLabel* t_name = new QLabel(tr("Название задачи"));
//     t_name->setStyleSheet("background:transparent");
//     headers->addStretch();
//     headers->addWidget(t_name);
//     headers->addStretch();
//     QLabel* t_count = new QLabel(tr("Количество"));
//     headers->addWidget(t_count);
//     t_count->setStyleSheet("background:transparent");
//     headers->addStretch();
//     QLabel* t_done = new QLabel(tr("Выполнено"));
//     headers->addWidget(t_done);
//     t_done->setStyleSheet("background:transparent");
//     QLabel* t_start = new QLabel(tr("Старт"));
//     headers->addWidget(t_start);
//     t_start->setStyleSheet("background:transparent");
//     QLabel* t_del = new QLabel(tr("Удалить"));
//     headers->addWidget( t_del);
//     t_del->setStyleSheet("background:transparent");
//     tasksLayout->addLayout(headers);
//     // Скролл
//     createTaskScroll(tasksLayout);
//     //Страница активного помидора
//     pomodoroPage = new QWidget();
//     QVBoxLayout* pomoLayout = new QVBoxLayout(pomodoroPage);
//     activeTaskLabel = new QLabel(tr("Задача: —"));
//     activeTaskLabel->setStyleSheet(
//         "color: white; font-size: 24px; font-weight: bold;");
//     activeTaskLabel->setAlignment(Qt::AlignCenter);
//     pomoLayout->addWidget(activeTaskLabel);
//     //  Выбранная активность
//     p_activeActivityLabel = new QLabel(tr("Активность: —"), pomodoroPage);
//     p_activeActivityLabel->setStyleSheet(
//         "color: #ffd54f; font-size: 16px; font-weight: bold; background: transparent;");
//     p_activeActivityLabel->setAlignment(Qt::AlignCenter);
//     pomoLayout->addWidget(p_activeActivityLabel);
//     // Подзадача
//     p_activeSubtaskLabel = new QLabel(tr("Подзадача: —"), pomodoroPage);
//     p_activeSubtaskLabel->setStyleSheet(
//         "color: white; font-size: 20px; font-weight: bold; background: transparent;");
//     p_activeSubtaskLabel->setAlignment(Qt::AlignCenter);
//     pomoLayout->addWidget(p_activeSubtaskLabel);
//     pomoClock = new digital_clock(pomodoroPage);
//     pomoClock->setColor(Qt::white);
//     pomoClock->setShowSeconds(true);
//     pomoClock->setEditable(false);
//     pomoClock->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//     pomoLayout->addWidget(pomoClock, 0, Qt::AlignHCenter);
//     // Кнопки под часами
//     QHBoxLayout* pomoBtns = new QHBoxLayout();
//     pomoBtns->setAlignment(Qt::AlignHCenter);
//     pomoLayout->addLayout(pomoBtns);
//     pomoLayout->addStretch();
//     activeClock = pomoClock;
//     setupButtons(pomoBtns);
//     QPushButton* backBtn = new QPushButton(tr("← К задачам"));
//     pomoLayout->addWidget(backBtn);
//     // =Первая строка задачи
//     addTaskRow();
//     // Кнопка "Добавить задачу"
//     QPushButton* addBtn = new QPushButton(tr("Добавить задачу"));
//     tasksLayout->addWidget(addBtn);
//     //  Стек
//     pomoStack->addWidget(tasksPage);
//     pomoStack->addWidget(pomodoroPage);
//     pomoStack->setCurrentWidget(tasksPage);
//     //  Коннекты
//     connect(addBtn, &QPushButton::clicked, this, [this]() {
//         QLineEdit* newLine = addTaskRow();
//         if (newLine) newLine->setFocus();
//     });
//     connect(backBtn, &QPushButton::clicked, this, [this]() {
//         if (pomoClock) pomoClock->timer_stop();
//         finishCurrentSession(false);
//         if (pomoStack && tasksPage)
//             pomoStack->setCurrentWidget(tasksPage);
//         if (digitalClock) digitalClock->show();
//     });
//     //  Стили
//     tasksPage->setStyleSheet(
//         "QWidget { background: rgba(0,0,0,0.6); border-radius: 10px; }"
//         "QLabel { color: white; }"
//         "QLineEdit { background: white; color: black; border-radius: 5px; padding: 4px; }"
//         "QPushButton { background: rgba(255,255,255,0.2); color: white;"
//         "              border: 2px solid rgba(255,255,255,0.4);"
//         "              border-radius: 8px; padding: 6px; }"
//         "QPushButton:hover { background: rgba(255,255,255,0.35); }"
//         "QCheckBox { color: white; }"
//         "QScrollArea { background: transparent; border: none; }"
//         "QScrollBar:vertical {"
//         "    background: rgba(0,0,0,0.3);"
//         "    width: 10px;"
//         "    border-radius: 5px;"
//         "}"
//         "QScrollBar::handle:vertical {"
//         "    background: rgba(255,255,255,0.4);"
//         "    border-radius: 5px;"
//         "    min-height: 20px;"
//         "}"
//         "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
//         "    height: 0;"
//         "}"
//         );

//     pomodoroPage->setStyleSheet(
//         "QWidget { background: rgba(0,0,0,0.6); border-radius: 10px; }"
//         "QLabel { color: white; }"
//         "QPushButton { background: rgba(255,255,255,0.2); color: white;"
//         "              border: 2px solid rgba(255,255,255,0.4);"
//         "              border-radius: 8px; padding: 6px; }"
//         );
//     tasksLayout->setContentsMargins(8, 8, 8, 8);
// }

// void BaseTracker::hideEvent(QHideEvent* e)//отрабатывает при вызове hide()
// {
//     finishCurrentSession(false);
//     QWidget::hideEvent(e);
// }

// void BaseTracker::createTaskScroll(QVBoxLayout* tasksLayout)//установка скроллэрии
// {
//     if (!tasksLayout) return;
//     rowsContainer = new QWidget();
//     rowsLayout    = new QVBoxLayout(rowsContainer);
//     rowsLayout->setContentsMargins(0, 0, 0, 0);
//     rowsLayout->setSpacing(4);
//     rowsLayout->addStretch();
//     taskScroll = new QScrollArea();
//     taskScroll->setWidgetResizable(true);
//     taskScroll->setWidget(rowsContainer);
//     taskScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//     taskScroll->setFrameShape(QFrame::NoFrame);
//     tasksLayout->addWidget(taskScroll, 1);
//     tasksLayout->addStretch();
// }

// // поведение стека с тасками при расширении экрана
// void BaseTracker::resizeEvent(QResizeEvent *event)
// {
//     QWidget::resizeEvent(event);
//     // POMODORO
//     if (pomoStack) {
//         const int maxAllowed = qBound(250, width()/2-50, 550);
//         const int maxAllowedH = qBound(300, height()/2, 800);
//         pomoStack->setMinimumWidth(250);
//         pomoStack->setMaximumWidth(maxAllowed);
//         pomoStack->setMinimumHeight(300);
//         pomoStack->setMaximumHeight(maxAllowedH);
//         pomoStack->updateGeometry();
//         if (pomoClock) {
//             pomoClock->setMaximumWidth(maxAllowed);
//             pomoClock->setMinimumWidth(250);
//             pomoClock->updateGeometry();
//         }
//     }
//     // FLOWMODE / EATTHEFROG
//     if (flowTabs) flowTabs->updateGeometry();

//     //TIMEBLOCKING
//     if (mode == FTR::TimeBlocking && blockFrame) {
//         // Вычисление желаемой ширины: 50% от окна, но не меньше 300 и не больше 750
//         int desiredWidth = qBound(300, width() / 2-65, 750);
//         blockFrame->setMinimumWidth(desiredWidth);
//         blockFrame->setMaximumWidth(desiredWidth);
//         qDebug() << "TIMEBLOCKING resize: window width =" << width()
//                  << " frame width =" << desiredWidth;
//     }
// }

// //отрисовка интерфейса в зависимости от модуля
// void BaseTracker::add_mode_interface(QVBoxLayout* layout)
// {
//     if (!layout) return;
//     mode_layout = layout;
//     switch (mode){
//     case FlowMode:
//         addFlowModeInterface(layout, nullptr);
//         break;
//     case EatTheFrog:
//         addFlowModeInterface(layout, &BaseTracker::buildPriorityCombo);
//         break;
//     case TimeBlocking:
//         addTimeBlockingInterface(layout);
//         break;
//     case Pomodoro:
//         addTaskListInterface(layout, 25, 5);
//         break;
//     case ReversePomodoro:
//         addTaskListInterface(layout, 5, 25);
//         break;
//     case Mode60_10:
//         addTaskListInterface(layout, 60, 10);
//         break;
//     case Mode52_17:
//         addTaskListInterface(layout, 52, 17);
//         break;
//     case Mode90_20:
//         addTaskListInterface(layout, 90, 20);
//         break;
//     }
// }


// void BaseTracker::on_pushButton_clicked()
// {
//     // Остановка таймеров
//     stopAllTimers();
//     //Остановка последовательности задач
//     stopTaskSequence();          // для Flow/EatTheFrog
//     // Запись незавершённой сессии в БД
//     if (sessionStart.isValid())
//         finishCurrentSession(false);//помодоро и таймблокинг
//     //  Возврат на главное окно
//     getback_clicked();
// }

// void BaseTracker::stopAllTimers() {
//     auto stopClock = [](digital_clock* c){
//         if (!c) return;
//         if (c->getMode() == FTR::stopwatch) c->stopwatch_stop();
//         else c->timer_stop();
//     };
//     stopClock(activeClock);
//     stopClock(clock);
//     stopClock(digitalClock);
//     stopClock(digitalClock2);
//     stopClock(pomoClock);
//     if (taskTimer  && taskTimer->isActive())  taskTimer->stop();
//     if (blockTimer && blockTimer->isActive()) blockTimer->stop();
// }

// void BaseTracker::add_Task(const QString &arg1) {
//     QString name = arg1.trimmed();
//     if (name.isEmpty()) return;
//     QComboBox* combo = activityComboBox();
//     if (!combo) return;
//     combo->setCurrentText(name);
//     ensureTaskRegistered(combo);
// }

// void BaseTracker::loadTasksFromDb()//загрузка активности из бд
// {
//     refreshTaskCombo(activityComboBox());
// }

// void BaseTracker::on_pushButton_2_clicked()   // Секундомер
// {
//     if (mode == FTR::FlowMode){
//         QMessageBox::information(this, "Секундомер", "Режим секундомера");
//         digitalClock->setMode(stopwatch);
//         ensureButtonsCreated();
//         if (!mode_label) {
//             mode_label = new QLabel(this);
//             mode_label->setStyleSheet(
//                 "QLabel {"
//                 "   background: rgba(0, 0, 0, 0.6);"
//                 "   color: white;"
//                 "   border: 2px solid rgba(255, 255, 255, 0.3);"
//                 "   border-radius: 10px;"
//                 "   font-size: 14px;"
//                 "   font-weight: bold;"
//                 "   padding: 10px;"
//                 "}");
//             mode_label->setFixedSize(250, 50);
//             mode_label->setAlignment(Qt::AlignCenter);
//             parent_buttonLayout->insertWidget(0, mode_label, 1, Qt::AlignHCenter);
//         }
//         mode_label->setText("Секундомер");
//     }
//     else return;
// }

// void BaseTracker::on_pushButton_3_clicked()   // Таймер
// {
//     if (mode == FTR::FlowMode){
//         QMessageBox::information(this, "Таймер", "Режим таймера");
//         analogClock->setMode(timer);
//         digitalClock->setMode(timer);
//         ensureButtonsCreated();
//         if (!mode_label) {
//             mode_label = new QLabel(this);
//             mode_label->setStyleSheet(
//                 "QLabel {"
//                 "   background: rgba(0, 0, 0, 0.6);"
//                 "   color: white;"
//                 "   border: 2px solid rgba(255, 255, 255, 0.3);"
//                 "   border-radius: 10px;"
//                 "   font-size: 14px;"
//                 "   font-weight: bold;"
//                 "   padding: 10px;"
//                 "}"
//                 );
//             mode_label->setFixedSize(250, 50);
//             mode_label->setAlignment(Qt::AlignCenter);
//             parent_buttonLayout->insertWidget(0, mode_label, 1, Qt::AlignHCenter);
//         }
//         mode_label->setText("Таймер");
//     }
//     else return;
// }

// //обновление  окрана в зависимости от активности юзера
// void BaseTracker::showEvent(QShowEvent *event)
// {
//     QWidget::showEvent(event);
//     loadCurrentUser();
//     loadUserProfile();
//     connectInfoLink();
//     QComboBox* combo = activityComboBox();
//     if (combo && !comboConnected) {
//         combo->setEditable(true);
//         combo->setInsertPolicy(QComboBox::NoInsert);
//         if (combo->lineEdit()) {
//             connect(combo->lineEdit(), &QLineEdit::editingFinished,
//                     this, [this]() {
//                         QComboBox* c = activityComboBox();
//                         if (!c) return;
//                         QString name = c->currentText().trimmed();
//                         if (name.isEmpty()) return;
//                         if (c->findText(name, Qt::MatchFixedString) == -1) {
//                             add_Task(name);
//                         }
//                     });
//         }
//         loadTasksFromDb();
//         comboConnected = true;
//     }
// }

// //метод возврата на предыдущее окно
// void BaseTracker::getback_clicked()
// {
//     if (openWindow) {openWindow->show();}
//     this->hide();
// }

// //запись окончания сессии и отправка в бд
// void BaseTracker::finishCurrentSession(bool is_completed)
// {
//     if (!sessionStart.isValid()) return;
//     Data_Manager& db = Data_Manager::instance();
//     const qint64 uid = db.get_currentUser();
//     if (uid < 0) {
//         sessionStart = QDateTime();
//         return;
//     }
//     if (currentTaskId <= 0) {
//         qDebug() << "finishCurrentSession: SKIP, currentTaskId invalid =" << currentTaskId;
//         sessionStart = QDateTime();
//         return;
//     }
//     Session_info s;
//     s.user_id      = uid;
//     s.task_id      = currentTaskId;
//     s.technic      = mode;
//     s.phase        = currentPhase;
//     s.start_time   = sessionStart;
//     s.finish_time  = QDateTime::currentDateTime();
//     s.duration_sec = quint32(s.start_time.secsTo(s.finish_time));
//     s.completed    = is_completed;
//     if (s.duration_sec > 0)db.sendSessionData(s);
//     sessionStart = QDateTime();
//     qDebug() << "finishCurrentSession: phase =" << phaseToStr(currentPhase)
//              << " duration =" << s.duration_sec << " sec";
// }

// //установка фона
// void BaseTracker::paintEvent(QPaintEvent *event)
// {
//     Q_UNUSED(event);
//     QPainter p(this);
//     QPixmap bg(backgroundImage());
//     if (bg.isNull()) return;
//     QSize s = bg.size().scaled(size(), Qt::KeepAspectRatioByExpanding);
//     QPixmap scaled = bg.scaled(s, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
//     p.drawPixmap((width()  - scaled.width())  / 2,
//                  (height() - scaled.height()) / 2, scaled);
// }

// //размещение часов в лэйауте
// void BaseTracker::placeClocksInLayout(MyClock* cl,digital_clock* clock,QVBoxLayout *layout, QHBoxLayout *buttonlayout)
// {
//     if (!clock) return;
//     if (!layout) return;
//     parent_buttonLayout = layout;
//     if (buttonlayout && layout->indexOf(buttonlayout) != -1)
//         layout->removeItem(buttonlayout);
//     if (cl && layout->indexOf(cl) == -1)
//         layout->insertWidget(0, cl, 0, Qt::AlignHCenter);
//     if (clock && layout->indexOf(clock) == -1) {
//         int pos = (analogClock && layout->indexOf(cl) != -1) ? 1 : 0;
//         layout->insertWidget(pos, clock, 0, Qt::AlignHCenter);
//     }
//     if (buttonlayout) {
//         buttonLayout = buttonlayout;
//         int pos = layout->count();
//         if (clock) {
//             int clockIdx = layout->indexOf(clock);
//             if (clockIdx != -1) pos = clockIdx + 1;
//         }
//         layout->insertLayout(pos, buttonlayout, 0);   // stretch 0
//     }
// }


// void BaseTracker::playNotificationSound(){QApplication::beep();}//Звуковая нотификация

// void BaseTracker::ensureButtonsCreated()//установка кнопок
// {
//     if (!buttonLayout) return;
//     if (startBtn) return;
//     setupButtons(buttonLayout);
// }

// void BaseTracker::setupButtons(QHBoxLayout *buttonlayout)
// {
//     if (!buttonlayout) return;
//     // (создание startBtn, pauseBtn, stopBtn, стили)
//     startBtn = new QToolButton(this);  startBtn->setText("▶");
//     pauseBtn = new QToolButton(this);  pauseBtn->setText("❚❚");
//     stopBtn  = new QToolButton(this);  stopBtn->setText("■");
//     buttonlayout->addWidget(startBtn);
//     buttonlayout->addWidget(pauseBtn);
//     buttonlayout->addWidget(stopBtn);
//     startBtn->setToolTip("Старт");
//     pauseBtn->setToolTip("Пауза");
//     stopBtn->setToolTip("Стоп");
//     QString style =
//         "QToolButton { color: white; background: rgba(0, 0, 0, 0.6); font-weight:bold; }"
//         "QToolButton:hover { background: rgba(255, 0, 0, 0.6); }";
//     startBtn->setStyleSheet(style);
//     pauseBtn->setStyleSheet(style);
//     stopBtn->setStyleSheet(style);
//     // КНОПКА СТАРТ (▶)
//     connect(startBtn, &QToolButton::clicked, this, [this]() {
//         if (digitalClock2 && digitalClock2->getMode() == FTR::timer && digitalClock2->is_running())
//             return;
//         if (mode == FTR::EatTheFrog) {
//             startTaskSequence();
//             return;
//         }
//         else if (mode == FTR::TimeBlocking) {
//             if (digitalClock2 && digitalClock2->is_running()) return;
//             startSequence();
//             return;
//         }
//         else if (mode == Pomodoro || mode == ReversePomodoro ||
//                  mode == Mode60_10 || mode == Mode52_17 || mode == Mode90_20) {
//             if (!pomoClock) return;
//             // Если таймер уже тикает — ничего не делаем
//             if (pomoClock->is_running()) return;
//             // Если таймер на паузе (осталось время > 0) — продолжаем
//             if (pomoClock->get_timer_time() > 0) {
//                 pomoClock->timer_start(0, 0, 0); // нули, чтобы он паузу
//                 sessionStart = QDateTime::currentDateTime(); // возобновление сессии
//                 return;
//             }
//             // Если таймер дошел до нуля (был остановлен) - новая фаза
//             work = true;
//             currentPhase = Phase::Work;
//             if (activeTaskLabel)
//                 activeTaskLabel->setText(tr("Работа %1 мин").arg(workMinutes));
//             pomoClock->setMode(FTR::timer);
//             pomoClock->timer_start(0, workMinutes, 0);
//             sessionStart = QDateTime::currentDateTime();
//             return;
//         }
//         if (mode == FTR::FlowMode) {
//             qDebug() << "DEBUG: Нажата ОБЩАЯ кнопка Старт. Размер списка:" << flowTasksList.size()
//                      << "Индекс:" << activeFlowIndex;
//             // Проверка что список не пуст
//             if (flowTasksList.isEmpty()) {
//                 QMessageBox::information(this, "Внимание", "Список задач пуст. Добавьте задачу.");
//                 return;
//             }
//             // Если задача не выбрана (индекс -1), поиск первой с текстом
//             if (activeFlowIndex < 0 || activeFlowIndex >= flowTasksList.size()) {
//                 int validIdx = -1;
//                 for (int i = 0; i < flowTasksList.size(); ++i) {
//                     if (!flowTasksList[i].nameEdit->currentText().trimmed().isEmpty()) {
//                         validIdx = i;
//                         break;
//                     }
//                 }
//                 if (validIdx == -1) {
//                     QMessageBox::information(this, "Ошибка", "Нет заполненных задач.");
//                     return;
//                 }
//                 activeFlowIndex = validIdx;
//             }
//             //  индекс валиден, взятие задачи
//             FlowTask& ft = flowTasksList[activeFlowIndex];
//             // проверка текста
//             QString name = ft.nameEdit->currentText().trimmed();
//             if (name.isEmpty()) {
//                 QMessageBox::information(this, "Ошибка", "Введите название задачи перед запуском.");
//                 ft.nameEdit->setFocus();
//                 return;
//             }
//             //имитация клика мыши по кнопке
//             if (ft.startBtn) {
//                 ft.startBtn->click();
//             } else {
//                 QMessageBox::warning(this, "Ошибка", "Кнопка запуска для этой задачи не найдена. "
//                                                      "Попробуйте добавить задачу заново.");
//             }
//             return;
//         }
//     });
//     // КНОПКА ПАУЗА (❚❚)
//     connect(pauseBtn, &QToolButton::clicked, this, [this](){
//         if (mode == FTR::EatTheFrog) {
//             pauseTaskSequence();
//             return;
//         }
//         else if (mode == FTR::TimeBlocking) {
//             if (digitalClock2) digitalClock2->timer_pause();
//             finishCurrentSession(false);
//             if (blockTitleLabel)
//                 blockTitleLabel->setText(tr("Текущий блок: %1 (пауза)").arg(currentBlockName));
//             return;
//         }
//         else if (mode == FTR::FlowMode) {
//             if (digitalClock2) {
//                 if (digitalClock2->getMode() == FTR::stopwatch) digitalClock2->stopwatch_pause();
//                 else digitalClock2->timer_pause();
//             }
//             finishCurrentSession(false);
//             return;
//         }
//         // Для Помодоро
//         if (analogClock) {
//             switch (analogClock->get_mode()) {
//             case stopwatch: analogClock->stopwatch_pause(); break;
//             case timer:     analogClock->timer_pause();     break;
//             default: break;
//             }
//         }
//         digital_clock* c = activeClock ? activeClock : digitalClock2;
//         if (!c) return;
//         switch (c->getMode()) {
//         case stopwatch: c->stopwatch_pause(); break;
//         case timer:     c->timer_pause();     break;
//         default: break;
//         }
//         finishCurrentSession(false);
//     });

//     // КНОПКА СТОП (■)
//     connect(stopBtn, &QToolButton::clicked, this, [this]() {
//         if (mode == FTR::EatTheFrog) {
//             stopTaskSequence();
//             if (digitalClock2) {
//                 digitalClock2->timer_stop();
//                 digitalClock2->set_timer_time(0, 0, 0);
//             }
//             return;
//         }
//         else if (mode == FTR::FlowMode) {
//             stopFlowClockAndSession();
//             return;
//         }
//         else if (mode == FTR::TimeBlocking) {
//             if (blockTimer) blockTimer->stop();
//             if (digitalClock2) {
//                 digitalClock2->timer_stop();
//                 digitalClock2->set_timer_time(0, 0, 0);
//             }
//             finishCurrentSession(false);
//             currentBlockIndex = -1;
//             isBreakTime = false;
//             if (blockTitleLabel) blockTitleLabel->setText(tr("Текущий блок: -"));
//             if (blockRangeLabel) blockRangeLabel->setText(tr("- : -"));
//             return;
//         }
//         // Для Помодоро
//         if (analogClock) {
//             switch (analogClock->get_mode()) {
//             case stopwatch: analogClock->stopwatch_stop(); break;
//             case timer:     analogClock->timer_stop();     break;
//             default: break;
//             }
//         }
//         digital_clock* c = activeClock ? activeClock : digitalClock2;
//         if (!c) return;
//         switch (c->getMode()) {
//         case stopwatch: c->stopwatch_stop(); break;
//         case timer:     c->timer_stop();     break;
//         default: break;
//         }
//         finishCurrentSession(false);
//     });
// }

// bool BaseTracker::applyClockMode(FTR::ClockMode clockMode)
// {
//     if (mode == FTR::TimeBlocking||mode == FTR::FlowMode||mode == FTR::EatTheFrog){
//         if (digitalClock2) digitalClock2->setMode(clockMode);
//         ensureButtonsCreated();
//         return true;}
//     else
//         return false;
// }

// void BaseTracker::loadUserProfile()//загрузка фото профиля
// {
//     QLabel* av = avatarLabel();
//     if (!av) return;
//     Data_Manager& dm = Data_Manager::instance();
//     User_Profile me = dm.getCurrentUserProfile();
//     // ГОСТЬ
//     if (me.user_id < 0) {
//         QPixmap pix(":/img/exit (1).jpg");
//         if (!pix.isNull()) {
//             av->setPixmap(pix.scaled(180, 180,
//                                      Qt::KeepAspectRatio,
//                                      Qt::SmoothTransformation));
//             av->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
//         }
//         return;
//     }
//     // АВАТАР
//     if (!me.Avatar.isEmpty()) {
//         QPixmap avatar;
//         if (avatar.loadFromData(me.Avatar)) {
//             av->setPixmap(avatar.scaled(180, 180,
//                                         Qt::KeepAspectRatio,
//                                         Qt::SmoothTransformation));
//             av->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
//             return;
//         }
//     }
//     // ЗАГЛУШКА
//     av->setPixmap(QPixmap(":/img/icon_question.png")
//                       .scaled(180, 180,
//                               Qt::KeepAspectRatio,
//                               Qt::SmoothTransformation));
//     av->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
// }

// void BaseTracker::loadCurrentUser()//загрузка профиля юзера
// {
//     QLabel* ul = userLabel();
//     if (!ul) return;
//     Data_Manager& dm = Data_Manager::instance();
//     User_Profile me = dm.getCurrentUserProfile();
//     if (me.user_id < 0) {
//         ul->setText("<a href=\"#guest\" style=\"color: white; font-weight: bold;\">Гость</a>");
//         return;
//     }
//     ul->setText(
//         QString("<a href=\"#open\" style=\"color: white; font-weight: bold;\">"
//                 "%1 %2</a>")
//             .arg(me.Name, me.Surname));
// }

// void BaseTracker::addTimeBlockingInterface(QVBoxLayout* layout)
// {
//     if (!layout) return;
//     if (digitalClock2) {
//         digitalClock2->setEditable(false);
//         disconnect(digitalClock2, &digital_clock::timerFinished, this, nullptr);
//     }
//     QFrame* frame = new QFrame(this);
//     frame->setObjectName("blockFrame");
//     frame->setStyleSheet(
//         "QFrame#blockFrame { background: rgba(1,1,1,0.6); border-radius: 10px; }"
//         "QLabel { color: white; }");
//     frame->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
//     this->blockFrame = frame;
//     QHBoxLayout* centerRow = new QHBoxLayout();
//     centerRow->addStretch();
//     centerRow->addWidget(frame, 1);
//     layout->addLayout(centerRow, 1);
//     QVBoxLayout* vbox = new QVBoxLayout(frame);
//     vbox->setContentsMargins(8, 8, 8, 8);
//     vbox->setSpacing(4);
//     // Карточка текущего блока (внутри фрэйма)
//     QWidget* currentCard = new QWidget(frame);
//     currentCard->setObjectName("currentCard");
//     currentCard->setStyleSheet(
//         "QWidget#currentCard { background: rgba(255,255,255,0.1); border-radius: 8px; }"
//         "QLabel { color: white; }");
//     QVBoxLayout* curLayout = new QVBoxLayout(currentCard);
//     curLayout->setContentsMargins(8, 8, 8, 8);
//     curLayout->setSpacing(4);
//     QHBoxLayout* btnRow = new QHBoxLayout();
//     btnRow->setContentsMargins(0, 0, 0, 0);
//     btnRow->setSpacing(6);
//     vbox->addLayout(btnRow);
//     placeClocksInLayout(nullptr,digitalClock2,vbox,btnRow);
//     setupButtons(btnRow);
//     vbox->addWidget(currentCard);
//     blockTitleLabel = new QLabel(tr("Текущий блок: -"), currentCard);
//     blockTitleLabel->setStyleSheet("font-size: 20px; font-weight: bold;");
//     blockTitleLabel->setAlignment(Qt::AlignCenter);
//     curLayout->addWidget(blockTitleLabel,1);
//     blockRangeLabel = new QLabel(tr("- : -"), currentCard);
//     blockRangeLabel->setAlignment(Qt::AlignCenter);
//     curLayout->addWidget(blockRangeLabel);
//     //  Расписание дня
//     QLabel* scheduleLabel = new QLabel(tr("Расписание дня"), this);
//     scheduleLabel->setStyleSheet("color: white; font-weight: bold;");
//     scheduleLabel->setAlignment(Qt::AlignCenter);
//     layout->addWidget(scheduleLabel);
//     vbox->addWidget(scheduleLabel);
//     QHBoxLayout* columns = new QHBoxLayout();
//     vbox->addLayout(columns);
//     columns->addStretch();
//     QLabel* task_name = new QLabel("Название задачи");
//     columns->addWidget(task_name );
//     columns->addStretch();
//     QLabel* task_time = new QLabel("Время задачи");
//     columns->addWidget(task_time);
//     QLabel* break_time = new QLabel("Время отдыха");
//     columns->addWidget(break_time);
//     QLabel* start_task = new QLabel("Старт");
//     columns->addWidget(start_task);
//     QLabel* delete_task = new QLabel("Удалить");
//     columns->addWidget(delete_task);
//     blockRowsContainer = new QWidget(this);
//     blockRowsLayout = new QVBoxLayout(blockRowsContainer);
//     blockRowsLayout->setContentsMargins(0, 0, 0, 0);
//     blockRowsLayout->setSpacing(4);
//     blockRowsLayout->addStretch();
//     QScrollArea* scroll = new QScrollArea(this);
//     scroll->setWidgetResizable(true);
//     scroll->setWidget(blockRowsContainer);
//     scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//     scroll->setFrameShape(QFrame::NoFrame);
//     scroll->setStyleSheet(
//         "QScrollArea { background: transparent; border: none; }"
//         "QScrollArea > QWidget > QWidget { background: transparent; }"
//         );
//     vbox->addWidget(scroll, 1);
//     // Кнопка «Добавить блок»
//     QPushButton* addBtn = new QPushButton(tr("+ Добавить блок"), this);
//     addBtn->setStyleSheet(
//         "QPushButton {"
//         "  color: black;font-weight: bold;"
//         "  background-color: rgba(255,255,255,0.5);"
//         "  border: 1px solid rgba(255,255,255,0.4);"
//         "  border-radius: 4px;"
//         "  padding: 2px;"
//         "}"
//         "QPushButton:hover {"
//         "  background-color: rgba(255,0,0,0.5);"
//         "}"
//         );
//     vbox->addWidget(addBtn);
//     connect(addBtn, &QPushButton::clicked, this, [this]() {
//         QWidget* row = buildBlockRow(tr(""), 60, 15);
//         blockRowsLayout->insertWidget(blockRowsLayout->count() - 1, row);
//     });
//     QWidget* first = buildBlockRow("", 30, 10);
//     blockRowsLayout->insertWidget(blockRowsLayout->count() - 1, first);
// }

// QWidget* BaseTracker::buildBlockRow(const QString& name, int durationMin, int bufferMin)
// {
//     QWidget* row = new QWidget();
//     row->setStyleSheet("QWidget { background: rgba(0,0,0,0.5); border-radius: 8px; } "
//                        "QLabel { color: black; }");
//     QHBoxLayout* h = new QHBoxLayout(row);
//     h->setContentsMargins(6, 4, 6, 4);
//     // Комбобокс задачи
//     QComboBox* taskCombo = new QComboBox(row);
//     taskCombo->setEditable(true);
//     taskCombo->setInsertPolicy(QComboBox::NoInsert);
//     taskCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
//     taskCombo->setMinimumWidth(90);
//     taskCombo->setFixedHeight(22);
//     taskCombo->setStyleSheet(
//         "QComboBox { color: black; font-weight: bold; background-color: rgba(255,255,255,0.5); "
//         "  border: 1px solid rgba(255,255,255,0.4); border-radius: 4px; padding: 2px; }"
//         "QComboBox:hover { background-color: rgba(255,0,0,0.5); }"
//         "QComboBox QLineEdit { background: transparent; border: none; color: black; }"
//         "QComboBox QAbstractItemView { background-color: rgba(30,30,30,0.9); color: white; "
//         "  selection-background-color: rgba(255,0,0,0.5); }"
//         );
//     // Заполняем существующими задачами из БД
//      refreshTaskCombo(taskCombo);
//     // Если передали название при создании
//     if (!name.isEmpty()) {
//         int idx = taskCombo->findText(name, Qt::MatchFixedString);
//         if (idx >= 0) {
//             taskCombo->setCurrentIndex(idx);
//         } else {
//             taskCombo->setCurrentText(name);
//         }
//     }
//     taskCombo->setPlaceholderText(tr("Введите или выберите задачу"));
//     h->addWidget(taskCombo, 0);
//     // Поле минут работы
//     QSpinBox* durSpin = new QSpinBox(row);
//     durSpin->setStyleSheet(
//         "QSpinBox { color: black; font-weight: bold; background-color: rgba(255,255,255,0.5); "
//         "  border: 1px solid rgba(255,255,255,0.4); border-radius: 4px; padding: 2px; }"
//         "QSpinBox:hover { background-color: rgba(255,0,0,0.5); }"
//         );
//     durSpin->setRange(1, 999);
//     durSpin->setValue(durationMin);
//     durSpin->setToolTip(tr("Введите время блока"));
//     durSpin->setSuffix(tr(" мин"));
//     h->addWidget(durSpin);
//     // Поле минут буфера
//     QSpinBox* bufSpin = new QSpinBox(row);
//     bufSpin->setStyleSheet(
//         "QSpinBox { color: black; font-weight: bold; background-color: rgba(255,255,255,0.5); "
//         "  border: 1px solid rgba(255,255,255,0.4); border-radius: 4px; padding: 2px; }"
//         "QSpinBox:hover { background-color: rgba(255,0,0,0.5); }"
//         );
//     bufSpin->setRange(0, 999);
//     bufSpin->setValue(bufferMin);
//     bufSpin->setToolTip(tr("Введите время перерыва"));
//     bufSpin->setSuffix(tr(" мин"));
//     h->addWidget(bufSpin);
//     // Кнопки
//     QPushButton* startBtn = new QPushButton("▶", row);
//     startBtn->setFixedWidth(30);
//     startBtn->setStyleSheet(
//         "QPushButton { color: black; font-weight: bold; background-color: rgba(255,255,255,0.5); "
//         "  border: 1px solid rgba(255,255,255,0.4); border-radius: 4px; padding: 2px; }"
//         "QPushButton:hover { background-color: rgba(255,0,0,0.5); }");
//     QPushButton* removeBtn = new QPushButton("✕", row);
//     removeBtn->setFixedWidth(30);
//     removeBtn->setStyleSheet(
//         "QPushButton { color: black; font-weight: bold; background-color: rgba(255,255,255,0.5); "
//         "  border: 1px solid rgba(255,255,255,0.4); border-radius: 4px; padding: 2px; }"
//         "QPushButton:hover { background-color: rgba(255,0,0,0.5); }"
//         );
//     h->addWidget(startBtn);
//     h->addWidget(removeBtn);
//     // Сохраняем структуру в модель
//     TimeBlock block;
//     block.name        = name;
//     block.durationMin = durationMin;
//     block.bufferMin   = bufferMin;
//     block.rowWidget   = row;
//     block.taskCombo   = taskCombo;
//     block.durSpin     = durSpin;
//     block.bufSpin     = bufSpin;
//     block.taskId      = taskCombo->currentData().toLongLong();
//     timeBlocks.append(block);
//     //  РЕГИСТРАЦИЯ ЗАДАЧИ В БД СРАЗУ ПРИ ВВОДЕ (editingFinished)
//     // Регистрация задачи при Enter
//     if (taskCombo->lineEdit()) {
//         if (taskCombo->lineEdit()) {
//             connect(taskCombo->lineEdit(), &QLineEdit::returnPressed,
//                     this, [this, taskCombo]() {
//                         // Закрытие popup, иначе Enter обработает сам QComboBox
//                         taskCombo->hidePopup();
//                         const qint64 id = ensureTaskRegistered(taskCombo);
//                         if (id > 0) {
//                             for (auto& b : timeBlocks) {
//                                 if (b.taskCombo == taskCombo) {
//                                     b.name   = taskCombo->currentText().trimmed();
//                                     b.taskId = id;
//                                     break;
//                                 }
//                             }
//                         }
//                     });
//         }
//         // Регистрация при выборе из выпадающего списка
//         connect(taskCombo, QOverload<int>::of(&QComboBox::activated),
//                 this, [this, taskCombo](int) {
//             ensureTaskRegistered(taskCombo);
//         });
//         connect(this, &BaseTracker::tasksChanged, taskCombo, [this, taskCombo]{
//             refreshTaskCombo(taskCombo);
//         });
//     }
//     //  СТАРТ КОНКРЕТНОГО БЛОКА
//     //  поиск индекса в момент клика по rowWidget
//     connect(startBtn, &QPushButton::clicked, this, [this, row, taskCombo]() {
//         if (digitalClock2 && digitalClock2->is_running()) return;
//         // Гарантия,что введённая задача попадёт в БД
//         const qint64 id = ensureTaskRegistered(taskCombo);
//         if (id <= 0) {
//             QMessageBox::information(this, tr("Внимание"),
//                                      tr("Введите название задачи перед запуском блока."));
//             taskCombo->setFocus();
//             return;
//         }
//         // поиск индекса блока
//         int idx = -1;
//         for (int i = 0; i < timeBlocks.size(); ++i) {
//             if (timeBlocks[i].rowWidget == row) { idx = i; break; }
//         }
//         if (idx < 0) return;
//         syncTimeBlocks();
//         currentBlockIndex = idx;
//         isBreakTime = false;
//         processCurrentBlock();
//     });
//     //  УДАЛЕНИЕ СТРОКИ
//     connect(removeBtn, &QPushButton::clicked, this, [this, row]() {
//         // поиск индекса блока
//         int idx = -1;
//         for (int i = 0; i < timeBlocks.size(); ++i) {
//             if (timeBlocks[i].rowWidget == row) { idx = i; break; }
//         }
//         if (idx < 0) return;
//         // при удалении текущего работающего — стоп
//         if (currentBlockIndex == idx) {
//             if (blockTimer) blockTimer->stop();
//             if (digitalClock2) {
//                 digitalClock2->timer_stop();
//                 digitalClock2->set_timer_time(0, 0, 0);
//             }
//             finishCurrentSession(false);
//             currentBlockIndex = -1;
//             isBreakTime = false;
//             if (blockTitleLabel) blockTitleLabel->setText(tr("Текущий блок: -"));
//             if (blockRangeLabel) blockRangeLabel->setText(tr("- : -"));
//         }
//         // при удалении блока до текущего - сдвиг индекса
//         else if (currentBlockIndex > idx) {
//             currentBlockIndex--;
//         }
//         // Удаление из модели
//         timeBlocks.removeAt(idx);
//         // удаление из layout
//         if (blockRowsLayout) {
//             blockRowsLayout->removeWidget(row);
//         }
//         // Удаление виджета
//         row->deleteLater();
//     });
//     return row;
// }

// void BaseTracker::syncTimeBlocks()
// {
//     for (auto& b : timeBlocks) {
//         if (!b.taskCombo) continue;
//         b.name   = b.taskCombo->currentText().trimmed();
//         b.taskId = ensureTaskRegistered(b.taskCombo);
//         if (b.durSpin) b.durationMin = b.durSpin->value();
//         if (b.bufSpin) b.bufferMin   = b.bufSpin->value();
//     }
// }

// qint64 BaseTracker::ensureTaskRegistered(QComboBox* combo)
// {
//     if (!combo) return -1;
//     const qint64 uid = Data_Manager::instance().get_currentUser();
//     if (uid < 0) return -1;
//     const QString name = combo->currentText().trimmed();
//     if (name.isEmpty()) return -1;
//     // поиск задачи с таким именем в БД
//     const QList<Task_Info> all = Data_Manager::instance().getTasksForUser(uid);
//     for (const auto& t : all) {
//         if (t.task_name == name) {
//             // если задача есть-привяка ID к текущему комбо
//             int idx = combo->findText(name, Qt::MatchFixedString);
//             if (idx >= 0) combo->setItemData(idx, t.task_id);
//             return t.task_id;
//         }
//     }
//     // Если нет — создание новой
//     Task_Info ti;
//     ti.task_name = name;
//     ti.user_id   = uid;
//     const qint64 newId = Data_Manager::instance().sendNewTask(ti);
//     if (newId <= 0) return -1;
//     // сигнал комбобоксам, что появилась новая задача
//     emit tasksChanged();
//     return newId;
// }

// void BaseTracker::processCurrentBlock()
// {
//     // Если выход за пределы списка — завершение
//     if (currentBlockIndex < 0 || currentBlockIndex >= timeBlocks.size()) {
//         if (blockTitleLabel) blockTitleLabel->setText(tr("Все блоки завершены"));
//         if (blockRangeLabel) blockRangeLabel->setText(tr("- : -"));
//         currentBlockIndex = -1;
//         if (digitalClock2) {
//             digitalClock2->timer_stop();
//             digitalClock2->set_timer_time(0, 0, 0);
//         }
//         return;
//     }
//     TimeBlock& b = timeBlocks[currentBlockIndex];
//     if (!isBreakTime) {
//         // РАБОТА
//         isBreakTime = true;
//         currentPhase = Phase::Work;
//         // УСТАНАВЛИВАЕМ ID ЗАДАЧИ ДЛЯ СЕССИИ
//         currentTaskId = b.taskId;
//         if (blockTitleLabel)
//             blockTitleLabel->setText(tr("Работа: %1").arg(b.name.isEmpty() ?
//                                                               tr("Блок %1").arg(currentBlockIndex+1) :
//                                                               b.name));
//         if (blockRangeLabel)
//             blockRangeLabel->setText(tr("%1 мин").arg(b.durationMin));
//         if (digitalClock2) {
//             digitalClock2->timer_stop();
//             digitalClock2->setMode(FTR::timer);
//             digitalClock2->set_timer_time(0, 0, 0);
//             digitalClock2->timer_start(0, 0, b.durationMin * 60);
//         }
//         sessionStart = QDateTime::currentDateTime();
//         if (blockTimer) blockTimer->start(b.durationMin * 60 * 1000);
//     } else {
//         // ПЕРЕРЫВ
//         isBreakTime = false;
//         currentPhase = Phase::Break;
//         // Во время перерыва мы тоже привязываем сессию к этой же задаче
//         currentTaskId = b.taskId;
//         if (b.bufferMin > 0) {
//             if (blockTitleLabel) blockTitleLabel->setText(tr("Перерыв"));
//             if (blockRangeLabel) blockRangeLabel->setText(tr("%1 мин").arg(b.bufferMin));
//             if (digitalClock2) {
//                 digitalClock2->timer_stop();
//                 digitalClock2->setMode(FTR::timer);
//                 digitalClock2->set_timer_time(0, 0, 0);
//                 digitalClock2->timer_start(0, 0, b.bufferMin * 60);
//             }
//             sessionStart = QDateTime::currentDateTime();
//             if (blockTimer) blockTimer->start(b.bufferMin * 60 * 1000);
//         } else {
//             // Если перерыва нет, переход к следующему блоку
//             currentBlockIndex++;
//             QTimer::singleShot(0, this, &BaseTracker::processCurrentBlock);
//         }
//     }
// }


// void BaseTracker::startSequence()
// {
//     if (digitalClock2) {
//         disconnect(digitalClock2, &digital_clock::timerFinished, this, nullptr);
//     }
//     syncTimeBlocks();
//     // Удаление из списка блоков, у которых виджеты были удалены
//     for (int i = timeBlocks.size() - 1; i >= 0; --i) {
//         if (!timeBlocks[i].rowWidget || !timeBlocks[i].taskCombo) {
//             timeBlocks.removeAt(i);
//         }
//     }
//     if (timeBlocks.isEmpty()) {
//         QMessageBox::information(this, "Ошибка", "Нет добавленных блоков");
//         return;
//     }
//     currentBlockIndex = 0;
//     isBreakTime = false;
//     processCurrentBlock();
// }

// void BaseTracker::onBlockTimerFinished()
// {
//     finishCurrentSession(true);
//     playNotificationSound();
//     if (!isBreakTime) {
//         // Значит закончился перерыв. Переход к следующему блоку.
//         currentBlockIndex++;
//     }
//     // Если isBreakTime == true, значит закончилась работа, и processCurrentBlock запустит перерыв,
//     //не увеличивая индекс
//     processCurrentBlock();
// }

// unsigned BaseTracker::parseDurationMin(const QString& text)
// {
//     QString s = text.trimmed();
//     if (s.isEmpty()) return 0;
//     if (s.contains(':')) {
//         const QStringList parts = s.split(':');
//         if (parts.size() != 2) return 0;
//         bool okH = false, okM = false;
//         int h = parts[0].toInt(&okH);
//         int m = parts[1].toInt(&okM);
//         if (!okH || !okM || h < 0 || m < 0 || m >= 60) return 0;
//         return unsigned(h * 60 + m);
//     }
//     bool ok = false;
//     int v = s.toInt(&ok);
//     if (!ok || v < 0) return 0;
//     return unsigned(v);
// }

// void BaseTracker::stopFlowClockAndSession()
// {
//     //Закрытие текущей сессии до сброса currentTaskId
//     finishCurrentSession(false);
//     // Невидимый таймер последовательности (EatTheFrog)
//     if (taskTimer && taskTimer->isActive()) {
//         taskTimer->stop();
//     }
//     // Визуальный таймер/секундомер
//     if (digitalClock2) {
//         if (digitalClock2->getMode() == FTR::stopwatch) {
//             digitalClock2->stopwatch_stop();   // остановить секундомер
//         } else {
//             digitalClock2->timer_stop();       // остановить таймер
//         }
//         digitalClock2->set_timer_time(0, 0, 0); // визуально 00:00:00
//     }
//     // Сброс привязки к задаче - сессия закрыта
//     currentTaskId = -1;
//     sessionStart  = QDateTime();
// }

// void BaseTracker::collectTasksFromRows()
// {
//     if (todolist) {
//         delete todolist;
//         todolist = nullptr;
//     }
//     todolist = new QList<todo>();
//     if (!rowsLayout) return;
//     for (int i = 0; i < rowsLayout->count(); ++i) {
//         QHBoxLayout* row = qobject_cast<QHBoxLayout*>(rowsLayout->itemAt(i)->layout());
//         if (!row) continue;
//         // Сбор виджетов строки в порядке добавления
//         QList<QWidget*> w;
//         for (int j = 0; j < row->count(); ++j) {
//             if (QWidget* ww = row->itemAt(j)->widget())
//                 w.append(ww);
//         }
//         // Ожидание минимума: taskCombo, timeLine, done, startBtn, removeBtn
//         if (w.size() < 5) continue;
//         // Последние 5 виджетов строки идут в фиксированном порядке:
//         // [taskCombo, timeEdit, done, startBtn, removeBtn]
//         auto* taskCombo = qobject_cast<QComboBox*>(w.at(w.size() - 5));
//         auto* timeEdit  = qobject_cast<QLineEdit*>(w.at(w.size() - 4));
//         auto* doneChk   = qobject_cast<QCheckBox*>(w.at(w.size() - 3));
//         if (!taskCombo) continue;
//         const QString name = taskCombo->currentText().trimmed();
//         if (name.isEmpty()) continue;
//         // ЕДИНАЯ ТОЧКА РЕГИСТРАЦИИ
//         // Возвращает существующий id либо создаёт задачу в БД
//         // эмитит tasksChanged()-ensureTaskRegistered
//         const qint64 taskId = ensureTaskRegistered(taskCombo);
//         if (taskId <= 0) continue;   // нет пользователя / пустое имя — пропуск
//         // Приоритет (только для EatTheFrog)
//         int priority = 1; // по умолчанию «Элементарно»
//         for (QWidget* ww : w) {
//             if (auto* cb = qobject_cast<QComboBox*>(ww)) {
//                 if (cb != taskCombo) {
//                     priority = cb->currentData().toInt();
//                     break;
//                 }
//             }
//         }
//         todo t;
//         t.task.task_name = name;
//         t.task.task_id = taskId;
//         t.frog = static_cast<EatTheFrog_dificulties>(priority);
//         t.duration = timeEdit ? parseDurationMin(timeEdit->text()) : 0u;
//         t.done = doneChk ? doneChk->isChecked() : false;
//         todolist->append(t);
//     }
// }

// void BaseTracker::refreshTaskCombo(QComboBox* combo) {
//     if (!combo) return;
//     const qint64 uid = Data_Manager::instance().get_currentUser();
//     const QString current = combo->currentText();
//     QSignalBlocker block(combo);
//     combo->clear();
//     if (uid >= 0)
//         for (const auto& t : Data_Manager::instance().getTasksForUser(uid))
//             combo->addItem(t.task_name, t.task_id);
//     if (!current.isEmpty()) {
//         int idx = combo->findText(current, Qt::MatchFixedString);
//         if (idx >= 0) combo->setCurrentIndex(idx);
//         else          combo->setCurrentText(current);
//     }
// }

// void BaseTracker::sortTasksByPriority()
// {
//     if (!todolist) return;
//     std::stable_sort(todolist->begin(), todolist->end(),
//                      [](const todo& a, const todo& b) {
//                          return a.frog > b.frog;
//                      });
// }

// void BaseTracker::startTaskSequence(int startIndex)
// {
//     stopTaskSequence();
//     collectTasksFromRows();
//     sortTasksByPriority();
//     if (!todolist || todolist->isEmpty()) {
//         if (sequenceStatusLabel)
//             sequenceStatusLabel->setText(tr("Список задач пуст"));
//         return;
//     }
//     if (!taskTimer) {
//         taskTimer = new QTimer(this);
//         taskTimer->setSingleShot(true);
//         connect(taskTimer, &QTimer::timeout, this,
//                 [this]{ completeCurrentTask(true); });
//     }

//     // поиск первой невыполненной задачи
//     currentTaskIndex = -1;
//     for (int i = 0; i < todolist->size(); ++i) {
//         if (!(*todolist)[i].done) {
//             currentTaskIndex = i;
//             break;
//         }
//     }
//     // Если все задачи уже сделаны - сообщение
//     if (currentTaskIndex == -1) {
//         if (sequenceStatusLabel)
//             sequenceStatusLabel->setText(tr("Все задачи выполнены ✔"));
//         return;
//     }
//     startNextTask();
// }

// void BaseTracker::startNextTask()
// {
//     if (!todolist || currentTaskIndex < 0
//         || currentTaskIndex >= todolist->size()) {
//         if (sequenceStatusLabel)
//             sequenceStatusLabel->setText(tr("Все задачи выполнены ✔"));
//         if (taskTimer && taskTimer->isActive()) taskTimer->stop();

//         // Останавливаем и сбрасываем визуальный таймер в ноль
//         if (digitalClock2) {
//             digitalClock2->timer_stop();
//             digitalClock2->set_timer_time(0, 0, 0);
//         }
//         currentTaskIndex = -1;
//         return;
//     }
//     const todo& t = (*todolist)[currentTaskIndex];
//     if (sequenceStatusLabel) {
//         sequenceStatusLabel->setText(
//             tr("Задача %1 из %2: %3 (%4 мин)")
//                 .arg(currentTaskIndex + 1)
//                 .arg(todolist->size())
//                 .arg(t.task.task_name)
//                 .arg(t.duration));
//     }
//     // Сессия в БД
//     finishCurrentSession(false);
//     Task_Info ti;
//     Data_Manager& db = Data_Manager::instance();
//     ti.task_name = t.task.task_name;
//     ti.user_id   = db.get_currentUser();
//     currentTaskId = db.sendNewTask(ti);
//     sessionStart  = QDateTime::currentDateTime();
//     currentPhase  = Phase::Work;
//     // ЖЕСТКИЙ СБРОС И СТАРТ ВИЗУАЛЬНОГО ТАЙМЕРА
//     digital_clock* c = digitalClock2 ? digitalClock2 : digitalClock;
//     if (c) {
//         c->timer_stop(); // Остановка
//         c->setMode(FTR::timer); // режим таймера
//         c->set_timer_time(0, 0, 0); // сброс в 00:00:00
//         c->reset_all_time();// сброс внутренних переменных
//         if (t.duration > 0) {//Запуск нового времени (костыль: минуты в секунды)
//             c->timer_start(0, 0, int(t.duration * 60));
//         }
//     }
//     //Сброс и старт невидимого таймера
//     if (taskTimer) {
//         if (taskTimer->isActive()) taskTimer->stop();
//         if (t.duration > 0) {
//             taskTimer->start(int(t.duration) * 60 * 1000);
//         }
//     }
// }

// void BaseTracker::completeCurrentTask(bool done)
// {
//     if (!todolist || currentTaskIndex < 0
//         || currentTaskIndex >= todolist->size())
//         return;
//     todo& t = (*todolist)[currentTaskIndex];
//     t.done = done;
//     finishCurrentSession(done);
//     playNotificationSound();
//     if (taskTimer && taskTimer->isActive()) taskTimer->stop();
//     ++currentTaskIndex;
//     startNextTask();
// }

// void BaseTracker::stopTaskSequence()
// {
//     stopFlowClockAndSession();
//     currentTaskIndex = -1;
//     if (sequenceStatusLabel)
//         sequenceStatusLabel->setText(tr("Последовательность остановлена"));
// }

// void BaseTracker::sortFlowRows(QVBoxLayout* rowsLayout)
// {
//     if (!rowsLayout) return;
//     QList<QHBoxLayout*> rows;
//     for (int i = 0; i < rowsLayout->count(); ++i) {
//         if (auto* h = qobject_cast<QHBoxLayout*>(rowsLayout->itemAt(i)->layout()))
//             rows.append(h);
//     }
//     auto prioOf = [](QHBoxLayout* row) -> int {
//         for (int j = 0; j < row->count(); ++j) {
//             if (auto* cb = qobject_cast<QComboBox*>(row->itemAt(j)->widget()))
//                 return cb->currentData().toInt();
//         }
//         return 1;
//     };
//     std::stable_sort(rows.begin(), rows.end(),
//                      [&](QHBoxLayout* a, QHBoxLayout* b){
//                          return prioOf(a) > prioOf(b);
//                      });
//     // Перестановка в layout
//     while (rowsLayout->count() > 0)
//         rowsLayout->takeAt(0);
//     for (auto* r : rows)
//         rowsLayout->addLayout(r);
//     rowsLayout->addStretch();
// }

// void BaseTracker::pauseTaskSequence()
// {
//     if (taskTimer && taskTimer->isActive()) {
//         taskTimer->stop();
//     }
//     if (digitalClock2) {
//         digitalClock2->timer_pause(); // останавливает визуальный таймер
//     }
//     finishCurrentSession(false);
//     if (sequenceStatusLabel)
//         sequenceStatusLabel->setText(tr("Пауза"));
// }

// void BaseTracker::resumeTaskSequence()
// {
//     if (currentTaskIndex < 0 || !todolist || currentTaskIndex >= todolist->size()) {
//         startTaskSequence(); // сначала, если не запущено
//         return;
//     }
//     const todo& t = (*todolist)[currentTaskIndex];
//     // Визуальный таймер
//     if (digitalClock2) {
//         digitalClock2->timer_start(0, 0, 0);
//     }
//     // Невидимый таймер
//     if (taskTimer && t.duration > 0) {
//         taskTimer->start(int(t.duration) * 60 * 1000);
//     }
//     sessionStart = QDateTime::currentDateTime();
//     if (sequenceStatusLabel)
//         sequenceStatusLabel->setText(tr("Задача %1 из %2 (продолжение)").arg(currentTaskIndex + 1)
//                                          .arg(todolist->size()));
// }

// void BaseTracker::updateClockEditable()
// {
//     if (!digitalClock2) return;
//     if (mode == FTR::EatTheFrog) {
//         digitalClock2->setEditable(false);
//         return;
//     }
//     if (mode == FTR::FlowMode) {
//         bool hasActiveTask = (activeFlowIndex >= 0
//                               && activeFlowIndex < flowTasksList.size());
//         digitalClock2->setEditable(hasActiveTask);
//         qDebug() << "updateClockEditable: FlowMode, hasActiveTask =" << hasActiveTask
//                  << " activeFlowIndex =" << activeFlowIndex
//                  << " size =" << flowTasksList.size();
//         return;
//     }
//     digitalClock2->setEditable(false);
// }

// bool BaseTracker::eventFilter(QObject *obj, QEvent *event)
// {
//     if (mode != FTR::FlowMode) {
//         return QWidget::eventFilter(obj, event);
//     }
//     if (event->type() == QEvent::MouseButtonPress) {
//         // Если клик на строку с задачей
//         QVariant idVar = obj->property("taskIndex");
//         if (idVar.isValid()) {
//             selectFlowTask(idVar.toInt());
//         }
//         // Если клик на любой контейнер (пустое место)
//         else if (obj->isWidgetType()) {
//             QWidget* w = qobject_cast<QWidget*>(obj);
//             if (!qobject_cast<QLineEdit*>(w) &&
//                 !qobject_cast<QPushButton*>(w) &&
//                 !qobject_cast<QToolButton*>(w) &&
//                 !qobject_cast<QComboBox*>(w) &&
//                 !qobject_cast<QCheckBox*>(w))
//             {
//                 // Снятие выделения
//                 activeFlowIndex = -1;
//                 updateClockEditable();
//                 // Возврат обычного стиля
//                 QString normalComboStyle =
//                     "QComboBox { color: black; font-weight: bold; background-color: "
//                                            "rgba(255,255,255,0.5); "
//                                            "border: 1px solid rgba(255,255,255,0.4); "
//                                            "border-radius: 4px; padding: 2px; }"
//                     "QComboBox:hover { background-color: rgba(255,0,0,0.3); }"
//                     "QComboBox:focus { background-color: rgba(255, 0, 0, 0.5); color: black; "
//                                            "border: 1px solid rgba(255,255,255,0.8); }"
//                     "QComboBox QLineEdit { background: transparent; border: none; color: black; }";
//                 QString normalLineStyle =
//                     "QLineEdit { color: black; font-weight: bold; "
//                                           "background-color: rgba(255,255,255,0.5); "
//                                           "border: 1px solid rgba(255,255,255,0.4); "
//                                           "border-radius: 4px; padding: 2px; }"
//                     "QLineEdit:hover { background-color: rgba(255,0,0,0.3); }"
//                     "QLineEdit:focus { background-color: rgba(255, 0, 0, 0.5); color: black; "
//                                           "border: 1px solid rgba(255,255,255,0.8); }";
//                 for (auto& ft : flowTasksList) {
//                     if (ft.nameEdit) ft.nameEdit->setStyleSheet(normalComboStyle);
//                     if (ft.timeEdit) ft.timeEdit->setStyleSheet(normalLineStyle);
//                 }
//                 // снятие фокуса с любого виджета
//                 if (QWidget* focused = QApplication::focusWidget()) {
//                     focused->clearFocus();
//                 }
//                 // Возврат фокуса самому окну, чтобы клики по пустому месту работали
//                 this->setFocus();
//             }
//         }
//     }
//     return QWidget::eventFilter(obj, event);
// }

// void BaseTracker::selectFlowTask(int index)
// {
//     if (index < 0 || index >= flowTasksList.size()) return;
//     if (!flowTasksList[index].nameEdit || !flowTasksList[index].timeEdit) return;
//     // СТИЛИ ДЛЯ КОМБОБОКСА
//     QString normalComboStyle =
//         "QComboBox { color: black; font-weight: bold; background-color: rgba(255,255,255,0.5); "
//                                "border: 1px solid rgba(255,255,255,0.4); border-radius: 4px; "
//                                "padding: 2px; }"
//         "QComboBox:hover { background-color: rgba(255,0,0,0.3); }"
//         "QComboBox:focus { background-color: rgba(255, 0, 0, 0.5); "
//                                "color: black; border: 1px solid rgba(255,255,255,0.8); }"
//         "QComboBox QLineEdit { background: transparent; border: none; color: black; }";
//     QString activeComboStyle =
//         "QComboBox { background-color: rgba(255, 0, 0, 0.5); color: black; border: 1px solid white; }"
//         "QComboBox:focus { background-color: rgba(255, 0, 0, 0.7); color: black;"
//                                " border: 1px solid white; }"
//         "QComboBox QLineEdit { background: transparent; border: none; color: black; }";
//     // СТИЛИ ДЛЯ QLINEEDIT (Время)
//     QString normalLineStyle =
//         "QLineEdit { color: black; font-weight: bold; background-color: rgba(255,255,255,0.5); "
//                               "border: 1px solid rgba(255,255,255,0.4); border-radius: 4px; "
//                               "padding: 2px; }"
//         "QLineEdit:hover { background-color: rgba(255,0,0,0.3); }"
//         "QLineEdit:focus { background-color: rgba(255, 0, 0, 0.5); "
//                               "color: black; border: 1px solid rgba(255,255,255,0.8); }";
//     QString activeLineStyle =
//         "QLineEdit { background-color: rgba(255, 0, 0, 0.5); color: black; border: 1px solid white; }"
//         "QLineEdit:focus { background-color: rgba(255, 0, 0, 0.7); color: black; "
//                               "border: 1px solid white; }";
//     // Снятие выделения со всех строк
//     for (auto& ft : flowTasksList) {
//         if (ft.nameEdit) ft.nameEdit->setStyleSheet(normalComboStyle);
//         if (ft.timeEdit) ft.timeEdit->setStyleSheet(normalLineStyle);
//     }
//     // новый активный индекс
//     activeFlowIndex = index;
//     // Подсветка красным
//     FlowTask& ft = flowTasksList[index];
//     ft.nameEdit->setStyleSheet(activeComboStyle);
//     ft.timeEdit->setStyleSheet(activeLineStyle);
//     // Синхронизация таймера
//     if (digitalClock2 && flowTabs && flowTabs->currentIndex() == 1
//         && !digitalClock2->is_running()) {
//         int duration = parseDurationMin(ft.timeEdit->text());
//         digitalClock2->blockSignals(true);
//         digitalClock2->set_timer_time(0, duration, 0);
//         digitalClock2->blockSignals(false);
//     }
//     updateClockEditable();
// }

// void BaseTracker::on_guest_link_activated(const QString &link)
// { if (link == "#open") {
//         // открыть профиль (логика из EatTheFrog::on_label_4_linkActivated)
//         if (!profileWindow) {
//             profileWindow = new profileW();
//             profileWindow->setOpenWindow(this);
//             profileWindow->setAttribute(Qt::WA_DeleteOnClose);
//             connect(profileWindow, &profileW::profileUpdated,
//                     this, [this]() { loadUserProfile(); loadCurrentUser(); });
//             connect(profileWindow, &QObject::destroyed,
//                     this, [this]() { profileWindow = nullptr; });
//         }
//         profileWindow->show();
//         profileWindow->raise();
//         profileWindow->activateWindow();
//         this->hide();
//         return;
//     }
//     if (link != "#guest") return;
//     if (!loginDialog) {
//         loginDialog = new QDialog(this);
//         loginDialog->setWindowTitle(tr("Окно авторизации"));
//         loginDialog->setStyleSheet(R"(
//             QDialog { background-color: rgba(0, 0, 0, 0.5); }
//             QLabel { color: white; font-size: 14px; font-weight: bold; padding: 8px 0; }
//             QPushButton { background-color: rgba(0, 0, 0, 0.5); color: white; font-weight: bold;
//                           border: 1px solid #555; border-radius: 6px; padding: 8px 16px;
//                           font-size: 14px; min-width: 140px; }
//             QPushButton:hover { background-color: rgba(255, 0, 0, 0.3); border-color: #6a9fb5; }
//             QPushButton:pressed { background-color: rgba(255, 0, 0, 0.8); }
//         )");
//         auto* layout = new QVBoxLayout(loginDialog);
//         layout->addWidget(new QLabel(tr("Выберите действие:"), loginDialog));
//         QPushButton* regBtn  = new QPushButton(tr("Регистрация"), loginDialog);
//         QPushButton* authBtn = new QPushButton(tr("Авторизация"), loginDialog);
//         QPushButton* cancel  = new QPushButton(tr("Отмена"), loginDialog);
//         layout->addWidget(regBtn);
//         layout->addWidget(authBtn);
//         layout->addWidget(cancel);
//         connect(regBtn,  &QPushButton::clicked, this, [this]() { loginDialog->done(1); });
//         connect(authBtn, &QPushButton::clicked, this, [this]() { loginDialog->done(2); });
//         connect(cancel,  &QPushButton::clicked, this, [this]() { loginDialog->done(0); });
//         connect(loginDialog, &QObject::destroyed,
//                 this, [this]() { loginDialog = nullptr; });
//     }
//     int choice = loginDialog->exec();
//     if (choice == 1) {
//         // Регистрация
//         auto* registration = new NEW_USER_FORM();
//         registration->setAttribute(Qt::WA_DeleteOnClose);
//         registration->setOpenWindow(this);
//         registration->show();
//         this->hide();
//     }
//     else if (choice == 2) {
//         // Авторизация
//         auto* autorisation = new USERS_FORM();
//         autorisation->setAttribute(Qt::WA_DeleteOnClose);
//         autorisation->setOpenWindow(this);
//         autorisation->show();
//         this->hide();
//     }
// }
#include "basetracker.h"
#include "qboxlayout.h"
#include "qcombobox.h"
#include "qlineedit.h"
#include "qmessagebox.h"
#include "qpushbutton.h"
#include "qtoolbutton.h"
#include <QCheckBox>
#include <QStackedWidget>
#include <QListWidget>
#include <QScrollArea>
#include <QScrollBar>
#include <QPainter>
#include "data_manager.h"
#include <QSpinBox>
#include "new_user_form.h"
#include "users_form.h"
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QSignalBlocker>

using namespace FTR;


BaseTracker::BaseTracker(QWidget *parent,FTR::TimerMode mode)
    : QWidget(parent),mode(mode)
{
    this->resize(800, 600);
    SPDLOG_INFO("BaseTracker: создание, mode = {}", static_cast<int>(mode));
    switch(mode){
    case FlowMode:
    case TimeBlocking:
    case EatTheFrog:
        analogClock = new MyClock(this);
        analogClock->setStyleSheet("background: transparent;");
        analogClock->setAttribute(Qt::WA_TranslucentBackground);

        digitalClock = new digital_clock(this);
        digitalClock->setColor(Qt::white);
        digitalClock->setShowSeconds(true);
        digitalClock->set24HourFormat(true);
        digitalClock->setStyleSheet("background: transparent;");

        digitalClock2 = new digital_clock(this);
        digitalClock2->setColor(Qt::white);
        digitalClock2->setShowSeconds(true);
        digitalClock2->set24HourFormat(true);
        digitalClock2->setStyleSheet("background: transparent;");
        digitalClock2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        analogClock->setFixedSize(250,250);
        digitalClock->setFixedSize(250, 80);
        digitalClock2->setFixedSize(250, 80);
        analogClock->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        digitalClock->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        SPDLOG_TRACE("BaseTracker: созданы часы (Flow/TimeBlocking/EatTheFrog)");
        break;
    case Pomodoro:
    case ReversePomodoro:
    case Mode60_10:
    case Mode52_17:
    case Mode90_20:
        digitalClock = new digital_clock(this);
        digitalClock->setColor(Qt::white);
        digitalClock->setShowSeconds(true);
        digitalClock->set24HourFormat(true);
        digitalClock->setStyleSheet("background: transparent;");
        digitalClock->setFixedSize(250, 80);
        digitalClock->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        SPDLOG_TRACE("BaseTracker: создан digitalClock (Pomodoro-режимы)");
        break;}
    //СОЗДАНИЕ НЕВИДИМОГО ТАЙМЕРА
    blockTimer = new QTimer(this);
    blockTimer->setSingleShot(true);
    connect(blockTimer, &QTimer::timeout, this, &BaseTracker::onBlockTimerFinished);
    SPDLOG_TRACE("Драйверы: {}",QSqlDatabase::drivers().join(", ").toStdString());
    SPDLOG_TRACE("Создание базового виртуального класса");
}

//скрыть комбобокс с тасками в зависимости от режима гостя или юзера
void BaseTracker::hide_combo(){
    qint64 uid = Data_Manager::instance().get_currentUser();
    if (uid>=0){
        SPDLOG_INFO("hide_combo: Режим пользователя. Не скрываем");
        return;
    }
    QComboBox* c = activityComboBox();
    c->hide();
    QLabel* l = get_activity_label();
    l->hide();
    SPDLOG_INFO("hide_combo: Режим гостя. Комбо-бокс скрыт");
}

BaseTracker::~BaseTracker()
{
    SPDLOG_DEBUG("~BaseTracker: начало деструктора");
    if (taskTimer && taskTimer->isActive())
        taskTimer->stop();
    if (todolist) {
        delete todolist;
        todolist = nullptr;
    }
    SPDLOG_DEBUG("Удален базовый класс");
}

//подключение ссылки на профиль
void BaseTracker::connectInfoLink()
{
    if (profileLinkConnected) {
        SPDLOG_TRACE("connectInfoLink: ссылки уже подключены, повторное подключение не требуется");
        return;}
    QLabel* link = get_info_link();
    if (link) {
        SPDLOG_DEBUG("connectInfoLink: ссылка 'Информация' подключена (objectName = {})",
                     link->objectName().toStdString());
        connect(link, &QLabel::linkActivated,
                this, &BaseTracker::on_label_7_linkActivated);
    }
    else{
        SPDLOG_WARN("connectInfoLink: get_info_link() вернул nullptr - ссылка не будет работать");}
    // обработка ссылки Гость
    QLabel* userLbl = userLabel();
    if (userLbl) {
        SPDLOG_DEBUG("connectInfoLink: ссылка 'Пользователь/Гость' подключена "
                     "(objectName = '{}')",
                     userLbl->objectName().toStdString());
        connect(userLbl, &QLabel::linkActivated,
                this, &BaseTracker::on_guest_link_activated);
    }
    profileLinkConnected = true;
    SPDLOG_TRACE("connectInfoLink: все ссылки подключены, флаг profileLinkConnected = true");
}

//старт режима таймера
void BaseTracker::startPomodoro(digital_clock* clock, int count)
{
    if (!clock) {
        SPDLOG_ERROR("startPomodoro: передан nullptr вместо часов — запуск невозможен");
        return;
    }
    if (count <= 0) {
        SPDLOG_ERROR("startPomodoro: некорректное количество помидоров (count = {}) — запуск невозможен",
                     count);
        return;
    }
    this->clock = clock;
    left  = count * 2 - 1;
    work  = true;
    currentPhase = Phase::Work;
    SPDLOG_INFO("startPomodoro: запуск, count = {}, left (фаз) = {}, "
                "workMinutes = {}, breakMinutes = {}",
                count, left, workMinutes, breakMinutes);
    if (sessionStart.isValid()) {
        SPDLOG_WARN("startPomodoro: обнаружена незакрытая сессия "
                    "(start = {}), закрываем её",
                    sessionStart.toString(Qt::ISODate).toStdString());
    }
    finishCurrentSession(false);
    connect(clock, &digital_clock::timerFinished,
            this, &BaseTracker::nextPhase,
            Qt::UniqueConnection);
    SPDLOG_TRACE("startPomodoro: сигнал timerFinished подключён к nextPhase()");
    nextPhase();
    SPDLOG_TRACE("startPomodoro: первая фаза запущена, left = {}", left);
}

//открытие окошка с информацией
void BaseTracker::on_label_7_linkActivated(const QString &link){
    if (link != "#open"){
        SPDLOG_TRACE("on_label_7_linkActivated: получена ссылка {},не #open - выход",
                     link.toStdString());
        return;
    }
    SPDLOG_TRACE("on_label_7_linkActivated: клик по ссылке 'Информация'");
    if (!info){
        SPDLOG_DEBUG("on_label_7_linkActivated: создаём новое окно Information");
        info = new Information(nullptr,get_info_text());
        info->setAttribute(Qt::WA_DeleteOnClose);
        info->set_parent_window(this);
        connect(info, &QObject::destroyed,
                this, [this]() {
                    SPDLOG_TRACE("on_label_7_linkActivated: окно Information удалено, указатель сброшен");
                    info = nullptr; });
    }else {
        SPDLOG_DEBUG("on_label_7_linkActivated: окно Information уже существует, "
                     "повторный показ");
    }
    info->show();
    SPDLOG_DEBUG("on_label_7_linkActivated: окно Information показано, "
                 "трекер '{}' скрыт",
                 windowTitleText().toStdString());
    this->hide();
}

//смена фазы
void BaseTracker::nextPhase()
{
    if (!clock) {
        SPDLOG_ERROR("nextPhase: clock == nullptr — смена фазы невозможна");
        return;
    }
    SPDLOG_DEBUG("nextPhase: закрываем предыдущую сессию (left = {}, work = {})",
                 left, work);
    finishCurrentSession(true);
    if (left <= 0) {
        SPDLOG_INFO("nextPhase: все фазы завершены (left = 0) — задача выполнена");
        if (activeTaskLabel)
            activeTaskLabel->setText(tr("Задача выполнена"));
        // ── ЗАЩИТА: проверяем, что чекбокс ещё существует ──
        if (activeDoneCheck) {
            SPDLOG_DEBUG("nextPhase: чекбокс текущей задачи отмечен как выполненный");
            activeDoneCheck->setChecked(true);
        } else {
            SPDLOG_WARN("nextPhase: activeDoneCheck == nullptr — "
                        "чекбокс не отмечен (возможно, строка удалена)");
        }
        playNotificationSound();
        SPDLOG_TRACE("nextPhase: цикл завершён, звуковая нотификация воспроизведена");
        return;
    }
    left--;
    sessionStart = QDateTime::currentDateTime();
    if (work) {
        currentPhase = Phase::Work;
        if (activeTaskLabel)
            activeTaskLabel->setText(tr("Работа %1 мин").arg(workMinutes));
        clock->setMode(FTR::timer);
        SPDLOG_INFO("nextPhase: запуск РАБОТЫ, {} мин (осталось фаз: {})",
                    workMinutes, left);
        clock->timer_start(0, workMinutes, 0);
    } else {
        currentPhase = Phase::Break;
        if (activeTaskLabel)
            activeTaskLabel->setText(tr("Отдых %1 мин").arg(breakMinutes));
        clock->setMode(FTR::timer);
        SPDLOG_INFO("nextPhase: запуск ОТДЫХА, {} мин (осталось фаз: {})",
                    breakMinutes, left);
        clock->timer_start(0, breakMinutes, 0);
    }
    playNotificationSound();
    work = !work;
    SPDLOG_TRACE("nextPhase: фаза запущена, следующий work = {}, left = {}",
                 work, left);
}

void BaseTracker::createTaskRow(QLineEdit*&   outName,
                                QLineEdit*&   outCount,
                                QCheckBox*&   outDone,
                                QPushButton*& outStart)
{
    if (!rowsLayout || !rowsContainer) {
        SPDLOG_ERROR("createTaskRow: rowsLayout или rowsContainer == nullptr — "
                     "строку создать невозможно (rowsLayout = {}, rowsContainer = {})",
                     static_cast<const void*>(rowsLayout),
                     static_cast<const void*>(rowsContainer));
        return;
    }
    SPDLOG_TRACE("createTaskRow: создание новой строки задачи");
    QHBoxLayout* row = new QHBoxLayout();
    outName  = new QLineEdit(rowsContainer);
    outCount = new QLineEdit(rowsContainer);
    outDone  = new QCheckBox(rowsContainer);
    outStart = new QPushButton("▶", rowsContainer);
    //кнопка удаления
    QPushButton* removeBtn = new QPushButton("✕", rowsContainer);
    removeBtn->setFixedWidth(30);
    removeBtn->setStyleSheet(
        "QPushButton { color: white; background: rgba(0,0,0,0.5); "
        "border: 1px solid rgba(255,255,255,0.4); border-radius: 4px; padding: 2px; }"
        "QPushButton:hover { background: rgba(255,0,0,0.5); }");
    outDone->setEnabled(false);
    outDone->setStyleSheet(
        "QCheckBox { color: white; }"
        "QCheckBox::indicator { width: 16px; height: 16px; }");
    outName->setPlaceholderText(tr("Название задачи"));
    outCount->setPlaceholderText(tr("Кол-во"));
    outCount->setValidator(new QIntValidator(1, 99, outCount));
    outStart->setFixedWidth(40);
    row->addWidget(outName);
    row->addWidget(outCount);
    row->addWidget(outDone);
    row->addWidget(outStart);
    row->addWidget(removeBtn);
    rowsLayout->insertLayout(rowsLayout->count() - 1, row);
    SPDLOG_TRACE("createTaskRow: строка добавлена в rowsLayout, "
                 "всего элементов в layout = {}",
                 rowsLayout->count());
    // Обработчик удаления
    connect(removeBtn, &QPushButton::clicked, this,
            [this, row, outName, outCount, outDone, outStart, removeBtn]() {
                SPDLOG_INFO("createTaskRow: удаление строки задачи");
                //если активная задача удалена-сброс указателя
                if (activeDoneCheck == outDone) {
                    SPDLOG_DEBUG("createTaskRow: удаляемая строка — активная "
                                 "(pomoClock running = {})",
                                 pomoClock ? pomoClock->is_running() : false);
                    activeDoneCheck = nullptr;
                    // Остановка таймера помидора, если он запущен
                    if (pomoClock) {
                        pomoClock->timer_stop();
                        pomoClock->set_timer_time(0, 0, 0);
                        SPDLOG_TRACE("createTaskRow: pomoClock остановлен и сброшен");
                    }
                    if (sessionStart.isValid()) {
                        SPDLOG_DEBUG("createTaskRow: закрытие активной сессии "
                                     "из-за удаления активной задачи");
                    }
                    // Закрытие сессии
                    finishCurrentSession(false);
                    // Возвращение на страницу задач (если сейчас на странице помидора)
                    if (pomoStack && tasksPage) {
                        pomoStack->setCurrentWidget(tasksPage);
                        SPDLOG_TRACE("createTaskRow: переключение на tasksPage");
                    }
                    // Показ часов
                    if (digitalClock) digitalClock->show();
                    if (analogClock)  analogClock->show();
                }
                if (rowsLayout) {
                    SPDLOG_TRACE("createTaskRow: строка удалена из rowsLayout, "
                                 "осталось элементов = {}",
                                 rowsLayout->count());
                    rowsLayout->removeItem(row);
                }
                delete outName;
                delete outCount;
                delete outDone;
                delete outStart;
                delete removeBtn;
                delete row;
                SPDLOG_TRACE("createTaskRow: все виджеты строки удалены");
            });
    if (get_info_link()) {
        connect(get_info_link(), &QLabel::linkActivated,
                this, &BaseTracker::on_label_7_linkActivated);
        SPDLOG_TRACE("createTaskRow: ссылка 'Информация' подключена");
    } else {
        SPDLOG_WARN("createTaskRow: get_info_link() == nullptr — "
                    "ссылка 'Информация' не будет работать в этой строке");
    }
}

QLineEdit* BaseTracker::addTaskRow()
{
    SPDLOG_TRACE("addTaskRow: добавление строки помодоро-задачи");
    QLineEdit*   taskLine      = nullptr;
    QLineEdit*   pomodoroCount = nullptr;
    QCheckBox*   doneCheck     = nullptr;
    QPushButton* startBtn      = nullptr;
    createTaskRow(taskLine, pomodoroCount, doneCheck, startBtn);
    if (!taskLine) {
        SPDLOG_WARN("addTaskRow: createTaskRow вернул nullptr");
        return nullptr;
    }
    connect(startBtn, &QPushButton::clicked, this,
            [this, taskLine, pomodoroCount, doneCheck]()
            {
                //Название задачи не пустое
                QString name = taskLine->text().trimmed();
                if (name.isEmpty()) {
                    SPDLOG_WARN("addTaskRow(startBtn): пустое название задачи");
                    QMessageBox::information(this, "Внимание",
                                             "Введите название задачи перед запуском.");
                    taskLine->setFocus();
                    return;
                }
                //Количество помидоров > 0
                int count = pomodoroCount->text().toInt();
                if (count <= 0) {
                    SPDLOG_WARN("addTaskRow(startBtn): некорректное количество помидоров = {}", count);
                    QMessageBox::information(this, "Внимание",
                                             "Введите количество помидоров (от 1 до 99).");
                    pomodoroCount->setFocus();
                    return;
                }
                QComboBox* combo = activityComboBox();
                QString activityName = combo ? combo->currentText().trimmed() : QString();
                currentTaskId = combo ? combo->currentData().toLongLong() : -1;
                SPDLOG_INFO("addTaskRow(startBtn): старт помодоро, activity = '{}', "
                            "subtask = '{}', taskId = {}, count = {}",
                            activityName.toStdString(), name.toStdString(),
                            currentTaskId, count);
                if (p_activeActivityLabel)
                    p_activeActivityLabel->setText(tr("Активность: %1").arg(
                        activityName.isEmpty() ? tr("—") : activityName));
                // Подзадача (название строки)
                if (p_activeSubtaskLabel)
                    p_activeSubtaskLabel->setText(tr("Подзадача: %1").arg(name));
                qDebug() << "Pomodoro start: activity =" << activityName
                         << "subtask =" << name<< "task_id =" << currentTaskId;
                // Закрытие старой сессии
                finishCurrentSession(false);
                this->activeDoneCheck = doneCheck;
                if (activeTaskLabel)
                    activeTaskLabel->setText(tr("Задача: %1").arg(name));
                if (pomoStack && pomodoroPage)
                    pomoStack->setCurrentWidget(pomodoroPage);
                if (digitalClock) digitalClock->hide();
                if (analogClock)  analogClock->hide();
                work = true;
                currentPhase = Phase::Work;
                startPomodoro(pomoClock, count);
            });
    return taskLine;
}

void BaseTracker::createFlowTaskRow(QVBoxLayout* rowsLayout,
                                    QWidget* container,
                                    const QString& text,
                                    PriorityWidgetBuilder priorityFactory,
                                    bool isStopwatch)
{
    if (!rowsLayout || !container) {
        SPDLOG_WARN("createFlowTaskRow: rowsLayout или container == nullptr — выход");
        return;
    }
    SPDLOG_TRACE("createFlowTaskRow: text = '{}', isStopwatch = {}",
                 text.toStdString(), isStopwatch);
    QHBoxLayout* row = new QHBoxLayout();
    QComboBox* taskCombo = new QComboBox(container);
    taskCombo->setEditable(true);
    taskCombo->setInsertPolicy(QComboBox::NoInsert);
    taskCombo->setStyleSheet(
        // Основной вид (когда не в фокусе)
        "QComboBox { "
        "   color: black; font-weight: bold; "
        "   background-color: rgba(255,255,255,0.5); "
        "   border: 1px solid rgba(255,255,255,0.4); "
        "   border-radius: 4px; padding: 2px; "
        "}"

        // При наведении
        "QComboBox:hover { background-color: rgba(255,0,0,0.3); }"

        // В фокусе (подсветка красным)
        "QComboBox:focus { "
        "   background-color: rgba(255, 0, 0, 0.5); color: black; "
        "   border: 1px solid rgba(255,255,255,0.8); "
        "}"

        // Убираем пунктирную рамку, которая появляется при фокусе
        "QComboBox:focus { outline: none; }"

        // ВНУТРЕННИЙ QLineEdit (когда пользователь вводит текст)
        "QComboBox QLineEdit { "
        "   background: transparent; border: none; color: black; "
        "   selection-background-color: rgba(255,0,0,0.5); "
        "}"

        // Выпадающий список
        "QComboBox QAbstractItemView { "
        "   background-color: rgba(30, 30, 30, 0.95); color: white; "
        "   selection-background-color: rgba(255,0,0,0.5); "
        "   border: 1px solid rgba(255,255,255,0.4); "
        "   outline: none; " // Убираем рамку вокруг выпадающего списка
        "}"
        );
    taskCombo->setToolTip("Введите название задачи\nили выберите его");
    // Заполнение задачами
    refreshTaskCombo(taskCombo);
    // Если передали текст при создании, ставим его
    if (!text.isEmpty()) {
        int idx = taskCombo->findText(text, Qt::MatchFixedString);
        if (idx >= 0) taskCombo->setCurrentIndex(idx);
        else taskCombo->setCurrentText(text);
    }
    //  РЕГИСТРАЦИЯ ЗАДАЧИ В БД СРАЗУ ПРИ ВВОДЕ
    if (taskCombo->lineEdit()) {
        connect(taskCombo->lineEdit(), &QLineEdit::returnPressed,
                this, [this, taskCombo]() {
                    // Закрываем popup, иначе Enter обработает сам QComboBox
                    taskCombo->hidePopup();
                    SPDLOG_TRACE("createFlowTaskRow(returnPressed): регистрация '{}'",
                                 taskCombo->currentText().toStdString());
                    ensureTaskRegistered(taskCombo);
                });
    }
    // Регистрация при выборе из выпадающего списка
    connect(taskCombo, QOverload<int>::of(&QComboBox::activated),
            this, [this, taskCombo](int) {
                SPDLOG_TRACE("createFlowTaskRow(activated): регистрация '{}'",
                             taskCombo->currentText().toStdString());
                ensureTaskRegistered(taskCombo);
            });
    //Синхронизация: когда где-то создана новая задача - обновление списка
    connect(this, &BaseTracker::tasksChanged, taskCombo, [this, taskCombo]{
        SPDLOG_TRACE("createFlowTaskRow(tasksChanged): refresh combo");
        refreshTaskCombo(taskCombo);
    });
    // ПОЛЕ ВРЕМЕНИ
    QLineEdit* timeLine = new QLineEdit(container);
    timeLine->setPlaceholderText(tr("Мин"));
    timeLine->setStyleSheet(
        "QLineEdit { color: black; font-weight: bold; background-color: rgba(255,255,255,0.5); border: 1px solid rgba(255,255,255,0.4); border-radius: 4px; padding: 2px; }"
        "QLineEdit:hover { background-color: rgba(255,0,0,0.3); }"
        "QLineEdit:focus { background-color: rgba(255, 0, 0, 0.5); color: black; border: 1px solid rgba(255,255,255,0.8); }"
        );
    if (isStopwatch) timeLine->hide();
    QCheckBox* done = new QCheckBox(container);
    done->setStyleSheet("QCheckBox { color: black; } QCheckBox::indicator { width: 16px; height: 16px; subcontrol-position: center center; }");
    done->setFixedWidth(20);
    if (isStopwatch) done->hide();
    QPushButton* startBtn = new QPushButton("▶", container);
    startBtn->setFixedWidth(30);
    startBtn->setStyleSheet(
        "QPushButton { color: black; font-weight: bold; background-color: rgba(255,255,255,0.5); border: 1px solid rgba(255,255,255,0.4); border-radius: 4px; padding: 2px; }"
        "QPushButton:hover { background-color: rgba(255,0,0,0.5); }");
    QPushButton* removeBtn = new QPushButton("✕", container);
    removeBtn->setFixedWidth(30);
    removeBtn->setStyleSheet(
        "QPushButton { color: black; font-weight: bold; background-color: rgba(255,255,255,0.5); border: 1px solid rgba(255,255,255,0.4); border-radius: 4px; padding: 2px; }"
        "QPushButton:hover { background-color: rgba(255,0,0,0.5); }");
    QComboBox* priorityCombo = nullptr;
    if (priorityFactory) {
        priorityCombo = qobject_cast<QComboBox*>(priorityFactory(container));
        if (priorityCombo) {
            row->insertWidget(0, priorityCombo);
            connect(priorityCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this, rowsLayout]() {
                SPDLOG_TRACE("createFlowTaskRow(priorityCombo): сортировка строк");
                sortFlowRows(rowsLayout);
            });
        }
    }
    row->addWidget(taskCombo, 1);
    if (!isStopwatch) {
        row->addWidget(timeLine, 1);
    }
    row->addWidget(done);
    row->addWidget(startBtn);
    row->addWidget(removeBtn);
    rowsLayout->insertLayout(rowsLayout->count() - 1, row);
    // СОХРАНЕНИЕ В СПИСОК
    int myIndex = flowTasksList.size();
    FlowTask ft;
    ft.nameEdit = taskCombo; // Теперь это указатель на QComboBox (проверь, что в .h структура FlowTask изменилась)
    ft.timeEdit = timeLine;
    ft.doneCheck = done;
    ft.priorityCombo = priorityCombo;
    ft.startBtn = startBtn;
    flowTasksList.append(ft);
    SPDLOG_TRACE("createFlowTaskRow: строка добавлена, myIndex = {}, "
                 "всего задач = {}",
                 myIndex, flowTasksList.size());
    if (flowTasksList.size() == 1) selectFlowTask(0);
    // СОБЫТИЯ ДЛЯ ВЫДЕЛЕНИЯ
    if (mode == FTR::FlowMode) {
        taskCombo->setProperty("taskIndex", myIndex);
        timeLine->setProperty("taskIndex", myIndex);
        taskCombo->installEventFilter(this);
        timeLine->installEventFilter(this);
    }
    // СИНХРОНИЗАЦИЯ ДАННЫХ
    connect(timeLine, &QLineEdit::textChanged, this, [this, taskCombo, timeLine]() {
        int idx = -1;
        for (int i = 0; i < flowTasksList.size(); ++i) {
            if (flowTasksList[i].nameEdit == taskCombo) { idx = i; break; }
        }
        if (idx < 0 || idx != activeFlowIndex) return;
        int duration = parseDurationMin(timeLine->text());
        SPDLOG_TRACE("createFlowTaskRow(textChanged): idx = {}, duration = {}",
                     idx, duration);
        if (digitalClock2 && flowTabs && flowTabs->currentIndex() == 1) {
            digitalClock2->set_timer_time(0, duration, 0);
        }
    });
    //КНОПКА СТАРТ
    connect(startBtn, &QPushButton::clicked, this, [this, taskCombo] {
        int idx = -1;
        for (int i = 0; i < flowTasksList.size(); ++i) {
            if (flowTasksList[i].nameEdit == taskCombo) { idx = i; break; }
        }
        if (idx < 0) {
            SPDLOG_WARN("createFlowTaskRow(startBtn): idx < 0 — задача не найдена");
            return;
        }
        // Гарантия что задача зарегистрирована в БД
        const qint64 id = ensureTaskRegistered(taskCombo);
        if (id <= 0) {
            SPDLOG_WARN("createFlowTaskRow(startBtn): ensureTaskRegistered вернул {} для '{}'",
                        id, taskCombo->currentText().toStdString());
            QMessageBox::information(this, tr("Внимание"),
                                     tr("Введите название задачи перед запуском."));
            taskCombo->setFocus();
            return;
        }
        SPDLOG_INFO("createFlowTaskRow(startBtn): запуск задачи id = {}, mode = {}",
                    id, static_cast<int>(mode));
        selectFlowTask(idx);
        // Если EatTheFrog - запуск последовательности
        if (mode == FTR::EatTheFrog) {
            startTaskSequence();
        } else {
            // Для  FlowMode запуск таймера напрямую
            FTR::ClockMode clockMode = FTR::stopwatch;
            if (flowTabs && flowTabs->currentIndex() == 1) clockMode = FTR::timer;
            applyClockMode(clockMode);
            // Проверка текста
            QString name = taskCombo->currentText().trimmed();
            if (name.isEmpty()) {
                SPDLOG_WARN("createFlowTaskRow(startBtn): пустое имя задачи");
                QMessageBox::information(this, "Ошибка", "Введите название задачи");
                return;
            }
            qint64 taskId = taskCombo->currentData().toLongLong();
            finishCurrentSession(false);
            currentTaskId = taskId;
            sessionStart = QDateTime::currentDateTime();
            digital_clock* c = digitalClock2;
            if (c) {
                c->timer_stop();
                c->set_timer_time(0, 0, 0);
                if (clockMode == FTR::stopwatch) {
                    SPDLOG_DEBUG("createFlowTaskRow(startBtn): старт секундомера");
                    c->stopwatch_start();
                } else {
                    int duration = parseDurationMin(flowTasksList[idx].timeEdit->text());
                    if (duration == 0) duration = workMinutes;
                    SPDLOG_DEBUG("createFlowTaskRow(startBtn): старт таймера на {} мин",
                                 duration);
                    c->timer_start(0, duration, 0);
                }
            }
        }
    });
    connect(removeBtn, &QPushButton::clicked, this,
            [this, row, taskCombo, timeLine, done, startBtn, removeBtn,priorityCombo]() {
                // индекс по указателю nameEdit
                int idx = -1;
                for (int i = 0; i < flowTasksList.size(); ++i) {
                    if (flowTasksList[i].nameEdit == taskCombo) { idx = i; break; }
                }
                // была ли удаляемая строка активной
                const bool removingActive = (idx >= 0 && idx == activeFlowIndex);
                SPDLOG_DEBUG("createFlowTaskRow(removeBtn): idx = {}, activeFlowIndex = {}, "
                             "removingActive = {}",
                             idx, activeFlowIndex, removingActive);
                // Удаление из модели и корректировка activeFlowIndex
                if (idx >= 0) {
                    flowTasksList.removeAt(idx);
                    if (activeFlowIndex == idx) {
                        activeFlowIndex = -1;
                    } else if (activeFlowIndex > idx) {
                        activeFlowIndex--;
                    }
                }
                //ОСТАНОВКА ТАЙМЕРА/СЕКУНДОМЕРА
                //список стал пустым — отсчёт не к чему привязать,
                //удалили активную строку — таймер тикал именно на неё.
                //Если удалили неактивную и остались другие — таймер не трогаем.
                if (flowTasksList.isEmpty() || removingActive) {
                    SPDLOG_DEBUG("createFlowTaskRow(removeBtn): остановка таймера "
                                 "(пусто = {}, removingActive = {})",
                                 flowTasksList.isEmpty(), removingActive);
                    stopFlowClockAndSession();
                }
                // АВТО-СОСТОЯНИЕ ПОСЛЕ УДАЛЕНИЯ
                if (flowTasksList.size() == 1) {
                    if (flowTasksList[0].nameEdit && flowTasksList[0].timeEdit) {
                        selectFlowTask(0);
                    }
                } else if (flowTasksList.isEmpty()) {
                    activeFlowIndex = -1;
                    updateClockEditable();
                    if (sequenceStatusLabel) {
                        sequenceStatusLabel->setText(tr("Список не запущен"));
                    }
                }
                else if (removingActive) {
                    if (sequenceStatusLabel) {
                        sequenceStatusLabel->setText(tr("Список не запущен"));
                    }
                }
                // Пересчёт taskIndex
                for (int i = 0; i < flowTasksList.size(); ++i) {
                    if (flowTasksList[i].nameEdit)
                        flowTasksList[i].nameEdit->setProperty("taskIndex", i);
                    if (flowTasksList[i].timeEdit)
                        flowTasksList[i].timeEdit->setProperty("taskIndex", i);
                }// удаление layout из rowsLayout
                if (this->rowsLayout && row) {
                    this->rowsLayout->removeItem(row);
                }
                // Удаление виджетов
                delete taskCombo;
                delete timeLine;
                delete done;
                delete startBtn;
                delete removeBtn;
                if (priorityCombo) delete priorityCombo;
                if (row) delete row;
                SPDLOG_INFO("createFlowTaskRow(removeBtn): строка удалена, осталось {}",
                            flowTasksList.size());
            });
}
QWidget* BaseTracker::buildFlowTabPage(FTR::ClockMode clockMode,
                                       PriorityWidgetBuilder priorityFactory)
{
    SPDLOG_TRACE("buildFlowTabPage: начало, clockMode = {}",
                 static_cast<int>(clockMode));
    QWidget* page = new QWidget();
    page->setStyleSheet("QWidget { background: rgba(0,0,0,0.6); border-radius: 10px; }");
    QVBoxLayout* pageLayout = new QVBoxLayout(page);
    QLabel* header = new QLabel(
        clockMode == FTR::stopwatch ? tr("Список задач — секундомер") : tr("Список задач — таймер"));
    header->setAlignment(Qt::AlignHCenter);
    header->setStyleSheet("QLabel { color: white; font-size: 18px; "
                          "font-weight: bold;background: transparent; }");
    pageLayout->addWidget(header);
    QWidget* container = new QWidget(page);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setMinimumWidth(400);
    container->setMaximumWidth(800);
    container->setStyleSheet("QWidget { background: transparent; }");
    QVBoxLayout* listLayout = new QVBoxLayout(container);
    listLayout->setContentsMargins(0, 0, 0, 0);
    listLayout->setSpacing(4);
    QHBoxLayout* columns = new QHBoxLayout();
    listLayout->addLayout(columns);
    columns->addStretch();
    if (mode == FTR::EatTheFrog){
        QLabel* task_priority = new QLabel("Приоритет");
        columns->addWidget(task_priority);
        columns->addStretch();
    }
    QLabel* task_name = new QLabel("Название задачи");
    columns->addWidget(task_name );
    columns->addStretch();
    if (clockMode == timer){
        QLabel* task_time = new QLabel("Время");
        columns->addWidget(task_time);
        columns->addStretch();
        QLabel* done_time = new QLabel("Сделано");
        columns->addWidget(done_time);
    }
    QLabel* start_task = new QLabel("Старт");
    columns->addWidget(start_task);
    QLabel* delete_task = new QLabel("Удалить");
    columns->addWidget(delete_task);
    listLayout->addStretch();
    this->rowsLayout = listLayout;
    this->rowsContainer = container;
    QScrollArea* area = new QScrollArea(page);
    area->setWidgetResizable(true);
    area->setWidget(container);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    area->setFrameShape(QFrame::NoFrame);
    area->setStyleSheet("QScrollArea { background: transparent; border: none; }");
    pageLayout->addWidget(area, 1);
    if (priorityFactory) {
        sequenceStatusLabel = new QLabel(tr("Список не запущен"), page);
        sequenceStatusLabel->setStyleSheet(
            "QLabel { color: white; font-weight: bold; background: transparent; }");
        pageLayout->addWidget(sequenceStatusLabel);
    }
    //Строка ввода (Комбобокс)
    QHBoxLayout* inputRow = new QHBoxLayout();
    QComboBox* input = new QComboBox(page);
    input->setEditable(true); // Разрешаем вводить текст
    input->setInsertPolicy(QComboBox::NoInsert); // Запрещаем Qt автоматически добавлять в конец списка (мы сделаем это сами)
    input->setPlaceholderText(tr("Введите или выберите задание"));
    // Заполнение задачами
    refreshTaskCombo(input);
    // Синхронизация input с другими комбобоксами
    connect(this, &BaseTracker::tasksChanged, input, [this, input]{
        SPDLOG_TRACE("buildFlowTabPage(tasksChanged): refresh input");
        refreshTaskCombo(input);
    });
    input->setStyleSheet(
        "QComboBox {"
        "  color: black; font-weight: bold;"
        "  background-color: rgba(255,255,255,0.5);"
        "  border: 1px solid rgba(255,255,255,0.4);"
        "  border-radius: 4px;"
        "  padding: 2px;"
        "}"
        "QComboBox:hover {"
        "  background-color: rgba(255,0,0,0.5);"
        "}"
        "QComboBox QLineEdit {"
        "  background: transparent; border: none; color: black;"
        "}"
        "QComboBox QAbstractItemView {"
        "  background-color: rgba(30, 30, 30, 0.9); color: white;"
        "  selection-background-color: rgba(255,0,0,0.5);"
        "}"
        );
    QPushButton* addBtn = new QPushButton(tr("Добавить"), page);
    addBtn->setStyleSheet(
        "QPushButton {"
        "  color: black;font-weight: bold;"
        "  background-color: rgba(255,255,255,0.5);"
        "  border: 1px solid rgba(255,255,255,0.4);"
        "  border-radius: 4px;"
        "  padding: 2px;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(255,0,0,0.5);"
        "}"
        );
    inputRow->addWidget(input, 1);
    inputRow->addWidget(addBtn);
    pageLayout->addLayout(inputRow);
    container->installEventFilter(this);
    bool isStopwatch = (clockMode == FTR::stopwatch);
    // Коннекты
    connect(addBtn, &QPushButton::clicked, this, [this, input, listLayout, container,
                                                  priorityFactory, isStopwatch]() {
        QString text = input->currentText().trimmed();
        if (text.isEmpty()) {
            SPDLOG_TRACE("buildFlowTabPage(addBtn): пустой ввод — игнор");
            return;
        }
        SPDLOG_TRACE("buildFlowTabPage(addBtn): попытка добавить '{}'",
                     text.toStdString());
        // ПРОВЕРКА НА ДУБЛИКАТ
        for (const auto& ft : flowTasksList) {
            if (ft.nameEdit && ft.nameEdit->currentText().trimmed() == text) {
                SPDLOG_DEBUG("buildFlowTabPage(addBtn): '{}' уже есть в списке",
                             text.toStdString());
                QMessageBox::information(this, "Внимание",
                                         QString("Задача \"%1\" уже есть в списке.").arg(text));
                input->setFocus();
                if (input->lineEdit()) input->lineEdit()->selectAll();
                return;
            }
        }
        // РЕГИСТРАЦИЯ ЗАДАЧИ В БД
        input->setCurrentText(text);
        ensureTaskRegistered(input);
        createFlowTaskRow(listLayout, container, text, priorityFactory, isStopwatch);
        SPDLOG_INFO("buildFlowTabPage(addBtn): задача '{}' добавлена в список",
                    text.toStdString());
        // Очистка ввода
        input->setCurrentIndex(-1);
        input->clearEditText();
        input->setFocus();
    });
    if (input->lineEdit()) {
        connect(input->lineEdit(), &QLineEdit::returnPressed, addBtn, &QPushButton::click);
    }
    if (digitalClock2) digitalClock2->setEditable(false);
    SPDLOG_TRACE("buildFlowTabPage: страница создана");
    return page;
}

QWidget* BaseTracker::buildPriorityCombo(QWidget* parent)
{
    SPDLOG_TRACE("buildPriorityCombo: создание комбо приоритета");
    QComboBox* diffCombo = new QComboBox(parent);
    diffCombo->addItem("🐸 Лягушка", 5);
    diffCombo->addItem("Сложно", 4);
    diffCombo->addItem("Обычно", 3);
    diffCombo->addItem("Просто", 2);
    diffCombo->addItem("Элементарно", 1);
    diffCombo->setStyleSheet(
        "QComboBox { color: black; font-weight: bold; "
        "background-color: rgba(255,255,255,0.5); "
        "border: 1px solid rgba(255,255,255,0.4); "
        "border-radius: 4px; padding: 2px; }"
        "QComboBox:hover { background-color: rgba(255,0,0,0.3); }"
        "QComboBox:focus { background-color: rgba(255, 0, 0, 0.5); color: black; "
        "border: 1px solid rgba(255,255,255,0.8); }");
    return diffCombo;
}
void BaseTracker::addFlowModeInterface(QVBoxLayout* layout,
                                       PriorityWidgetBuilder priorityBuilder )
{
    if (!layout) {
        SPDLOG_WARN("addFlowModeInterface: layout == nullptr — выход");
        return;
    }
    SPDLOG_TRACE("addFlowModeInterface: mode = {}", static_cast<int>(mode));
    QFrame* frame = new QFrame(this);
    frame->setObjectName("blockFrame");
    frame->setStyleSheet(
        "QFrame#blockFrame { background: rgba(1,1,1,0.6); border-radius: 10px; }"
        "QLabel { color: white; }");
    frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    frame->setMinimumWidth(420);
    layout->addWidget(frame, 1);
    QVBoxLayout* vbox = new QVBoxLayout(frame);
    vbox->setContentsMargins(8, 8, 8, 8);
    vbox->setSpacing(4);
    QHBoxLayout* btnrow = new QHBoxLayout();
    vbox->addLayout(btnrow);
    placeClocksInLayout(nullptr,digitalClock2,vbox,btnrow);
    setupButtons(btnrow);
    flowTabs = new QTabWidget(this);
    flowTabs->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    flowTabs->setMinimumWidth(400);
    flowTabs->setMaximumWidth(800);
    flowTabs->setStyleSheet(
        "QTabWidget::pane { border: none; background: transparent; }"
        "QTabBar::tab {"
        "   background: rgba(0,0,0,0.6); color: white;"
        "   padding: 8px 16px; border-top-left-radius: 8px;"
        "   border-top-right-radius: 8px; margin-right: 2px;"
        "}"
        "QTabBar::tab:selected { background: rgba(255,0,0,0.6); }");
    if (digitalClock2) {
        connect(digitalClock2, &digital_clock::timeChanged, this, [this](int seconds) {
            if (mode != FTR::FlowMode) return;
            qDebug() << "DEBUG: Лямбда поймала timeChanged! Секунд:" << seconds;
            if (activeFlowIndex < 0 || activeFlowIndex >= flowTasksList.size()) return;
            if (!flowTabs || flowTabs->currentIndex() != 1) return;
            int minutes = seconds / 60;
            if (seconds % 60 != 0) minutes++;
            FlowTask& ft = flowTasksList[activeFlowIndex];
            ft.timeEdit->blockSignals(true);
            ft.timeEdit->setText(QString::number(minutes));
            ft.timeEdit->blockSignals(false);
        });
    }
    if (this->mode == FTR::FlowMode){
        stopwatchPage = buildFlowTabPage(FTR::stopwatch, priorityBuilder);
        timerPage     = buildFlowTabPage(FTR::timer, priorityBuilder);
        flowTabs->addTab(stopwatchPage, tr("Секундомер"));
        flowTabs->addTab(timerPage, tr("Таймер"));
        vbox->addWidget(flowTabs, 1);
        connect(flowTabs, &QTabWidget::currentChanged, this, [this](int idx) {
            SPDLOG_DEBUG("addFlowModeInterface(tabChanged): idx = {}", idx);
            applyClockMode(idx == 0 ? FTR::stopwatch : FTR::timer);
            updateClockEditable();
        });
        applyClockMode(FTR::stopwatch);}
    else {
        timerPage     = buildFlowTabPage(FTR::timer, priorityBuilder);
        flowTabs->addTab(timerPage,tr("Таймер"));
        vbox->addWidget(flowTabs, 1);
        applyClockMode(FTR::timer);
    }
    SPDLOG_TRACE("addFlowModeInterface: завершено");
}

void BaseTracker::addTaskListInterface(QVBoxLayout* layout, int workMin, int breakMin)
{
    if (!layout) {
        SPDLOG_WARN("addTaskListInterface: layout == nullptr — выход");
        return;
    }
    SPDLOG_TRACE("addTaskListInterface: workMin = {}, breakMin = {}", workMin, breakMin);
    workMinutes  = workMin;
    breakMinutes = breakMin;
    //Стек
    pomoStack = new QStackedWidget(this);
    pomoStack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //Центрируем стек по горизонтали
    QHBoxLayout* centerRow = new QHBoxLayout();
    centerRow->addStretch();
    centerRow->addWidget(pomoStack, 1);
    centerRow->addStretch();
    layout->addLayout(centerRow, 1);   // 1 = забирает всё свободное место
    //Страница задач
    tasksPage = new QWidget();
    QVBoxLayout* tasksLayout = new QVBoxLayout(tasksPage);
    QHBoxLayout* headers = new QHBoxLayout();
    QLabel* t_name = new QLabel(tr("Название задачи"));
    t_name->setStyleSheet("background:transparent");
    headers->addStretch();
    headers->addWidget(t_name);
    headers->addStretch();
    QLabel* t_count = new QLabel(tr("Количество"));
    headers->addWidget(t_count);
    t_count->setStyleSheet("background:transparent");
    headers->addStretch();
    QLabel* t_done = new QLabel(tr("Выполнено"));
    headers->addWidget(t_done);
    t_done->setStyleSheet("background:transparent");
    QLabel* t_start = new QLabel(tr("Старт"));
    headers->addWidget(t_start);
    t_start->setStyleSheet("background:transparent");
    QLabel* t_del = new QLabel(tr("Удалить"));
    headers->addWidget( t_del);
    t_del->setStyleSheet("background:transparent");
    tasksLayout->addLayout(headers);
    // Скролл
    createTaskScroll(tasksLayout);
    //Страница активного помидора
    pomodoroPage = new QWidget();
    QVBoxLayout* pomoLayout = new QVBoxLayout(pomodoroPage);
    activeTaskLabel = new QLabel(tr("Задача: —"));
    activeTaskLabel->setStyleSheet(
        "color: white; font-size: 24px; font-weight: bold;");
    activeTaskLabel->setAlignment(Qt::AlignCenter);
    pomoLayout->addWidget(activeTaskLabel);
    //  Выбранная активность
    p_activeActivityLabel = new QLabel(tr("Активность: —"), pomodoroPage);
    p_activeActivityLabel->setStyleSheet(
        "color: #ffd54f; font-size: 16px; font-weight: bold; background: transparent;");
    p_activeActivityLabel->setAlignment(Qt::AlignCenter);
    pomoLayout->addWidget(p_activeActivityLabel);
    // Подзадача
    p_activeSubtaskLabel = new QLabel(tr("Подзадача: —"), pomodoroPage);
    p_activeSubtaskLabel->setStyleSheet(
        "color: white; font-size: 20px; font-weight: bold; background: transparent;");
    p_activeSubtaskLabel->setAlignment(Qt::AlignCenter);
    pomoLayout->addWidget(p_activeSubtaskLabel);
    pomoClock = new digital_clock(pomodoroPage);
    pomoClock->setColor(Qt::white);
    pomoClock->setShowSeconds(true);
    pomoClock->setEditable(false);
    pomoClock->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pomoLayout->addWidget(pomoClock, 0, Qt::AlignHCenter);
    // Кнопки под часами
    QHBoxLayout* pomoBtns = new QHBoxLayout();
    pomoBtns->setAlignment(Qt::AlignHCenter);
    pomoLayout->addLayout(pomoBtns);
    pomoLayout->addStretch();
    activeClock = pomoClock;
    setupButtons(pomoBtns);
    QPushButton* backBtn = new QPushButton(tr("← К задачам"));
    pomoLayout->addWidget(backBtn);
    // =Первая строка задачи
    addTaskRow();
    // Кнопка "Добавить задачу"
    QPushButton* addBtn = new QPushButton(tr("Добавить задачу"));
    tasksLayout->addWidget(addBtn);
    //  Стек
    pomoStack->addWidget(tasksPage);
    pomoStack->addWidget(pomodoroPage);
    pomoStack->setCurrentWidget(tasksPage);
    //  Коннекты
    connect(addBtn, &QPushButton::clicked, this, [this]() {
        SPDLOG_TRACE("addTaskListInterface(addBtn): добавление строки задачи");
        QLineEdit* newLine = addTaskRow();
        if (newLine) newLine->setFocus();
    });
    connect(backBtn, &QPushButton::clicked, this, [this]() {
        SPDLOG_DEBUG("addTaskListInterface(backBtn): возврат к списку задач");
        if (pomoClock) pomoClock->timer_stop();
        finishCurrentSession(false);
        if (pomoStack && tasksPage)
            pomoStack->setCurrentWidget(tasksPage);
        if (digitalClock) digitalClock->show();
    });
    //  Стили
    tasksPage->setStyleSheet(
        "QWidget { background: rgba(0,0,0,0.6); border-radius: 10px; }"
        "QLabel { color: white; }"
        "QLineEdit { background: white; color: black; border-radius: 5px; padding: 4px; }"
        "QPushButton { background: rgba(255,255,255,0.2); color: white;"
        "              border: 2px solid rgba(255,255,255,0.4);"
        "              border-radius: 8px; padding: 6px; }"
        "QPushButton:hover { background: rgba(255,255,255,0.35); }"
        "QCheckBox { color: white; }"
        "QScrollArea { background: transparent; border: none; }"
        "QScrollBar:vertical {"
        "    background: rgba(0,0,0,0.3);"
        "    width: 10px;"
        "    border-radius: 5px;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background: rgba(255,255,255,0.4);"
        "    border-radius: 5px;"
        "    min-height: 20px;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "    height: 0;"
        "}"
        );

    pomodoroPage->setStyleSheet(
        "QWidget { background: rgba(0,0,0,0.6); border-radius: 10px; }"
        "QLabel { color: white; }"
        "QPushButton { background: rgba(255,255,255,0.2); color: white;"
        "              border: 2px solid rgba(255,255,255,0.4);"
        "              border-radius: 8px; padding: 6px; }"
        );
    tasksLayout->setContentsMargins(8, 8, 8, 8);
    SPDLOG_TRACE("addTaskListInterface: завершено");
}

void BaseTracker::hideEvent(QHideEvent* e)//отрабатывает при вызове hide()
{
    SPDLOG_TRACE("hideEvent: закрытие сессии при скрытии трекера");
    finishCurrentSession(false);
    QWidget::hideEvent(e);
}

void BaseTracker::createTaskScroll(QVBoxLayout* tasksLayout)//установка скроллэрии
{
    if (!tasksLayout) return;
    SPDLOG_TRACE("createTaskScroll: создание области прокрутки");
    rowsContainer = new QWidget();
    rowsLayout    = new QVBoxLayout(rowsContainer);
    rowsLayout->setContentsMargins(0, 0, 0, 0);
    rowsLayout->setSpacing(4);
    rowsLayout->addStretch();
    taskScroll = new QScrollArea();
    taskScroll->setWidgetResizable(true);
    taskScroll->setWidget(rowsContainer);
    taskScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    taskScroll->setFrameShape(QFrame::NoFrame);
    tasksLayout->addWidget(taskScroll, 1);
    tasksLayout->addStretch();
}

// поведение стека с тасками при расширении экрана
void BaseTracker::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    // POMODORO
    if (pomoStack) {
        const int maxAllowed = qBound(250, width()/2-50, 550);
        const int maxAllowedH = qBound(300, height()/2, 800);
        pomoStack->setMinimumWidth(250);
        pomoStack->setMaximumWidth(maxAllowed);
        pomoStack->setMinimumHeight(300);
        pomoStack->setMaximumHeight(maxAllowedH);
        pomoStack->updateGeometry();
        if (pomoClock) {
            pomoClock->setMaximumWidth(maxAllowed);
            pomoClock->setMinimumWidth(250);
            pomoClock->updateGeometry();
        }
    }
    // FLOWMODE / EATTHEFROG
    if (flowTabs) flowTabs->updateGeometry();

    //TIMEBLOCKING
    if (mode == FTR::TimeBlocking && blockFrame) {
        // Вычисление желаемой ширины: 50% от окна, но не меньше 300 и не больше 750
        int desiredWidth = qBound(300, width() / 2-65, 750);
        blockFrame->setMinimumWidth(desiredWidth);
        blockFrame->setMaximumWidth(desiredWidth);
        qDebug() << "TIMEBLOCKING resize: window width =" << width()
                 << " frame width =" << desiredWidth;
    }
}

//отрисовка интерфейса в зависимости от модуля
void BaseTracker::add_mode_interface(QVBoxLayout* layout)
{
    if (!layout) {
        SPDLOG_WARN("add_mode_interface: layout == nullptr — выход");
        return;
    }
    SPDLOG_INFO("add_mode_interface: mode = {}", static_cast<int>(mode));
    mode_layout = layout;
    switch (mode){
    case FlowMode:
        addFlowModeInterface(layout, nullptr);
        break;
    case EatTheFrog:
        addFlowModeInterface(layout, &BaseTracker::buildPriorityCombo);
        break;
    case TimeBlocking:
        addTimeBlockingInterface(layout);
        break;
    case Pomodoro:
        addTaskListInterface(layout, 25, 5);
        break;
    case ReversePomodoro:
        addTaskListInterface(layout, 5, 25);
        break;
    case Mode60_10:
        addTaskListInterface(layout, 60, 10);
        break;
    case Mode52_17:
        addTaskListInterface(layout, 52, 17);
        break;
    case Mode90_20:
        addTaskListInterface(layout, 90, 20);
        break;
    }
}


void BaseTracker::on_pushButton_clicked()
{
    SPDLOG_INFO("on_pushButton_clicked: выход из трекера (mode = {})",
                static_cast<int>(mode));
    // Остановка таймеров
    stopAllTimers();
    //Остановка последовательности задач
    stopTaskSequence();          // для Flow/EatTheFrog
    // Запись незавершённой сессии в БД
    if (sessionStart.isValid())
        finishCurrentSession(false);//помодоро и таймблокинг
    //  Возврат на главное окно
    getback_clicked();
    SPDLOG_TRACE("on_pushButton_clicked: возврат в главное окно выполнен");
}

void BaseTracker::stopAllTimers() {
    SPDLOG_TRACE("stopAllTimers: остановка всех таймеров");
    auto stopClock = [](digital_clock* c){
        if (!c) return;
        if (c->getMode() == FTR::stopwatch) c->stopwatch_stop();
        else c->timer_stop();
    };
    stopClock(activeClock);
    stopClock(clock);
    stopClock(digitalClock);
    stopClock(digitalClock2);
    stopClock(pomoClock);
    if (taskTimer  && taskTimer->isActive())  taskTimer->stop();
    if (blockTimer && blockTimer->isActive()) blockTimer->stop();
    SPDLOG_TRACE("stopAllTimers: завершено");
}

void BaseTracker::add_Task(const QString &arg1) {
    QString name = arg1.trimmed();
    if (name.isEmpty()) return;
    QComboBox* combo = activityComboBox();
    if (!combo) {
        SPDLOG_WARN("add_Task: activityComboBox() == nullptr — выход");
        return;
    }
    SPDLOG_TRACE("add_Task: name = '{}'", name.toStdString());
    combo->setCurrentText(name);
    ensureTaskRegistered(combo);
}

void BaseTracker::loadTasksFromDb()//загрузка активности из бд
{
    SPDLOG_TRACE("loadTasksFromDb: обновление activityComboBox");
    refreshTaskCombo(activityComboBox());
}

void BaseTracker::on_pushButton_2_clicked()   // Секундомер
{
    if (mode == FTR::FlowMode){
        SPDLOG_INFO("on_pushButton_2_clicked: переход в режим Секундомер");
        QMessageBox::information(this, "Секундомер", "Режим секундомера");
        digitalClock->setMode(stopwatch);
        ensureButtonsCreated();
        if (!mode_label) {
            mode_label = new QLabel(this);
            mode_label->setStyleSheet(
                "QLabel {"
                "   background: rgba(0, 0, 0, 0.6);"
                "   color: white;"
                "   border: 2px solid rgba(255, 255, 255, 0.3);"
                "   border-radius: 10px;"
                "   font-size: 14px;"
                "   font-weight: bold;"
                "   padding: 10px;"
                "}");
            mode_label->setFixedSize(250, 50);
            mode_label->setAlignment(Qt::AlignCenter);
            parent_buttonLayout->insertWidget(0, mode_label, 1, Qt::AlignHCenter);
        }
        mode_label->setText("Секундомер");
    }
    else return;
}

void BaseTracker::on_pushButton_3_clicked()   // Таймер
{
    if (mode == FTR::FlowMode){
        SPDLOG_INFO("on_pushButton_3_clicked: переход в режим Таймер");
        QMessageBox::information(this, "Таймер", "Режим таймера");
        analogClock->setMode(timer);
        digitalClock->setMode(timer);
        ensureButtonsCreated();
        if (!mode_label) {
            mode_label = new QLabel(this);
            mode_label->setStyleSheet(
                "QLabel {"
                "   background: rgba(0, 0, 0, 0.6);"
                "   color: white;"
                "   border: 2px solid rgba(255, 255, 255, 0.3);"
                "   border-radius: 10px;"
                "   font-size: 14px;"
                "   font-weight: bold;"
                "   padding: 10px;"
                "}"
                );
            mode_label->setFixedSize(250, 50);
            mode_label->setAlignment(Qt::AlignCenter);
            parent_buttonLayout->insertWidget(0, mode_label, 1, Qt::AlignHCenter);
        }
        mode_label->setText("Таймер");
    }
    else return;
}

//обновление  окрана в зависимости от активности юзера
void BaseTracker::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    SPDLOG_TRACE("showEvent: трекер показан");
    loadCurrentUser();
    loadUserProfile();
    connectInfoLink();
    QComboBox* combo = activityComboBox();
    if (combo && !comboConnected) {
        SPDLOG_DEBUG("showEvent: инициализация activityComboBox");
        combo->setEditable(true);
        combo->setInsertPolicy(QComboBox::NoInsert);
        if (combo->lineEdit()) {
            connect(combo->lineEdit(), &QLineEdit::editingFinished,
                    this, [this]() {
                        QComboBox* c = activityComboBox();
                        if (!c) return;
                        QString name = c->currentText().trimmed();
                        if (name.isEmpty()) return;
                        if (c->findText(name, Qt::MatchFixedString) == -1) {
                            SPDLOG_TRACE("showEvent(editingFinished): регистрация '{}'",
                                         name.toStdString());
                            add_Task(name);
                        }
                    });
        }
        loadTasksFromDb();
        comboConnected = true;
    }
}

//метод возврата на предыдущее окно
void BaseTracker::getback_clicked()
{
    SPDLOG_TRACE("getback_clicked: показ главного окна и скрытие трекера");
    if (openWindow) {openWindow->show();}
    this->hide();
}

//запись окончания сессии и отправка в бд
void BaseTracker::finishCurrentSession(bool is_completed)
{
    if (!sessionStart.isValid()) return;
    Data_Manager& db = Data_Manager::instance();
    const qint64 uid = db.get_currentUser();
    if (uid < 0) {
        sessionStart = QDateTime();
        return;
    }
    if (currentTaskId <= 0) {
        qDebug() << "finishCurrentSession: SKIP, currentTaskId invalid =" << currentTaskId;
        sessionStart = QDateTime();
        return;
    }
    Session_info s;
    s.user_id      = uid;
    s.task_id      = currentTaskId;
    s.technic      = mode;
    s.phase        = currentPhase;
    s.start_time   = sessionStart;
    s.finish_time  = QDateTime::currentDateTime();
    s.duration_sec = quint32(s.start_time.secsTo(s.finish_time));
    s.completed    = is_completed;
    if (s.duration_sec > 0)db.sendSessionData(s);
    sessionStart = QDateTime();
    qDebug() << "finishCurrentSession: phase =" << phaseToStr(currentPhase)
             << " duration =" << s.duration_sec << " sec";
}

//установка фона
void BaseTracker::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    QPixmap bg(backgroundImage());
    if (bg.isNull()) return;
    QSize s = bg.size().scaled(size(), Qt::KeepAspectRatioByExpanding);
    QPixmap scaled = bg.scaled(s, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    p.drawPixmap((width()  - scaled.width())  / 2,
                 (height() - scaled.height()) / 2, scaled);
}

//размещение часов в лэйауте
void BaseTracker::placeClocksInLayout(MyClock* cl,digital_clock* clock,QVBoxLayout *layout, QHBoxLayout *buttonlayout)
{
    if (!clock) return;
    if (!layout) return;
    parent_buttonLayout = layout;
    if (buttonlayout && layout->indexOf(buttonlayout) != -1)
        layout->removeItem(buttonlayout);
    if (cl && layout->indexOf(cl) == -1)
        layout->insertWidget(0, cl, 0, Qt::AlignHCenter);
    if (clock && layout->indexOf(clock) == -1) {
        int pos = (analogClock && layout->indexOf(cl) != -1) ? 1 : 0;
        layout->insertWidget(pos, clock, 0, Qt::AlignHCenter);
    }
    if (buttonlayout) {
        buttonLayout = buttonlayout;
        int pos = layout->count();
        if (clock) {
            int clockIdx = layout->indexOf(clock);
            if (clockIdx != -1) pos = clockIdx + 1;
        }
        layout->insertLayout(pos, buttonlayout, 0);   // stretch 0
    }
}


void BaseTracker::playNotificationSound(){QApplication::beep();}//Звуковая нотификация

void BaseTracker::ensureButtonsCreated()//установка кнопок
{
    if (!buttonLayout) return;
    if (startBtn) return;
    setupButtons(buttonLayout);
}

void BaseTracker::setupButtons(QHBoxLayout *buttonlayout)
{
    if (!buttonlayout) return;
    // (создание startBtn, pauseBtn, stopBtn, стили)
    startBtn = new QToolButton(this);  startBtn->setText("▶");
    pauseBtn = new QToolButton(this);  pauseBtn->setText("❚❚");
    stopBtn  = new QToolButton(this);  stopBtn->setText("■");
    buttonlayout->addWidget(startBtn);
    buttonlayout->addWidget(pauseBtn);
    buttonlayout->addWidget(stopBtn);
    startBtn->setToolTip("Старт");
    pauseBtn->setToolTip("Пауза");
    stopBtn->setToolTip("Стоп");
    QString style =
        "QToolButton { color: white; background: rgba(0, 0, 0, 0.6); font-weight:bold; }"
        "QToolButton:hover { background: rgba(255, 0, 0, 0.6); }";
    startBtn->setStyleSheet(style);
    pauseBtn->setStyleSheet(style);
    stopBtn->setStyleSheet(style);
    // КНОПКА СТАРТ (▶)
    connect(startBtn, &QToolButton::clicked, this, [this]() {
        SPDLOG_DEBUG("setupButtons(startBtn): mode = {}", static_cast<int>(mode));
        if (digitalClock2 && digitalClock2->getMode() == FTR::timer && digitalClock2->is_running())
            return;
        if (mode == FTR::EatTheFrog) {
            startTaskSequence();
            return;
        }
        else if (mode == FTR::TimeBlocking) {
            if (digitalClock2 && digitalClock2->is_running()) return;
            startSequence();
            return;
        }
        else if (mode == Pomodoro || mode == ReversePomodoro ||
                 mode == Mode60_10 || mode == Mode52_17 || mode == Mode90_20) {
            if (!pomoClock) return;
            // Если таймер уже тикает — ничего не делаем
            if (pomoClock->is_running()) return;
            // Если таймер на паузе (осталось время > 0) — продолжаем
            if (pomoClock->get_timer_time() > 0) {
                SPDLOG_DEBUG("setupButtons(startBtn): продолжение помодоро с паузы");
                pomoClock->timer_start(0, 0, 0); // нули, чтобы он паузу
                sessionStart = QDateTime::currentDateTime(); // возобновление сессии
                return;
            }
            // Если таймер дошел до нуля (был остановлен) - новая фаза
            SPDLOG_DEBUG("setupButtons(startBtn): новая фаза РАБОТА");
            work = true;
            currentPhase = Phase::Work;
            if (activeTaskLabel)
                activeTaskLabel->setText(tr("Работа %1 мин").arg(workMinutes));
            pomoClock->setMode(FTR::timer);
            pomoClock->timer_start(0, workMinutes, 0);
            sessionStart = QDateTime::currentDateTime();
            return;
        }
        if (mode == FTR::FlowMode) {
            qDebug() << "DEBUG: Нажата ОБЩАЯ кнопка Старт. Размер списка:" << flowTasksList.size()
                     << "Индекс:" << activeFlowIndex;
            // Проверка что список не пуст
            if (flowTasksList.isEmpty()) {
                SPDLOG_WARN("setupButtons(startBtn,FlowMode): список задач пуст");
                QMessageBox::information(this, "Внимание", "Список задач пуст. Добавьте задачу.");
                return;
            }
            // Если задача не выбрана (индекс -1), поиск первой с текстом
            if (activeFlowIndex < 0 || activeFlowIndex >= flowTasksList.size()) {
                int validIdx = -1;
                for (int i = 0; i < flowTasksList.size(); ++i) {
                    if (!flowTasksList[i].nameEdit->currentText().trimmed().isEmpty()) {
                        validIdx = i;
                        break;
                    }
                }
                if (validIdx == -1) {
                    SPDLOG_WARN("setupButtons(startBtn,FlowMode): нет заполненных задач");
                    QMessageBox::information(this, "Ошибка", "Нет заполненных задач.");
                    return;
                }
                activeFlowIndex = validIdx;
            }
            //  индекс валиден, взятие задачи
            FlowTask& ft = flowTasksList[activeFlowIndex];
            // проверка текста
            QString name = ft.nameEdit->currentText().trimmed();
            if (name.isEmpty()) {
                SPDLOG_WARN("setupButtons(startBtn,FlowMode): пустое имя задачи");
                QMessageBox::information(this, "Ошибка", "Введите название задачи перед запуском.");
                ft.nameEdit->setFocus();
                return;
            }
            SPDLOG_DEBUG("setupButtons(startBtn,FlowMode): клик по startBtn задачи '{}'",
                         name.toStdString());
            //имитация клика мыши по кнопке
            if (ft.startBtn) {
                ft.startBtn->click();
            } else {
                SPDLOG_WARN("setupButtons(startBtn,FlowMode): ft.startBtn == nullptr");
                QMessageBox::warning(this, "Ошибка", "Кнопка запуска для этой задачи не найдена. "
                                                     "Попробуйте добавить задачу заново.");
            }
            return;
        }
    });
    // КНОПКА ПАУЗА (❚❚)
    connect(pauseBtn, &QToolButton::clicked, this, [this](){
        SPDLOG_DEBUG("setupButtons(pauseBtn): mode = {}", static_cast<int>(mode));
        if (mode == FTR::EatTheFrog) {
            pauseTaskSequence();
            return;
        }
        else if (mode == FTR::TimeBlocking) {
            if (digitalClock2) digitalClock2->timer_pause();
            finishCurrentSession(false);
            if (blockTitleLabel)
                blockTitleLabel->setText(tr("Текущий блок: %1 (пауза)").arg(currentBlockName));
            return;
        }
        else if (mode == FTR::FlowMode) {
            if (digitalClock2) {
                if (digitalClock2->getMode() == FTR::stopwatch) digitalClock2->stopwatch_pause();
                else digitalClock2->timer_pause();
            }
            finishCurrentSession(false);
            return;
        }
        // Для Помодоро
        if (analogClock) {
            switch (analogClock->get_mode()) {
            case stopwatch: analogClock->stopwatch_pause(); break;
            case timer:     analogClock->timer_pause();     break;
            default: break;
            }
        }
        digital_clock* c = activeClock ? activeClock : digitalClock2;
        if (!c) return;
        switch (c->getMode()) {
        case stopwatch: c->stopwatch_pause(); break;
        case timer:     c->timer_pause();     break;
        default: break;
        }
        finishCurrentSession(false);
    });

    // КНОПКА СТОП (■)
    connect(stopBtn, &QToolButton::clicked, this, [this]() {
        SPDLOG_DEBUG("setupButtons(stopBtn): mode = {}", static_cast<int>(mode));
        if (mode == FTR::EatTheFrog) {
            stopTaskSequence();
            if (digitalClock2) {
                digitalClock2->timer_stop();
                digitalClock2->set_timer_time(0, 0, 0);
            }
            return;
        }
        else if (mode == FTR::FlowMode) {
            stopFlowClockAndSession();
            return;
        }
        else if (mode == FTR::TimeBlocking) {
            if (blockTimer) blockTimer->stop();
            if (digitalClock2) {
                digitalClock2->timer_stop();
                digitalClock2->set_timer_time(0, 0, 0);
            }
            finishCurrentSession(false);
            currentBlockIndex = -1;
            isBreakTime = false;
            if (blockTitleLabel) blockTitleLabel->setText(tr("Текущий блок: -"));
            if (blockRangeLabel) blockRangeLabel->setText(tr("- : -"));
            return;
        }
        // Для Помодоро
        if (analogClock) {
            switch (analogClock->get_mode()) {
            case stopwatch: analogClock->stopwatch_stop(); break;
            case timer:     analogClock->timer_stop();     break;
            default: break;
            }
        }
        digital_clock* c = activeClock ? activeClock : digitalClock2;
        if (!c) return;
        switch (c->getMode()) {
        case stopwatch: c->stopwatch_stop(); break;
        case timer:     c->timer_stop();     break;
        default: break;
        }
        finishCurrentSession(false);
    });
}

bool BaseTracker::applyClockMode(FTR::ClockMode clockMode)
{
    if (mode == FTR::TimeBlocking||mode == FTR::FlowMode||mode == FTR::EatTheFrog){
        if (digitalClock2) digitalClock2->setMode(clockMode);
        ensureButtonsCreated();
        return true;}
    else
        return false;
}

void BaseTracker::loadUserProfile()//загрузка фото профиля
{
    QLabel* av = avatarLabel();
    if (!av) return;
    Data_Manager& dm = Data_Manager::instance();
    User_Profile me = dm.getCurrentUserProfile();
    // ГОСТЬ
    if (me.user_id < 0) {
        SPDLOG_TRACE("loadUserProfile: гость — стандартный аватар");
        QPixmap pix(":/img/exit (1).jpg");
        if (!pix.isNull()) {
            av->setPixmap(pix.scaled(180, 180,
                                     Qt::KeepAspectRatio,
                                     Qt::SmoothTransformation));
            av->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        }
        return;
    }
    // АВАТАР
    if (!me.Avatar.isEmpty()) {
        QPixmap avatar;
        if (avatar.loadFromData(me.Avatar)) {
            SPDLOG_TRACE("loadUserProfile: установлен аватар пользователя id = {}",
                         me.user_id);
            av->setPixmap(avatar.scaled(180, 180,
                                        Qt::KeepAspectRatio,
                                        Qt::SmoothTransformation));
            av->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            return;
        }
    }
    // ЗАГЛУШКА
    av->setPixmap(QPixmap(":/img/icon_question.png")
                      .scaled(180, 180,
                              Qt::KeepAspectRatio,
                              Qt::SmoothTransformation));
    av->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void BaseTracker::loadCurrentUser()//загрузка профиля юзера
{
    QLabel* ul = userLabel();
    if (!ul) return;
    Data_Manager& dm = Data_Manager::instance();
    User_Profile me = dm.getCurrentUserProfile();
    if (me.user_id < 0) {
        SPDLOG_TRACE("loadCurrentUser: гость");
        ul->setText("<a href=\"#guest\" style=\"color: white; font-weight: bold;\">Гость</a>");
        return;
    }
    SPDLOG_TRACE("loadCurrentUser: пользователь id = {}, '{} {}'",
                 me.user_id, me.Name.toStdString(), me.Surname.toStdString());
    ul->setText(
        QString("<a href=\"#open\" style=\"color: white; font-weight: bold;\">"
                "%1 %2</a>")
            .arg(me.Name, me.Surname));
}

void BaseTracker::addTimeBlockingInterface(QVBoxLayout* layout)
{
    if (!layout) return;
    SPDLOG_TRACE("addTimeBlockingInterface: начало");
    if (digitalClock2) {
        digitalClock2->setEditable(false);
        disconnect(digitalClock2, &digital_clock::timerFinished, this, nullptr);
    }
    QFrame* frame = new QFrame(this);
    frame->setObjectName("blockFrame");
    frame->setStyleSheet(
        "QFrame#blockFrame { background: rgba(1,1,1,0.6); border-radius: 10px; }"
        "QLabel { color: white; }");
    frame->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    this->blockFrame = frame;
    QHBoxLayout* centerRow = new QHBoxLayout();
    centerRow->addStretch();
    centerRow->addWidget(frame, 1);
    layout->addLayout(centerRow, 1);
    QVBoxLayout* vbox = new QVBoxLayout(frame);
    vbox->setContentsMargins(8, 8, 8, 8);
    vbox->setSpacing(4);
    // Карточка текущего блока (внутри фрэйма)
    QWidget* currentCard = new QWidget(frame);
    currentCard->setObjectName("currentCard");
    currentCard->setStyleSheet(
        "QWidget#currentCard { background: rgba(255,255,255,0.1); border-radius: 8px; }"
        "QLabel { color: white; }");
    QVBoxLayout* curLayout = new QVBoxLayout(currentCard);
    curLayout->setContentsMargins(8, 8, 8, 8);
    curLayout->setSpacing(4);
    QHBoxLayout* btnRow = new QHBoxLayout();
    btnRow->setContentsMargins(0, 0, 0, 0);
    btnRow->setSpacing(6);
    vbox->addLayout(btnRow);
    placeClocksInLayout(nullptr,digitalClock2,vbox,btnRow);
    setupButtons(btnRow);
    vbox->addWidget(currentCard);
    blockTitleLabel = new QLabel(tr("Текущий блок: -"), currentCard);
    blockTitleLabel->setStyleSheet("font-size: 20px; font-weight: bold;");
    blockTitleLabel->setAlignment(Qt::AlignCenter);
    curLayout->addWidget(blockTitleLabel,1);
    blockRangeLabel = new QLabel(tr("- : -"), currentCard);
    blockRangeLabel->setAlignment(Qt::AlignCenter);
    curLayout->addWidget(blockRangeLabel);
    //  Расписание дня
    QLabel* scheduleLabel = new QLabel(tr("Расписание дня"), this);
    scheduleLabel->setStyleSheet("color: white; font-weight: bold;");
    scheduleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(scheduleLabel);
    vbox->addWidget(scheduleLabel);
    QHBoxLayout* columns = new QHBoxLayout();
    vbox->addLayout(columns);
    columns->addStretch();
    QLabel* task_name = new QLabel("Название задачи");
    columns->addWidget(task_name );
    columns->addStretch();
    QLabel* task_time = new QLabel("Время задачи");
    columns->addWidget(task_time);
    QLabel* break_time = new QLabel("Время отдыха");
    columns->addWidget(break_time);
    QLabel* start_task = new QLabel("Старт");
    columns->addWidget(start_task);
    QLabel* delete_task = new QLabel("Удалить");
    columns->addWidget(delete_task);
    blockRowsContainer = new QWidget(this);
    blockRowsLayout = new QVBoxLayout(blockRowsContainer);
    blockRowsLayout->setContentsMargins(0, 0, 0, 0);
    blockRowsLayout->setSpacing(4);
    blockRowsLayout->addStretch();
    QScrollArea* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setWidget(blockRowsContainer);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet(
        "QScrollArea { background: transparent; border: none; }"
        "QScrollArea > QWidget > QWidget { background: transparent; }"
        );
    vbox->addWidget(scroll, 1);
    // Кнопка «Добавить блок»
    QPushButton* addBtn = new QPushButton(tr("+ Добавить блок"), this);
    addBtn->setStyleSheet(
        "QPushButton {"
        "  color: black;font-weight: bold;"
        "  background-color: rgba(255,255,255,0.5);"
        "  border: 1px solid rgba(255,255,255,0.4);"
        "  border-radius: 4px;"
        "  padding: 2px;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(255,0,0,0.5);"
        "}"
        );
    vbox->addWidget(addBtn);
    connect(addBtn, &QPushButton::clicked, this, [this]() {
        SPDLOG_TRACE("addTimeBlockingInterface(addBtn): добавление блока");
        QWidget* row = buildBlockRow(tr(""), 60, 15);
        blockRowsLayout->insertWidget(blockRowsLayout->count() - 1, row);
    });
    QWidget* first = buildBlockRow("", 30, 10);
    blockRowsLayout->insertWidget(blockRowsLayout->count() - 1, first);
    SPDLOG_TRACE("addTimeBlockingInterface: завершено");
}

QWidget* BaseTracker::buildBlockRow(const QString& name, int durationMin, int bufferMin)
{
    SPDLOG_TRACE("buildBlockRow: name = '{}', durationMin = {}, bufferMin = {}",
                 name.toStdString(), durationMin, bufferMin);
    QWidget* row = new QWidget();
    row->setStyleSheet("QWidget { background: rgba(0,0,0,0.5); border-radius: 8px; } "
                       "QLabel { color: black; }");
    QHBoxLayout* h = new QHBoxLayout(row);
    h->setContentsMargins(6, 4, 6, 4);
    // Комбобокс задачи
    QComboBox* taskCombo = new QComboBox(row);
    taskCombo->setEditable(true);
    taskCombo->setInsertPolicy(QComboBox::NoInsert);
    taskCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    taskCombo->setMinimumWidth(90);
    taskCombo->setFixedHeight(22);
    taskCombo->setStyleSheet(
        "QComboBox { color: black; font-weight: bold; background-color: rgba(255,255,255,0.5); "
        "  border: 1px solid rgba(255,255,255,0.4); border-radius: 4px; padding: 2px; }"
        "QComboBox:hover { background-color: rgba(255,0,0,0.5); }"
        "QComboBox QLineEdit { background: transparent; border: none; color: black; }"
        "QComboBox QAbstractItemView { background-color: rgba(30,30,30,0.9); color: white; "
        "  selection-background-color: rgba(255,0,0,0.5); }"
        );
    // Заполняем существующими задачами из БД
    refreshTaskCombo(taskCombo);
    // Если передали название при создании
    if (!name.isEmpty()) {
        int idx = taskCombo->findText(name, Qt::MatchFixedString);
        if (idx >= 0) {
            taskCombo->setCurrentIndex(idx);
        } else {
            taskCombo->setCurrentText(name);
        }
    }
    taskCombo->setPlaceholderText(tr("Введите или выберите задачу"));
    h->addWidget(taskCombo, 0);
    // Поле минут работы
    QSpinBox* durSpin = new QSpinBox(row);
    durSpin->setStyleSheet(
        "QSpinBox { color: black; font-weight: bold; background-color: rgba(255,255,255,0.5); "
        "  border: 1px solid rgba(255,255,255,0.4); border-radius: 4px; padding: 2px; }"
        "QSpinBox:hover { background-color: rgba(255,0,0,0.5); }"
        );
    durSpin->setRange(1, 999);
    durSpin->setValue(durationMin);
    durSpin->setToolTip(tr("Введите время блока"));
    durSpin->setSuffix(tr(" мин"));
    h->addWidget(durSpin);
    // Поле минут буфера
    QSpinBox* bufSpin = new QSpinBox(row);
    bufSpin->setStyleSheet(
        "QSpinBox { color: black; font-weight: bold; background-color: rgba(255,255,255,0.5); "
        "  border: 1px solid rgba(255,255,255,0.4); border-radius: 4px; padding: 2px; }"
        "QSpinBox:hover { background-color: rgba(255,0,0,0.5); }"
        );
    bufSpin->setRange(0, 999);
    bufSpin->setValue(bufferMin);
    bufSpin->setToolTip(tr("Введите время перерыва"));
    bufSpin->setSuffix(tr(" мин"));
    h->addWidget(bufSpin);
    // Кнопки
    QPushButton* startBtn = new QPushButton("▶", row);
    startBtn->setFixedWidth(30);
    startBtn->setStyleSheet(
        "QPushButton { color: black; font-weight: bold; background-color: rgba(255,255,255,0.5); "
        "  border: 1px solid rgba(255,255,255,0.4); border-radius: 4px; padding: 2px; }"
        "QPushButton:hover { background-color: rgba(255,0,0,0.5); }");
    QPushButton* removeBtn = new QPushButton("✕", row);
    removeBtn->setFixedWidth(30);
    removeBtn->setStyleSheet(
        "QPushButton { color: black; font-weight: bold; background-color: rgba(255,255,255,0.5); "
        "  border: 1px solid rgba(255,255,255,0.4); border-radius: 4px; padding: 2px; }"
        "QPushButton:hover { background-color: rgba(255,0,0,0.5); }"
        );
    h->addWidget(startBtn);
    h->addWidget(removeBtn);
    // Сохраняем структуру в модель
    TimeBlock block;
    block.name        = name;
    block.durationMin = durationMin;
    block.bufferMin   = bufferMin;
    block.rowWidget   = row;
    block.taskCombo   = taskCombo;
    block.durSpin     = durSpin;
    block.bufSpin     = bufSpin;
    block.taskId      = taskCombo->currentData().toLongLong();
    timeBlocks.append(block);
    SPDLOG_TRACE("buildBlockRow: блок добавлен, всего блоков = {}",
                 timeBlocks.size());
    //  РЕГИСТРАЦИЯ ЗАДАЧИ В БД СРАЗУ ПРИ ВВОДЕ (editingFinished)
    // Регистрация задачи при Enter
    if (taskCombo->lineEdit()) {
        if (taskCombo->lineEdit()) {
            connect(taskCombo->lineEdit(), &QLineEdit::returnPressed,
                    this, [this, taskCombo]() {
                        // Закрытие popup, иначе Enter обработает сам QComboBox
                        taskCombo->hidePopup();
                        const qint64 id = ensureTaskRegistered(taskCombo);
                        if (id > 0) {
                            SPDLOG_DEBUG("buildBlockRow(returnPressed): id = {} для '{}'",
                                         id, taskCombo->currentText().toStdString());
                            for (auto& b : timeBlocks) {
                                if (b.taskCombo == taskCombo) {
                                    b.name   = taskCombo->currentText().trimmed();
                                    b.taskId = id;
                                    break;
                                }
                            }
                        }
                    });
        }
        // Регистрация при выборе из выпадающего списка
        connect(taskCombo, QOverload<int>::of(&QComboBox::activated),
                this, [this, taskCombo](int) {
                    SPDLOG_TRACE("buildBlockRow(activated): регистрация '{}'",
                                 taskCombo->currentText().toStdString());
                    ensureTaskRegistered(taskCombo);
                });
        connect(this, &BaseTracker::tasksChanged, taskCombo, [this, taskCombo]{
            SPDLOG_TRACE("buildBlockRow(tasksChanged): refresh combo");
            refreshTaskCombo(taskCombo);
        });
    }
    //  СТАРТ КОНКРЕТНОГО БЛОКА
    //  поиск индекса в момент клика по rowWidget
    connect(startBtn, &QPushButton::clicked, this, [this, row, taskCombo]() {
        if (digitalClock2 && digitalClock2->is_running()) return;
        // Гарантия,что введённая задача попадёт в БД
        const qint64 id = ensureTaskRegistered(taskCombo);
        if (id <= 0) {
            SPDLOG_WARN("buildBlockRow(startBtn): ensureTaskRegistered вернул {} для '{}'",
                        id, taskCombo->currentText().toStdString());
            QMessageBox::information(this, tr("Внимание"),
                                     tr("Введите название задачи перед запуском блока."));
            taskCombo->setFocus();
            return;
        }
        // поиск индекса блока
        int idx = -1;
        for (int i = 0; i < timeBlocks.size(); ++i) {
            if (timeBlocks[i].rowWidget == row) { idx = i; break; }
        }
        if (idx < 0) {
            SPDLOG_WARN("buildBlockRow(startBtn): idx < 0 — блок не найден");
            return;
        }
        SPDLOG_INFO("buildBlockRow(startBtn): старт блока idx = {}, id = {}", idx, id);
        syncTimeBlocks();
        currentBlockIndex = idx;
        isBreakTime = false;
        processCurrentBlock();
    });
    //  УДАЛЕНИЕ СТРОКИ
    connect(removeBtn, &QPushButton::clicked, this, [this, row]() {
        // поиск индекса блока
        int idx = -1;
        for (int i = 0; i < timeBlocks.size(); ++i) {
            if (timeBlocks[i].rowWidget == row) { idx = i; break; }
        }
        if (idx < 0) {
            SPDLOG_WARN("buildBlockRow(removeBtn): idx < 0 — блок не найден");
            return;
        }
        SPDLOG_DEBUG("buildBlockRow(removeBtn): удаление блока idx = {}, "
                     "currentBlockIndex = {}",
                     idx, currentBlockIndex);
        // при удалении текущего работающего — стоп
        if (currentBlockIndex == idx) {
            if (blockTimer) blockTimer->stop();
            if (digitalClock2) {
                digitalClock2->timer_stop();
                digitalClock2->set_timer_time(0, 0, 0);
            }
            finishCurrentSession(false);
            currentBlockIndex = -1;
            isBreakTime = false;
            if (blockTitleLabel) blockTitleLabel->setText(tr("Текущий блок: -"));
            if (blockRangeLabel) blockRangeLabel->setText(tr("- : -"));
        }
        // при удалении блока до текущего - сдвиг индекса
        else if (currentBlockIndex > idx) {
            currentBlockIndex--;
        }
        // Удаление из модели
        timeBlocks.removeAt(idx);
        // удаление из layout
        if (blockRowsLayout) {
            blockRowsLayout->removeWidget(row);
        }
        // Удаление виджета
        row->deleteLater();
        SPDLOG_INFO("buildBlockRow(removeBtn): блок удалён, осталось {}",
                    timeBlocks.size());
    });
    return row;
}

void BaseTracker::syncTimeBlocks()
{
    SPDLOG_TRACE("syncTimeBlocks: блоков = {}", timeBlocks.size());
    for (auto& b : timeBlocks) {
        if (!b.taskCombo) continue;
        b.name   = b.taskCombo->currentText().trimmed();
        b.taskId = ensureTaskRegistered(b.taskCombo);
        if (b.durSpin) b.durationMin = b.durSpin->value();
        if (b.bufSpin) b.bufferMin   = b.bufSpin->value();
    }
    SPDLOG_TRACE("syncTimeBlocks: завершено");
}

qint64 BaseTracker::ensureTaskRegistered(QComboBox* combo)
{
    if (!combo) {
        SPDLOG_WARN("ensureTaskRegistered: combo == nullptr — выход");
        return -1;
    }
    const qint64 uid = Data_Manager::instance().get_currentUser();
    if (uid < 0) {
        SPDLOG_DEBUG("ensureTaskRegistered: гость (uid < 0) — регистрация не требуется");
        return -1;
    }
    const QString name = combo->currentText().trimmed();
    if (name.isEmpty()) {
        SPDLOG_DEBUG("ensureTaskRegistered: пустое имя задачи — выход");
        return -1;
    }
    SPDLOG_TRACE("ensureTaskRegistered: uid = {}, name = '{}'",
                 uid, name.toStdString());
    // поиск задачи с таким именем в БД
    const QList<Task_Info> all = Data_Manager::instance().getTasksForUser(uid);
    for (const auto& t : all) {
        if (t.task_name == name) {
            // если задача есть-привяка ID к текущему комбо
            int idx = combo->findText(name, Qt::MatchFixedString);
            if (idx >= 0) combo->setItemData(idx, t.task_id);
            SPDLOG_DEBUG("ensureTaskRegistered: найдена существующая задача '{}' (id = {})",
                         name.toStdString(), t.task_id);
            return t.task_id;
        }
    }
    // Если нет — создание новой
    Task_Info ti;
    ti.task_name = name;
    ti.user_id   = uid;
    const qint64 newId = Data_Manager::instance().sendNewTask(ti);
    if (newId <= 0) {
        SPDLOG_ERROR("ensureTaskRegistered: sendNewTask вернул {} для '{}'",
                     newId, name.toStdString());
        return -1;
    }
    SPDLOG_INFO("ensureTaskRegistered: создана новая задача '{}' (id = {})",
                name.toStdString(), newId);
    // сигнал комбобоксам, что появилась новая задача
    emit tasksChanged();
    return newId;
}

void BaseTracker::processCurrentBlock()
{
    // Если выход за пределы списка — завершение
    if (currentBlockIndex < 0 || currentBlockIndex >= timeBlocks.size()) {
        SPDLOG_INFO("processCurrentBlock: все блоки завершены (index = {})",
                    currentBlockIndex);
        if (blockTitleLabel) blockTitleLabel->setText(tr("Все блоки завершены"));
        if (blockRangeLabel) blockRangeLabel->setText(tr("- : -"));
        currentBlockIndex = -1;
        if (digitalClock2) {
            digitalClock2->timer_stop();
            digitalClock2->set_timer_time(0, 0, 0);
        }
        return;
    }
    TimeBlock& b = timeBlocks[currentBlockIndex];
    if (!isBreakTime) {
        // РАБОТА
        SPDLOG_INFO("processCurrentBlock: РАБОТА, блок {}/{}, '{}', {} мин",
                    currentBlockIndex + 1, timeBlocks.size(),
                    b.name.toStdString(), b.durationMin);
        isBreakTime = true;
        currentPhase = Phase::Work;
        // УСТАНАВЛИВАЕМ ID ЗАДАЧИ ДЛЯ СЕССИИ
        currentTaskId = b.taskId;
        if (blockTitleLabel)
            blockTitleLabel->setText(tr("Работа: %1").arg(b.name.isEmpty() ?
                                                              tr("Блок %1").arg(currentBlockIndex+1) :
                                                              b.name));
        if (blockRangeLabel)
            blockRangeLabel->setText(tr("%1 мин").arg(b.durationMin));
        if (digitalClock2) {
            digitalClock2->timer_stop();
            digitalClock2->setMode(FTR::timer);
            digitalClock2->set_timer_time(0, 0, 0);
            digitalClock2->timer_start(0, 0, b.durationMin * 60);
        }
        sessionStart = QDateTime::currentDateTime();
        if (blockTimer) blockTimer->start(b.durationMin * 60 * 1000);
    } else {
        // ПЕРЕРЫВ
        SPDLOG_INFO("processCurrentBlock: ПЕРЕРЫВ, блок {}/{}, '{}', {} мин",
                    currentBlockIndex + 1, timeBlocks.size(),
                    b.name.toStdString(), b.bufferMin);
        isBreakTime = false;
        currentPhase = Phase::Break;
        // Во время перерыва мы тоже привязываем сессию к этой же задаче
        currentTaskId = b.taskId;
        if (b.bufferMin > 0) {
            if (blockTitleLabel) blockTitleLabel->setText(tr("Перерыв"));
            if (blockRangeLabel) blockRangeLabel->setText(tr("%1 мин").arg(b.bufferMin));
            if (digitalClock2) {
                digitalClock2->timer_stop();
                digitalClock2->setMode(FTR::timer);
                digitalClock2->set_timer_time(0, 0, 0);
                digitalClock2->timer_start(0, 0, b.bufferMin * 60);
            }
            sessionStart = QDateTime::currentDateTime();
            if (blockTimer) blockTimer->start(b.bufferMin * 60 * 1000);
        } else {
            // Если перерыва нет, переход к следующему блоку
            SPDLOG_DEBUG("processCurrentBlock: bufferMin = 0 — сразу к следующему блоку");
            currentBlockIndex++;
            QTimer::singleShot(0, this, &BaseTracker::processCurrentBlock);
        }
    }
}


void BaseTracker::startSequence()
{
    SPDLOG_INFO("startSequence: запуск последовательности блоков");
    if (digitalClock2) {
        disconnect(digitalClock2, &digital_clock::timerFinished, this, nullptr);
    }
    syncTimeBlocks();
    // Удаление из списка блоков, у которых виджеты были удалены
    for (int i = timeBlocks.size() - 1; i >= 0; --i) {
        if (!timeBlocks[i].rowWidget || !timeBlocks[i].taskCombo) {
            SPDLOG_DEBUG("startSequence: удаление невалидного блока idx = {}", i);
            timeBlocks.removeAt(i);
        }
    }
    if (timeBlocks.isEmpty()) {
        SPDLOG_WARN("startSequence: нет добавленных блоков");
        QMessageBox::information(this, "Ошибка", "Нет добавленных блоков");
        return;
    }
    currentBlockIndex = 0;
    isBreakTime = false;
    processCurrentBlock();
}

void BaseTracker::onBlockTimerFinished()
{
    SPDLOG_INFO("onBlockTimerFinished: таймер блока истёк (isBreakTime = {})",
                isBreakTime);
    finishCurrentSession(true);
    playNotificationSound();
    if (!isBreakTime) {
        // Значит закончился перерыв. Переход к следующему блоку.
        currentBlockIndex++;
    }
    // Если isBreakTime == true, значит закончилась работа, и processCurrentBlock запустит перерыв,
    //не увеличивая индекс
    processCurrentBlock();
}

unsigned BaseTracker::parseDurationMin(const QString& text)
{
    QString s = text.trimmed();
    if (s.isEmpty()) return 0;
    if (s.contains(':')) {
        const QStringList parts = s.split(':');
        if (parts.size() != 2) return 0;
        bool okH = false, okM = false;
        int h = parts[0].toInt(&okH);
        int m = parts[1].toInt(&okM);
        if (!okH || !okM || h < 0 || m < 0 || m >= 60) return 0;
        return unsigned(h * 60 + m);
    }
    bool ok = false;
    int v = s.toInt(&ok);
    if (!ok || v < 0) return 0;
    return unsigned(v);
}

void BaseTracker::stopFlowClockAndSession()
{
    SPDLOG_DEBUG("stopFlowClockAndSession: закрытие сессии и остановка таймера");
    //Закрытие текущей сессии до сброса currentTaskId
    finishCurrentSession(false);
    // Невидимый таймер последовательности (EatTheFrog)
    if (taskTimer && taskTimer->isActive()) {
        taskTimer->stop();
    }
    // Визуальный таймер/секундомер
    if (digitalClock2) {
        if (digitalClock2->getMode() == FTR::stopwatch) {
            digitalClock2->stopwatch_stop();   // остановить секундомер
        } else {
            digitalClock2->timer_stop();       // остановить таймер
        }
        digitalClock2->set_timer_time(0, 0, 0); // визуально 00:00:00
    }
    // Сброс привязки к задаче - сессия закрыта
    currentTaskId = -1;
    sessionStart  = QDateTime();
    SPDLOG_TRACE("stopFlowClockAndSession: готово");
}

void BaseTracker::collectTasksFromRows()
{
    SPDLOG_TRACE("collectTasksFromRows: сбор задач из строк");
    if (todolist) {
        delete todolist;
        todolist = nullptr;
    }
    todolist = new QList<todo>();
    if (!rowsLayout) return;
    for (int i = 0; i < rowsLayout->count(); ++i) {
        QHBoxLayout* row = qobject_cast<QHBoxLayout*>(rowsLayout->itemAt(i)->layout());
        if (!row) continue;
        // Сбор виджетов строки в порядке добавления
        QList<QWidget*> w;
        for (int j = 0; j < row->count(); ++j) {
            if (QWidget* ww = row->itemAt(j)->widget())
                w.append(ww);
        }
        // Ожидание минимума: taskCombo, timeLine, done, startBtn, removeBtn
        if (w.size() < 5) continue;
        // Последние 5 виджетов строки идут в фиксированном порядке:
        // [taskCombo, timeEdit, done, startBtn, removeBtn]
        auto* taskCombo = qobject_cast<QComboBox*>(w.at(w.size() - 5));
        auto* timeEdit  = qobject_cast<QLineEdit*>(w.at(w.size() - 4));
        auto* doneChk   = qobject_cast<QCheckBox*>(w.at(w.size() - 3));
        if (!taskCombo) continue;
        const QString name = taskCombo->currentText().trimmed();
        if (name.isEmpty()) continue;
        // ЕДИНАЯ ТОЧКА РЕГИСТРАЦИИ
        // Возвращает существующий id либо создаёт задачу в БД
        // эмитит tasksChanged()-ensureTaskRegistered
        const qint64 taskId = ensureTaskRegistered(taskCombo);
        if (taskId <= 0) {
            SPDLOG_WARN("collectTasksFromRows: пропуск '{}' (taskId <= 0)",
                        name.toStdString());
            continue;   // нет пользователя / пустое имя — пропуск
        }
        // Приоритет (только для EatTheFrog)
        int priority = 1; // по умолчанию «Элементарно»
        for (QWidget* ww : w) {
            if (auto* cb = qobject_cast<QComboBox*>(ww)) {
                if (cb != taskCombo) {
                    priority = cb->currentData().toInt();
                    break;
                }
            }
        }
        todo t;
        t.task.task_name = name;
        t.task.task_id = taskId;
        t.frog = static_cast<EatTheFrog_dificulties>(priority);
        t.duration = timeEdit ? parseDurationMin(timeEdit->text()) : 0u;
        t.done = doneChk ? doneChk->isChecked() : false;
        todolist->append(t);
        SPDLOG_DEBUG("collectTasksFromRows: '{}' id = {}, prio = {}, dur = {}",
                     name.toStdString(), taskId, priority, t.duration);
    }
    SPDLOG_TRACE("collectTasksFromRows: собрано задач = {}",
                 todolist ? todolist->size() : 0);
}

void BaseTracker::refreshTaskCombo(QComboBox* combo) {
    if (!combo) {
        SPDLOG_WARN("refreshTaskCombo: combo == nullptr — выход");
        return;
    }
    const qint64 uid = Data_Manager::instance().get_currentUser();
    const QString current = combo->currentText();
    SPDLOG_TRACE("refreshTaskCombo: uid = {}, текущий текст = '{}'",
                 uid, current.toStdString());

    QSignalBlocker block(combo);
    combo->clear();

    int added = 0;
    if (uid >= 0) {
        for (const auto& t : Data_Manager::instance().getTasksForUser(uid)) {
            combo->addItem(t.task_name, t.task_id);
            ++added;
        }
    } else {
        SPDLOG_DEBUG("refreshTaskCombo: гость (uid < 0) — список задач пуст");
    }

    if (!current.isEmpty()) {
        int idx = combo->findText(current, Qt::MatchFixedString);
        if (idx >= 0) combo->setCurrentIndex(idx);
        else          combo->setCurrentText(current);
    }
    SPDLOG_TRACE("refreshTaskCombo: добавлено задач = {}, итоговый текст = '{}'",
                 added, combo->currentText().toStdString());
}

void BaseTracker::sortTasksByPriority()
{
    if (!todolist) return;
    std::stable_sort(todolist->begin(), todolist->end(),
                     [](const todo& a, const todo& b) {
                         return a.frog > b.frog;
                     });
    SPDLOG_TRACE("sortTasksByPriority: отсортировано {}", todolist->size());
}

void BaseTracker::startTaskSequence(int startIndex)
{
    SPDLOG_INFO("startTaskSequence: старт последовательности");
    stopTaskSequence();
    collectTasksFromRows();
    sortTasksByPriority();
    if (!todolist || todolist->isEmpty()) {
        SPDLOG_WARN("startTaskSequence: список задач пуст");
        if (sequenceStatusLabel)
            sequenceStatusLabel->setText(tr("Список задач пуст"));
        return;
    }
    SPDLOG_DEBUG("startTaskSequence: задач = {}", todolist->size());
    if (!taskTimer) {
        taskTimer = new QTimer(this);
        taskTimer->setSingleShot(true);
        connect(taskTimer, &QTimer::timeout, this,
                [this]{ completeCurrentTask(true); });
    }

    // поиск первой невыполненной задачи
    currentTaskIndex = -1;
    for (int i = 0; i < todolist->size(); ++i) {
        if (!(*todolist)[i].done) {
            currentTaskIndex = i;
            break;
        }
    }
    // Если все задачи уже сделаны - сообщение
    if (currentTaskIndex == -1) {
        SPDLOG_INFO("startTaskSequence: все задачи уже выполнены");
        if (sequenceStatusLabel)
            sequenceStatusLabel->setText(tr("Все задачи выполнены ✔"));
        return;
    }
    startNextTask();
}

void BaseTracker::startNextTask()
{
    if (!todolist || currentTaskIndex < 0
        || currentTaskIndex >= todolist->size()) {
        SPDLOG_INFO("startNextTask: последовательность завершена");
        if (sequenceStatusLabel)
            sequenceStatusLabel->setText(tr("Все задачи выполнены ✔"));
        if (taskTimer && taskTimer->isActive()) taskTimer->stop();

        // Останавливаем и сбрасываем визуальный таймер в ноль
        if (digitalClock2) {
            digitalClock2->timer_stop();
            digitalClock2->set_timer_time(0, 0, 0);
        }
        currentTaskIndex = -1;
        return;
    }
    const todo& t = (*todolist)[currentTaskIndex];
    SPDLOG_INFO("startNextTask: задача {}/{}, '{}', {} мин",
                currentTaskIndex + 1, todolist->size(),
                t.task.task_name.toStdString(), t.duration);
    if (sequenceStatusLabel) {
        sequenceStatusLabel->setText(
            tr("Задача %1 из %2: %3 (%4 мин)")
                .arg(currentTaskIndex + 1)
                .arg(todolist->size())
                .arg(t.task.task_name)
                .arg(t.duration));
    }
    // Сессия в БД
    finishCurrentSession(false);
    Task_Info ti;
    Data_Manager& db = Data_Manager::instance();
    ti.task_name = t.task.task_name;
    ti.user_id   = db.get_currentUser();
    currentTaskId = db.sendNewTask(ti);
    sessionStart  = QDateTime::currentDateTime();
    currentPhase  = Phase::Work;
    // ЖЕСТКИЙ СБРОС И СТАРТ ВИЗУАЛЬНОГО ТАЙМЕРА
    digital_clock* c = digitalClock2 ? digitalClock2 : digitalClock;
    if (c) {
        c->timer_stop(); // Остановка
        c->setMode(FTR::timer); // режим таймера
        c->set_timer_time(0, 0, 0); // сброс в 00:00:00
        c->reset_all_time();// сброс внутренних переменных
        if (t.duration > 0) {//Запуск нового времени (костыль: минуты в секунды)
            c->timer_start(0, 0, int(t.duration * 60));
        }
    }
    //Сброс и старт невидимого таймера
    if (taskTimer) {
        if (taskTimer->isActive()) taskTimer->stop();
        if (t.duration > 0) {
            taskTimer->start(int(t.duration) * 60 * 1000);
        }
    }
}

void BaseTracker::completeCurrentTask(bool done)
{
    if (!todolist || currentTaskIndex < 0
        || currentTaskIndex >= todolist->size())
        return;
    todo& t = (*todolist)[currentTaskIndex];
    t.done = done;
    SPDLOG_INFO("completeCurrentTask: idx = {}, done = {}", currentTaskIndex, done);
    finishCurrentSession(done);
    playNotificationSound();
    if (taskTimer && taskTimer->isActive()) taskTimer->stop();
    ++currentTaskIndex;
    startNextTask();
}

void BaseTracker::stopTaskSequence()
{
    SPDLOG_DEBUG("stopTaskSequence: остановка последовательности");
    stopFlowClockAndSession();
    currentTaskIndex = -1;
    if (sequenceStatusLabel)
        sequenceStatusLabel->setText(tr("Последовательность остановлена"));
}

void BaseTracker::sortFlowRows(QVBoxLayout* rowsLayout)
{
    if (!rowsLayout) return;
    QList<QHBoxLayout*> rows;
    for (int i = 0; i < rowsLayout->count(); ++i) {
        if (auto* h = qobject_cast<QHBoxLayout*>(rowsLayout->itemAt(i)->layout()))
            rows.append(h);
    }
    auto prioOf = [](QHBoxLayout* row) -> int {
        for (int j = 0; j < row->count(); ++j) {
            if (auto* cb = qobject_cast<QComboBox*>(row->itemAt(j)->widget()))
                return cb->currentData().toInt();
        }
        return 1;
    };
    std::stable_sort(rows.begin(), rows.end(),
                     [&](QHBoxLayout* a, QHBoxLayout* b){
                         return prioOf(a) > prioOf(b);
                     });
    // Перестановка в layout
    while (rowsLayout->count() > 0)
        rowsLayout->takeAt(0);
    for (auto* r : rows)
        rowsLayout->addLayout(r);
    rowsLayout->addStretch();
    SPDLOG_TRACE("sortFlowRows: отсортировано строк = {}", rows.size());
}

void BaseTracker::pauseTaskSequence()
{
    SPDLOG_DEBUG("pauseTaskSequence: пауза последовательности");
    if (taskTimer && taskTimer->isActive()) {
        taskTimer->stop();
    }
    if (digitalClock2) {
        digitalClock2->timer_pause(); // останавливает визуальный таймер
    }
    finishCurrentSession(false);
    if (sequenceStatusLabel)
        sequenceStatusLabel->setText(tr("Пауза"));
}

void BaseTracker::resumeTaskSequence()
{
    if (currentTaskIndex < 0 || !todolist || currentTaskIndex >= todolist->size()) {
        SPDLOG_DEBUG("resumeTaskSequence: последовательность не запущена — старт заново");
        startTaskSequence(); // сначала, если не запущено
        return;
    }
    const todo& t = (*todolist)[currentTaskIndex];
    SPDLOG_DEBUG("resumeTaskSequence: продолжение задачи idx = {}, dur = {}",
                 currentTaskIndex, t.duration);
    // Визуальный таймер
    if (digitalClock2) {
        digitalClock2->timer_start(0, 0, 0);
    }
    // Невидимый таймер
    if (taskTimer && t.duration > 0) {
        taskTimer->start(int(t.duration) * 60 * 1000);
    }
    sessionStart = QDateTime::currentDateTime();
    if (sequenceStatusLabel)
        sequenceStatusLabel->setText(tr("Задача %1 из %2 (продолжение)").arg(currentTaskIndex + 1)
                                         .arg(todolist->size()));
}

void BaseTracker::updateClockEditable()
{
    if (!digitalClock2) return;
    if (mode == FTR::EatTheFrog) {
        digitalClock2->setEditable(false);
        return;
    }
    if (mode == FTR::FlowMode) {
        bool hasActiveTask = (activeFlowIndex >= 0
                              && activeFlowIndex < flowTasksList.size());
        digitalClock2->setEditable(hasActiveTask);
        qDebug() << "updateClockEditable: FlowMode, hasActiveTask =" << hasActiveTask
                 << " activeFlowIndex =" << activeFlowIndex
                 << " size =" << flowTasksList.size();
        return;
    }
    digitalClock2->setEditable(false);
}

bool BaseTracker::eventFilter(QObject *obj, QEvent *event)
{
    if (mode != FTR::FlowMode) {
        return QWidget::eventFilter(obj, event);
    }
    if (event->type() == QEvent::MouseButtonPress) {
        // Если клик на строку с задачей
        QVariant idVar = obj->property("taskIndex");
        if (idVar.isValid()) {
            SPDLOG_TRACE("eventFilter: клик по строке taskIndex = {}",
                         idVar.toInt());
            selectFlowTask(idVar.toInt());
        }
        // Если клик на любой контейнер (пустое место)
        else if (obj->isWidgetType()) {
            QWidget* w = qobject_cast<QWidget*>(obj);
            if (!qobject_cast<QLineEdit*>(w) &&
                !qobject_cast<QPushButton*>(w) &&
                !qobject_cast<QToolButton*>(w) &&
                !qobject_cast<QComboBox*>(w) &&
                !qobject_cast<QCheckBox*>(w))
            {
                SPDLOG_TRACE("eventFilter: клик по пустому месту — снятие выделения");
                // Снятие выделения
                activeFlowIndex = -1;
                updateClockEditable();
                // Возврат обычного стиля
                QString normalComboStyle =
                    "QComboBox { color: black; font-weight: bold; background-color: "
                    "rgba(255,255,255,0.5); "
                    "border: 1px solid rgba(255,255,255,0.4); "
                    "border-radius: 4px; padding: 2px; }"
                    "QComboBox:hover { background-color: rgba(255,0,0,0.3); }"
                    "QComboBox:focus { background-color: rgba(255, 0, 0, 0.5); color: black; "
                    "border: 1px solid rgba(255,255,255,0.8); }"
                    "QComboBox QLineEdit { background: transparent; border: none; color: black; }";
                QString normalLineStyle =
                    "QLineEdit { color: black; font-weight: bold; "
                    "background-color: rgba(255,255,255,0.5); "
                    "border: 1px solid rgba(255,255,255,0.4); "
                    "border-radius: 4px; padding: 2px; }"
                    "QLineEdit:hover { background-color: rgba(255,0,0,0.3); }"
                    "QLineEdit:focus { background-color: rgba(255, 0, 0, 0.5); color: black; "
                    "border: 1px solid rgba(255,255,255,0.8); }";
                for (auto& ft : flowTasksList) {
                    if (ft.nameEdit) ft.nameEdit->setStyleSheet(normalComboStyle);
                    if (ft.timeEdit) ft.timeEdit->setStyleSheet(normalLineStyle);
                }
                // снятие фокуса с любого виджета
                if (QWidget* focused = QApplication::focusWidget()) {
                    focused->clearFocus();
                }
                // Возврат фокуса самому окну, чтобы клики по пустому месту работали
                this->setFocus();
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

void BaseTracker::selectFlowTask(int index)
{
    if (index < 0 || index >= flowTasksList.size()) {
        SPDLOG_DEBUG("selectFlowTask: некорректный index = {}", index);
        return;
    }
    if (!flowTasksList[index].nameEdit || !flowTasksList[index].timeEdit) return;
    SPDLOG_TRACE("selectFlowTask: выбран index = {}", index);
    // СТИЛИ ДЛЯ КОМБОБОКСА
    QString normalComboStyle =
        "QComboBox { color: black; font-weight: bold; background-color: rgba(255,255,255,0.5); "
        "border: 1px solid rgba(255,255,255,0.4); border-radius: 4px; "
        "padding: 2px; }"
        "QComboBox:hover { background-color: rgba(255,0,0,0.3); }"
        "QComboBox:focus { background-color: rgba(255, 0, 0, 0.5); "
        "color: black; border: 1px solid rgba(255,255,255,0.8); }"
        "QComboBox QLineEdit { background: transparent; border: none; color: black; }";
    QString activeComboStyle =
        "QComboBox { background-color: rgba(255, 0, 0, 0.5); color: black; border: 1px solid white; }"
        "QComboBox:focus { background-color: rgba(255, 0, 0, 0.7); color: black;"
        " border: 1px solid white; }"
        "QComboBox QLineEdit { background: transparent; border: none; color: black; }";
    // СТИЛИ ДЛЯ QLINEEDIT (Время)
    QString normalLineStyle =
        "QLineEdit { color: black; font-weight: bold; background-color: rgba(255,255,255,0.5); "
        "border: 1px solid rgba(255,255,255,0.4); border-radius: 4px; "
        "padding: 2px; }"
        "QLineEdit:hover { background-color: rgba(255,0,0,0.3); }"
        "QLineEdit:focus { background-color: rgba(255, 0, 0, 0.5); "
        "color: black; border: 1px solid rgba(255,255,255,0.8); }";
    QString activeLineStyle =
        "QLineEdit { background-color: rgba(255, 0, 0, 0.5); color: black; border: 1px solid white; }"
        "QLineEdit:focus { background-color: rgba(255, 0, 0, 0.7); color: black; "
        "border: 1px solid white; }";
    // Снятие выделения со всех строк
    for (auto& ft : flowTasksList) {
        if (ft.nameEdit) ft.nameEdit->setStyleSheet(normalComboStyle);
        if (ft.timeEdit) ft.timeEdit->setStyleSheet(normalLineStyle);
    }
    // новый активный индекс
    activeFlowIndex = index;
    // Подсветка красным
    FlowTask& ft = flowTasksList[index];
    ft.nameEdit->setStyleSheet(activeComboStyle);
    ft.timeEdit->setStyleSheet(activeLineStyle);
    // Синхронизация таймера
    if (digitalClock2 && flowTabs && flowTabs->currentIndex() == 1
        && !digitalClock2->is_running()) {
        int duration = parseDurationMin(ft.timeEdit->text());
        digitalClock2->blockSignals(true);
        digitalClock2->set_timer_time(0, duration, 0);
        digitalClock2->blockSignals(false);
    }
    updateClockEditable();
}

void BaseTracker::on_guest_link_activated(const QString &link)
{
    if (link == "#open") {
        SPDLOG_TRACE("on_guest_link_activated: открытие профиля");
        // открыть профиль (логика из EatTheFrog::on_label_4_linkActivated)
        if (!profileWindow) {
            profileWindow = new profileW();
            profileWindow->setOpenWindow(this);
            profileWindow->setAttribute(Qt::WA_DeleteOnClose);
            connect(profileWindow, &profileW::profileUpdated,
                    this, [this]() { loadUserProfile(); loadCurrentUser(); });
            connect(profileWindow, &QObject::destroyed,
                    this, [this]() { profileWindow = nullptr; });
        }
        profileWindow->show();
        profileWindow->raise();
        profileWindow->activateWindow();
        this->hide();
        return;
    }
    if (link != "#guest") return;
    SPDLOG_TRACE("on_guest_link_activated: открытие окна авторизации");
    if (!loginDialog) {
        loginDialog = new QDialog(this);
        loginDialog->setWindowTitle(tr("Окно авторизации"));
        loginDialog->setStyleSheet(R"(
            QDialog { background-color: rgba(0, 0, 0, 0.5); }
            QLabel { color: white; font-size: 14px; font-weight: bold; padding: 8px 0; }
            QPushButton { background-color: rgba(0, 0, 0, 0.5); color: white; font-weight: bold;
                          border: 1px solid #555; border-radius: 6px; padding: 8px 16px;
                          font-size: 14px; min-width: 140px; }
            QPushButton:hover { background-color: rgba(255, 0, 0, 0.3); border-color: #6a9fb5; }
            QPushButton:pressed { background-color: rgba(255, 0, 0, 0.8); }
        )");
        auto* layout = new QVBoxLayout(loginDialog);
        layout->addWidget(new QLabel(tr("Выберите действие:"), loginDialog));
        QPushButton* regBtn  = new QPushButton(tr("Регистрация"), loginDialog);
        QPushButton* authBtn = new QPushButton(tr("Авторизация"), loginDialog);
        QPushButton* cancel  = new QPushButton(tr("Отмена"), loginDialog);
        layout->addWidget(regBtn);
        layout->addWidget(authBtn);
        layout->addWidget(cancel);
        connect(regBtn,  &QPushButton::clicked, this, [this]() { loginDialog->done(1); });
        connect(authBtn, &QPushButton::clicked, this, [this]() { loginDialog->done(2); });
        connect(cancel,  &QPushButton::clicked, this, [this]() { loginDialog->done(0); });
        connect(loginDialog, &QObject::destroyed,
                this, [this]() { loginDialog = nullptr; });
    }
    int choice = loginDialog->exec();
    if (choice == 1) {
        SPDLOG_INFO("on_guest_link_activated: выбор — Регистрация");
        // Регистрация
        auto* registration = new NEW_USER_FORM();
        registration->setAttribute(Qt::WA_DeleteOnClose);
        registration->setOpenWindow(this);
        registration->show();
        this->hide();
    }
    else if (choice == 2) {
        SPDLOG_INFO("on_guest_link_activated: выбор — Авторизация");
        // Авторизация
        auto* autorisation = new USERS_FORM();
        autorisation->setAttribute(Qt::WA_DeleteOnClose);
        autorisation->setOpenWindow(this);
        autorisation->show();
        this->hide();
    }
}
