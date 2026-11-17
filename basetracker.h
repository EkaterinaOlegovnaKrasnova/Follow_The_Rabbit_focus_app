#ifndef BASETRACKER_H
#define BASETRACKER_H

#include "digital_clock.h"
#include "information.h"
#include "myclock.h"
#include "profilew.h"
#include "qboxlayout.h"
#include "qcheckbox.h"
#include "qcombobox.h"
#include "qevent.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "qpushbutton.h"
#include "qscrollarea.h"
#include "qspinbox.h"
#include "qstackedwidget.h"
#include "qtmetamacros.h"
#include "qtoolbutton.h"
#include <QSqlDatabase>
#include <QDebug>
#include <QComboBox>
#include <QApplication>


//ДЛЯ EATTHEFROG
using PriorityWidgetBuilder = std::function<QWidget*(QWidget* parent)>; // виджета приоритета
struct todo{
    FTR::EatTheFrog_dificulties frog;// Приоритет задачи (1..5)
    Task_Info task;// Данные задачи (id, имя)
    unsigned duration;// Время выполнения (в минутах)
    bool done;// Выполнена ли задача
};

//ДЛЯ TIMEBLOKING
struct TimeBlock {
    QString name;// Название блока
    int durationMin = 0;// Длительность работы (мин)
    int bufferMin = 0;// Длительность перерыва (мин)
    QWidget* rowWidget = nullptr;// Виджет строки (для удаления/поиска индекса)
    QComboBox* taskCombo = nullptr;// Комбобокс выбора задачи
    QSpinBox* durSpin = nullptr;// Счетчик времени работы
    QSpinBox* bufSpin = nullptr;// Счетчик времени отдыха
    qint64 taskId = -1;// ID задачи в БД
};

//ДЛЯ FLOW MODE
struct FlowTask {
     QComboBox* nameEdit = nullptr;// Комбобокс названия задачи
    QLineEdit* timeEdit = nullptr;// Поле ввода времени (мин)
    QCheckBox* doneCheck = nullptr; // Чекбокс выполнено
    QComboBox* priorityCombo = nullptr;// Комбобокс приоритета (только для EatTheFrog)
    QPushButton* startBtn = nullptr;// Кнопка запуск
};

class BaseTracker:public QWidget
{
    Q_OBJECT
public:
    BaseTracker(QWidget *parent,FTR::TimerMode mode);
    virtual ~BaseTracker();
    void ensureButtonsCreated();// Установка кнопок (Старт/Пауза/Стоп), если их еще нет
    void setOpenWindow(QWidget* mainWindow){openWindow = mainWindow;}//настройка указателя на главное окно
    void playNotificationSound();//нотификация при переключении режима работы и отдыха
    void showEvent(QShowEvent *event)override;//обновление  окрана в зависимости от активности юзера
    void hideEvent(QHideEvent* e) override;//отрабатывает при вызове hide()
    void loadUserProfile();//загрузка фото профиля
    void loadCurrentUser();//загрузка профиля юзера
    virtual QString get_info_text() const = 0;//получить текст информации
    virtual void set_info_text(const QString& text) = 0;//настройка текста информации
    void finishCurrentSession(bool is_completed);//запись окончания сессии и отправка в бд
public slots:
    void on_label_7_linkActivated(const QString &link);//нажатие на ссылку информации
protected slots:
    void on_pushButton_clicked( );//нажатие на кнопку назад
    void on_pushButton_2_clicked();//выбор режима секундомер
    void on_pushButton_3_clicked();//выбор режима таймер
    void add_Task(const QString &arg1);//слот инициализации активности
    void loadTasksFromDb();//загрузка активности из бд
protected:
    bool applyClockMode(FTR::ClockMode clockMode);//Переключить режим часов (только Flow/TimeBlocking/EatTheFrog)
    void nextPhase();//логика переключения фаз работы и отдыха
    void hide_combo();//сокрытие комбобокса в зависимости от режима юзера
    void add_mode_interface(QVBoxLayout* layout);//отрисовка интерфейса в зависимости от модуля
    void setupButtons(QHBoxLayout *buttonlayout);//создание кнопок для управления часами

