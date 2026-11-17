#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H
#include "myclock.h"
#include <QObject>
#include <QString>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QCryptographicHash>
#include <QStandardPaths>
#include <QDir>

//Структура с результатами аналитики
struct Analytics_Summary {
    QString favorite_technic;
    quint64 total_day_sec   = 0;   // за сегодня
    quint64 total_month_sec = 0;   // за текущий месяц
    quint64 total_year_sec  = 0;   // за текущий год
    double  completion_rate = 0.0; // 0..1  (completed / total)
    quint32 longest_session_sec = 0;
    quint32 avg_session_sec     = 0;
    std::pair<int,int>  work_break_ratio    = {0,0}; // work_sec / break_sec (0 если break=0)
    int  procrastination_index = 0; // 0..1
    int     total_sessions = 0;
};

//Структура профиля пользователя
struct User_Profile{
    qint64 user_id  = -1;
    QString Name;
    QString Middle_Name;
    QString Surname;
    QDate  DateBirth;
    QString phone_number;
    QString email;
    QString login;
    QString password;
    QDateTime created_at;
    QString salt;
    QString About;
    QByteArray Avatar;
};

// Структура задачи
enum class Phase {
    Work,
    Break
};

//Структура сессии (одного интервала)
struct Task_Info {
    qint64    task_id    = -1;
    QString   task_name;
    qint64    user_id    = -1;
    QDateTime created_at;
};

//Преобразование техники в строку для БД
struct Session_info {
    qint64      session_id  = -1;
    qint64      user_id     = -1;
    qint64      task_id     = -1;
    FTR::TimerMode technic = FTR::TimerMode::Pomodoro;
    Phase       phase = Phase::Work;
    QDateTime   start_time;
    QDateTime   finish_time;
    quint32     duration_sec = 0;
    bool        completed = false;
};

// Преобразование строки из БД в технику
inline QString technicToStr(FTR::TimerMode t) {
    switch (t) {
    case FTR::TimerMode::Pomodoro:return "pomodoro";
    case FTR::TimerMode::ReversePomodoro:return "reverse_pomodoro";
    case FTR::TimerMode::FlowMode:return "flowtime";
    case FTR::TimerMode::Mode60_10:return "mode_60_10";
    case FTR::TimerMode::Mode52_17:return "mode_52_17";
    case FTR::TimerMode::Mode90_20:return "mode_90_20";
    case FTR::TimerMode::TimeBlocking:return "time_blocking";
    case FTR::TimerMode::EatTheFrog:return "eat_the_frog";
    }
    return "unknown";
}

//Преобразование фазы в строку для БД
inline FTR::TimerMode technicFromStr(const QString& s) {
    if (s == "pomodoro")return FTR::TimerMode::Pomodoro;
    if (s == "reverse_pomodoro")return FTR::TimerMode::ReversePomodoro;
    if (s == "flowtime")return FTR::TimerMode::FlowMode;
    if (s == "mode_60_10")return FTR::TimerMode::Mode60_10;
    if (s == "mode_52_17")return FTR::TimerMode::Mode52_17;
    if (s == "mode_90_20")return FTR::TimerMode::Mode90_20;
    if (s == "eat_the_frog")return FTR::TimerMode::EatTheFrog;
    SPDLOG_WARN("technicFromStr: неизвестная техника '{}', "
                "использован TimeBlocking по умолчанию",
                s.toStdString());
    return FTR::TimerMode::TimeBlocking;
}

// Преобразование фазы в строку для БД
inline QString phaseToStr(Phase p) {return p == Phase::Work ? "work" : "break";}

// Преобразование строки из БД в фазу
inline Phase phaseFromStr(const QString& s) {
    const QString lower = s.trimmed().toLower();
    if (lower == "work")  return Phase::Work;
    if (lower == "break") return Phase::Break;
    SPDLOG_WARN("phaseFromStr: неизвестная фаза '{}', "
                "использован Break по умолчанию", s.toStdString());
    return Phase::Break;
}

// Класс-синглтон для работы с БД
class Data_Manager
{
public:
    static Data_Manager& instance();//подключение к базе синглтон
    Data_Manager(const Data_Manager&) = delete;// Запрет копирования
    Data_Manager& operator=(const Data_Manager&) = delete;// Запрет присваивания
    bool isOpen() const;//открыто ли БД
    bool open();//открытие БД
    void close();//закрытие БД
    // Пользователи__________________________________________________
    qint64 sendNewUserData(const User_Profile& new_user); // INSERT нового пользователя, возвращает user_id
    bool updateUserData(const User_Profile& user);// UPDATE данных профиля
    bool updateUserPassword(qint64 userId, const QString& rawPassword);// Смена пароля (генерирует новую соль и хеш)
    User_Profile getUserInfo(qint64 user_id); // SELECT профиля по user_id
    QList<User_Profile> getAllUsers();// SELECT * для формы USERS_FORM (сейчас не реализован)
    // проверка логин-пароля
    bool loginUser(const QString& login,const QString& password,User_Profile&  out_user);
    // Профиль текущего пользователя (с кешированием)
    User_Profile getCurrentUserProfile();
    //сеттер профиля
    void setCurrentUser(qint64 user_id) {
        currentUserId = user_id;
        currentUserLoaded = false;
        currentUser = User_Profile();
    }
    // Сессии______________________________________________________________
    qint64 sendSessionData(const Session_info& session);// INSERT новой сессии, возвращает session_id
    QList<Session_info> getSessionsForUser(qint64  user_id,// SELECT сессий за период
                                           const QDateTime& from,
                                           const QDateTime& to);
    qint64  get_currentUser() const { return currentUserId; }// Геттер ID текущего пользователя
    // Таски______________________________________________________________
    qint64 sendNewTask(const Task_Info& task);// INSERT новой задачи или получить существующую по имени
    QList<Task_Info> getTasksForUser(qint64 user_id);// SELECT всех задач пользователя
    // Аналитика__________________________________________________________
   Analytics_Summary getAnalyticsSummary(qint64 user_id, qint64 task_id);//сбор аналитики в одну структуру
    // отдельные методы, если понадобятся
    quint64 totalDurationForPeriod(qint64 user_id,// Сумма duration_sec за период (только work)
                                   qint64 task_id,//по конкретной задаче
                                   const QDateTime& from,
                                   const QDateTime& to);
    QString favoriteTechnic(qint64 user_id);// Самая часто используемая техника (work-сессии)
    double  completionRate(qint64 user_id);// Коэффициент завершённости (completed / total)
    quint32 longestSessionSec(qint64 user_id);// Самая длинная сессия (по duration_sec)
    quint32 averageSessionSec(qint64 user_id);// Средняя длина сессии (AVG(duration_sec))
    std::pair<int,int> workBreakRatio(qint64 user_id);// Соотношение работа/отдых в процентах
    int  procrastinationIndex(qint64 user_id);// Индекс прокрастинации (процент незавершённых)
private:
    Data_Manager(); // Приватный конструктор
    ~Data_Manager();// Приватный деструктор
    QSqlDatabase db;// Объект подключения к SQLite
    qint64 currentUserId = -1; // ID текущего пользователя (-1 = гость)
    bool createSchema(); // Создание таблиц (users, task, sessions) и индексов, если их нет
    void applyPragmas(); // Настройка SQLite (foreign_keys, journal_mode, synchronous)
    User_Profile currentUser;// Кеш профиля текущего пользователя
    bool currentUserLoaded = false;// Флаг: загружен ли профиль в кеш
};
#endif // DATA_MANAGER_H