    // Помодоро-интерфейс_______________________________________________________
    void addTaskListInterface(QVBoxLayout* layout, int workMin, int breakMin);//добавление интерейса
    void createTaskScroll(QVBoxLayout* tasksLayout);// Скролл + контейнер строк
    void createTaskRow(QLineEdit*&   outName,// Одна строка задачи: только UI
                       QLineEdit*&   outCount,
                       QCheckBox*&   outDone,
                       QPushButton*& outStart);
    QLineEdit* addTaskRow();// Одна строка задачи + подключение кнопки ▶
    void startPomodoro(digital_clock* clock, int count);//подключение режима
    QLabel* p_activeTaskLabel    = nullptr;  // этап модуля (Работа/Отдых)
    QLabel* p_activeActivityLabel = nullptr; // выбранная активность (из combo)
    QLabel* p_activeSubtaskLabel  = nullptr; // подзадача (строка ▶)

    //Флоуинтерейс для eat the frog и флоу_____________________________________
    void addFlowModeInterface(QVBoxLayout* layout,//добавление интерейса
                              PriorityWidgetBuilder priorityBuilder = nullptr);
    void createFlowTaskRow(QVBoxLayout* rowsLayout,//создание строки
                           QWidget* container,
                           const QString& text = "",
                           PriorityWidgetBuilder priorityFactory = nullptr,
                           bool isStopwatch = false);
    QWidget* buildFlowTabPage(FTR::ClockMode clockMode,//создание страницы таймер или секудомер
                              PriorityWidgetBuilder priorityFactory = nullptr);
    void sortFlowRows(QVBoxLayout* rowsLayout);//сортировка для лягушки
    static QWidget* buildPriorityCombo(QWidget* parent);// Отдельная функция, которая создает комбобокс
    //Eat the Frog: последовательное выполнение списка
    void collectTasksFromRows();// Собрать все задачи из строк в todolist
    void sortTasksByPriority(); // Отсортировать todolist по приоритету
    void startTaskSequence(int startIndex=0);// Начать последовательное выполнение
    void startNextTask();// Запустить следующую задачу из списка
    void completeCurrentTask(bool done);// Завершить текущую задачу и перейти к следующей
    void stopTaskSequence();// Остановить всю последовательность
    static unsigned parseDurationMin(const QString& text);// Парсинг в минуты
    void stopFlowClockAndSession();// Остановить часы и закрыть сессию (для Flow/EatTheFrog)
    void pauseTaskSequence();// Пауза для EatTheFrog (останавливает taskTimer и визуальный таймер)
    void resumeTaskSequence();// Продолжение для EatTheFrog
protected:
    QTabWidget* flowTabs = nullptr;// вкладки для Flow/TimeBlocking
    QWidget* stopwatchPage = nullptr;// страница секундомера
    QWidget* timerPage     = nullptr;// страница таймера
    QList<todo>* todolist = nullptr;// Список задач для EatTheFrog
    int currentTaskIndex = -1;// Индекс текущей задачи в todolist
    QTimer* taskTimer = nullptr;// Невидимый таймер для EatTheFrog
    QLabel* sequenceStatusLabel = nullptr; // Лейбл "Задача 1 из 5: ..."
    QList<TimeBlock> timeBlocks;// Список блоков для TimeBlocking
    int currentBlockIndex = -1; // какой блок сейчас запущен
    QList<FlowTask> flowTasksList;// Модель данных
    int activeFlowIndex = -1;// Индекс выбранной задачи

    // Вспомогательные методы
    void selectFlowTask(int index);// Выделить задачу красным и синхронизировать таймер
    bool eventFilter(QObject *obj, QEvent *event) override;// Снятие выделения при клике на пустое место
    void updateClockEditable();// Разрешить/запретить редактирование часов
    void refreshTaskCombo(QComboBox* combo);//Оюновление комбобоксов
    void stopAllTimers();//остановка всех таймеров

    // TimeBlocking интерфейс
    void addTimeBlockingInterface(QVBoxLayout* layout);// Добавление интерфейса TimeBlocking
    QWidget* buildBlockRow(const QString& name, int durationMin,int bufferMin); // Создание строки блока
    void onBlockTimerFinished();// Обработчик окончания таймера блока
    void syncTimeBlocks();// Синхронизация данных из виджетов в модель timeBlocks
    void startSequence();// Запуск всей последовательности блоков
    void processCurrentBlock();// Обработка текущего блока (работа или перерыв)
    qint64 ensureTaskRegistered(QComboBox* combo);// Регистрация задачи в БД, если её там нет
    QTimer* blockTimer = nullptr;// Невидимый таймер для отсчета
    bool isBreakTime = false;// Флаг: сейчас перерыв или работа
    QWidget* blockRowsContainer = nullptr;// Контейнер для строк блоков
    QVBoxLayout* blockRowsLayout = nullptr;// Layout для строк блоков
    QLabel* blockTitleLabel = nullptr;// Лейбл "Работа: Задача" / "Перерыв"
    QLabel* blockRangeLabel = nullptr;// Лейбл "25 мин"
    QString currentBlockName;// Название текущего блока
    QFrame* blockFrame = nullptr;// Фрейм для интерфейса TimeBlocking
    virtual QLabel* avatarLabel() const = 0;// label под аватар (label_2)
    virtual QLabel* userLabel()   const = 0;// label под имя (label_3)
    virtual QLabel* get_info_link() const = 0;//геттер лэйбла для ссылки
    virtual QLabel* get_activity_label() const=0;//геттер лэйбла для комбобокса
    virtual QString backgroundImage() const = 0;//геттер изображения
    virtual QString windowTitleText() const = 0;//геттер названия модуля
    virtual QComboBox* activityComboBox() const = 0;//геттер комбобокса
    void connectInfoLink();//подключение ссылки на профиль
    void paintEvent(QPaintEvent *)override;//установка фона
    void getback_clicked();//метод возврата на предыдущее окно
    void placeClocksInLayout(MyClock* cl,digital_clock* clock,QVBoxLayout *layout,
                             QHBoxLayout *buttonlayout);//размещение часов в лэйауте
    void resizeEvent(QResizeEvent *event) override;// поведение стека с тасками при расширении экрана
    profileW* profileWindow = nullptr;//указатель на окно с профилем
    qint64 currentTaskId = -1; // id задачи, над которой работаем
    QDateTime sessionStart;// старт текущей фазы
    bool profileLinkConnected = false;//флаг подключения ссылки на профиль
    Phase currentPhase = Phase::Work;//текущая фаза
    MyClock *analogClock = nullptr;      // Аналоговые часы
    digital_clock *digitalClock = nullptr; // Электронные часы
    digital_clock *digitalClock2 = nullptr;//для 3х режимов
    QToolButton* startBtn = nullptr;// Кнопка старт
    QToolButton* pauseBtn = nullptr;// Кнопка пауза
    QToolButton* stopBtn  = nullptr;// Кнопка стоп
    QHBoxLayout *buttonLayout = nullptr;// Layout для кнопок
    QVBoxLayout *parent_buttonLayout = nullptr; // Родительский layout для кнопок
    QVBoxLayout *mode_layout = nullptr;// Layout для интерфейса
    QLabel *mode_label = nullptr; // Лейбл "Секундомер" / "Таймер"
    digital_clock* activeClock = nullptr;// Активные часы (для Pomodoro)
    QCheckBox* activeDoneCheck = nullptr;// Чекбокс текущей задачи (для Pomodoro)

    // Pomodoro-интерфейс
    QLabel* activeTaskLabel = nullptr;// Лейбл "Задача: ..."
    digital_clock* pomoClock = nullptr;// Часы для Pomodoro
    QStackedWidget* pomoStack = nullptr;// Стек страниц Pomodoro
    QWidget* tasksPage = nullptr;// Страница задач
    QWidget* pomodoroPage = nullptr;// Страница активного помидора
    Information* info = nullptr;//указатель на окно с информацией
    // Состояние помидора
    digital_clock*  clock = nullptr;// Часы основные
    int left  = 0;//сколько осталось фаз
    bool work  = true;//флаг работы
    // Скролл + строки задач
    QWidget* rowsContainer = nullptr;// Контейнер для строк задач
    QVBoxLayout* rowsLayout = nullptr;// Layout для строк задач
    QScrollArea* taskScroll = nullptr;// Скролл для задач
    // Параметры цикла
    int workMinutes = 25;// Длительность работы (мин)
    int breakMinutes = 5;// Длительность перерыва (мин)
signals:
    void tasksChanged();// Сигнал: список задач изменился (для синхронизации комбобоксов)
private:
    QWidget* openWindow = nullptr;//указатель на главное окно
    FTR::TimerMode mode = FTR::FlowMode;//режим таймера
    bool comboConnected = false;//флаг подключения комбобокса для выбора активности
    User_Profile me;// Профиль текущего пользователя
    QDialog* loginDialog = nullptr; // Диалог выбора (Регистрация/Авторизация/Отмена)
protected slots:
    void completeCurrentTaskWrapper() { completeCurrentTask(true); }// Обертка для QTimer
public slots:
    void on_guest_link_activated(const QString &link);// Слот: клик по ссылке "Гость"
};

#endif // BASETRACKER_H
