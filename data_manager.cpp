#include "data_manager.h"
#include "password_utils.h"
#include <QFile>

Data_Manager::Data_Manager() {
    if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
        SPDLOG_ERROR("Драйвер QSQLITE не найден! Приложение не сможет работать с БД.");
        return;
    }

    const QString dir = QStandardPaths::writableLocation(
        QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dir + QDir::separator() + "timetracker.db");

    if (!db.open()) {
        SPDLOG_ERROR("SQLite: не удалось открыть БД '{}': {}",
                     db.databaseName().toStdString(),
                     db.lastError().text().toStdString());
        return;
    }

    applyPragmas();

    if (!createSchema()) {
        SPDLOG_ERROR("SQLite: не удалось создать/проверить схему");
        db.close();
        return;
    }

    SPDLOG_INFO("SQLite: база открыта, файл = '{}'",
                db.databaseName().toStdString());
}

bool Data_Manager::loginUser(const QString& login,
                             const QString& password,
                             User_Profile&  out_user)
{
    if (!db.isOpen()) {
        SPDLOG_WARN("loginUser: база не открыта");
        return false;
    }

    QSqlQuery q(db);
    q.prepare("SELECT user_id, name, middle_name, surname, birth_date, "
              "       login, password_hash, salt, email, phone, created_at, "
              "       about, avatar "
              "FROM users WHERE login = :login");
    q.bindValue(":login", login);

    if (!q.exec()) {
        SPDLOG_ERROR("loginUser: ошибка SELECT: {}",q.lastError().text().toStdString());
        return false;
    }
    if (!q.next()) {
        SPDLOG_ERROR("loginUser: пользователь не найден, login = {} ", login.toStdString());
        return false;
    }

    const QString storedHash = q.value(6).toString();
    const QString salt       = q.value(7).toString();

    if (!verifyPassword(password, salt, storedHash)) {
        SPDLOG_ERROR( "loginUser: неверный пароль для {}",login.toStdString());
        return false;
    }

    out_user.user_id      = q.value(0).toLongLong();
    out_user.Name         = q.value(1).toString();
    out_user.Middle_Name  = q.value(2).toString();
    out_user.Surname      = q.value(3).toString();
    out_user.DateBirth    = q.value(4).toDate();
    out_user.login        = q.value(5).toString();
    out_user.password     = QString();
    out_user.salt         = salt;
    out_user.email        = q.value(8).toString();
    out_user.phone_number = q.value(9).toString();
    out_user.created_at   = q.value(10).toDateTime();
    out_user.About        = q.value(11).toString();
    out_user.Avatar       = q.value(12).toByteArray();

    currentUserId = out_user.user_id;
    currentUserLoaded = false;
    SPDLOG_INFO("loginUser: успешный вход, user_id = {}, login = '{}', "
                "avatar_bytes = {}",
                out_user.user_id,
                out_user.login.toStdString(),
                out_user.Avatar.size());
    return true;
}

User_Profile Data_Manager::getUserInfo(qint64 user_id)
{
    if (!db.isOpen()) {
        SPDLOG_WARN("getUserInfo: База не подключена");
        return User_Profile();
    }
    if (user_id < 0) {
        SPDLOG_WARN("getUserInfo: некорректный user_id = {}",user_id);
        return User_Profile();
    }

    QSqlQuery query(db);
    query.prepare("SELECT user_id, name, surname, created_at, "
                  "       login, password_hash, salt, email, phone, "
                  "       middle_name, about, avatar, birth_date "
                  "FROM users WHERE user_id = :user_id");
    query.bindValue(":user_id", user_id);

    if (!query.exec()) {
        SPDLOG_ERROR("SELECT users error: {}",query.lastError().text().toStdString());
        return User_Profile();
    }
    if (!query.next()) {
        SPDLOG_ERROR("Пользователь не найден, user_id = {}", user_id);
        return User_Profile();
    }

    User_Profile user;
    user.user_id       = query.value(0).toLongLong();
    user.Name          = query.value(1).toString();
    user.Surname       = query.value(2).toString();
    user.created_at    = query.value(3).toDateTime();
    user.login         = query.value(4).toString();
    user.password      = query.value(5).toString();
    user.salt          = query.value(6).toString();
    user.email         = query.value(7).toString();
    user.phone_number  = query.value(8).toString();
    user.Middle_Name   = query.value(9).toString();
    user.About         = query.value(10).toString();
    user.Avatar        = query.value(11).toByteArray();
    user.DateBirth     = query.value(12).toDate();

    SPDLOG_INFO("getUserInfo: данные получены, user_id = {}, login = '{}', "
                "avatar_bytes = {}",
                user.user_id,
                user.login.toStdString(),
                user.Avatar.size());
    return user;
}

 Data_Manager& Data_Manager::instance() {
    static Data_Manager dm;
    static bool firstCall = true;
     if (firstCall) {
         SPDLOG_INFO("Data_Manager: создан единственный экземпляр (Singleton)");
         firstCall = false;
     }
    return dm;
}
 Data_Manager::~Data_Manager() {
     close();
 }

 void Data_Manager::applyPragmas()
 {
     QSqlQuery q(db);
     q.exec("PRAGMA foreign_keys = ON");
     q.exec("PRAGMA journal_mode = WAL");
     q.exec("PRAGMA synchronous = NORMAL");
 }

 bool Data_Manager::createSchema()
 {
     QSqlQuery q(db);
     const QStringList ddl = {
         R"(CREATE TABLE IF NOT EXISTS users (
            user_id       INTEGER PRIMARY KEY AUTOINCREMENT,
            name          TEXT,
            surname       TEXT,
            middle_name   TEXT,
            birth_date    DATE,
            login         TEXT NOT NULL UNIQUE,
            password_hash TEXT NOT NULL,
            salt          TEXT NOT NULL,
            email         TEXT,
            phone         TEXT,
            created_at    DATETIME DEFAULT CURRENT_TIMESTAMP,
            about         TEXT,
            avatar        BLOB
        ))",
         R"(CREATE TABLE IF NOT EXISTS task (
            task_id    INTEGER PRIMARY KEY AUTOINCREMENT,
            task_name  TEXT NOT NULL,
            user_id    INTEGER NOT NULL,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            UNIQUE(user_id, task_name),
            FOREIGN KEY(user_id) REFERENCES users(user_id) ON DELETE CASCADE
        ))",
         R"(CREATE TABLE IF NOT EXISTS sessions (
            session_id   INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id      INTEGER NOT NULL,
            task_id      INTEGER,
            technic_name TEXT NOT NULL,
            phase        TEXT NOT NULL,
            start_time   DATETIME NOT NULL,
            finish_time  DATETIME,
            duration_sec INTEGER NOT NULL,
            completed    INTEGER NOT NULL DEFAULT 0,
            FOREIGN KEY(user_id) REFERENCES users(user_id) ON DELETE CASCADE,
            FOREIGN KEY(task_id) REFERENCES task(task_id) ON DELETE SET NULL
        ))",
         R"(CREATE INDEX IF NOT EXISTS idx_sessions_user_time
              ON sessions(user_id, start_time))",
         R"(CREATE INDEX IF NOT EXISTS idx_sessions_task
              ON sessions(task_id))"
     };

     for (const QString& sql : ddl) {
         if (!q.exec(sql)) {
             SPDLOG_ERROR("SQLite DDL error: {}",
                          q.lastError().text().toStdString());
             return false;
         }
     }
     return true;
 }

 void Data_Manager::close()
 {
     if (db.isOpen()) {
         SPDLOG_INFO("SQLite: закрываем базу");
         db.close();
     }
     const QString conn = db.connectionName();
     db = QSqlDatabase();
     if (!conn.isEmpty())
         QSqlDatabase::removeDatabase(conn);
     SPDLOG_INFO("SQLite: подключение удалено");
 }

 bool Data_Manager::open()
 {
     if (db.isOpen()) {
         SPDLOG_DEBUG("Data_Manager::open: база уже открыта");
         return true;
     }
     if (!db.open()) {
         SPDLOG_ERROR("Data_Manager::open: не удалось подключиться: {}",
                      db.lastError().text().toStdString());
         return false;
     }
     applyPragmas();
     SPDLOG_INFO("Data_Manager::open: база подключена успешно");
     return true;
 }

bool Data_Manager::isOpen() const{
    return db.isOpen();
}

qint64 Data_Manager::sendNewUserData(const User_Profile& u)
{
    SPDLOG_DEBUG("sendNewUserData: создание нового пользователя, login = '{}'",
                 u.login.toStdString());

    if (!db.isOpen()) {
        SPDLOG_ERROR("sendNewUserData: база не открыта");
        return -1;
    }

    QSqlQuery q(db);
    q.prepare("INSERT INTO users "
              "(name, surname, login, password_hash, salt, email, phone, birth_date, "
              " middle_name, about, avatar) "
              "VALUES (:name, :surname, :login, :ph, :salt, :email, :phone, :birth_date, "
              "        :middle_name, :about, :avatar)");
    q.bindValue(":name",        u.Name);
    q.bindValue(":surname",     u.Surname);
    q.bindValue(":login",       u.login);
    q.bindValue(":ph",          u.password);
    q.bindValue(":salt",        u.salt);
    q.bindValue(":email",       u.email);
    q.bindValue(":phone",       u.phone_number);
    q.bindValue(":birth_date",  u.DateBirth.isValid() ? QVariant(u.DateBirth) : QVariant());
    q.bindValue(":middle_name", u.Middle_Name);
    q.bindValue(":about",       u.About);
    q.bindValue(":avatar",      u.Avatar);

    if (!q.exec()) {
        SPDLOG_ERROR("sendNewUserData: ошибка INSERT: {}",
                     q.lastError().text().toStdString());
        return -1;
    }

    const qint64 newId = q.lastInsertId().toLongLong();
    if (newId <= 0) {
        SPDLOG_ERROR("sendNewUserData: INSERT прошёл, но lastInsertId не получен "
                     "(login = '{}')", u.login.toStdString());
        return -1;
    }

    SPDLOG_INFO("sendNewUserData: пользовательские данные созданы, user_id = {}, login = '{}', "
                "avatar_bytes = {}",
                newId,
                u.login.toStdString(),
                u.Avatar.size());

    return newId;
}


bool Data_Manager::updateUserPassword(qint64 userId, const QString& rawPassword)
{
    if (!db.isOpen() || userId < 0) {
        SPDLOG_ERROR("updateUserPassword: База не подключена или пользователя не существует");
        return false;}

    const QString salt = generateSalt();
    const QString hash = hashPassword(rawPassword, salt);

    QSqlQuery q(db);
    q.prepare("UPDATE users SET salt = :salt, password_hash = :hash "
              "WHERE user_id = :id");
    q.bindValue(":salt", salt);
    q.bindValue(":hash", hash);
    q.bindValue(":id",   userId);

    if (!q.exec()) {
        SPDLOG_ERROR("UPDATE password: {}",q.lastError().text().toStdString());
        return false;
    }
    if (q.numRowsAffected() == 0) {
        SPDLOG_WARN("updateUserPassword: пользователь user_id = {} не найден "
                    "(0 строк обновлено)", userId);
     return false;}
    if (q.numRowsAffected() < 0) {
        SPDLOG_WARN("updateUserPassword: numRowsAffected() = -1 "
                        "(драйвер не сообщает число строк), user_id = {}",
                        userId);}
    SPDLOG_INFO("updateUserPassword: Пароль успешно изменен");
    return true;
}

bool Data_Manager::updateUserData(const User_Profile& user)
{
    if (!db.isOpen() || user.user_id < 0) {
        SPDLOG_ERROR("updateUserData: База не подключена или пользователя не существует");
        return false;}

    QSqlQuery q(db);
    q.prepare("UPDATE users SET "
              " name = :name, surname = :surname, middle_name = :middle_name, "
              " email = :email, phone = :phone, "
              " about = :about, avatar = :avatar ,birth_date = :birth_date , login = :login "
              "WHERE user_id = :user_id");
    q.bindValue(":name",        user.Name);
    q.bindValue(":surname",     user.Surname);
    q.bindValue(":middle_name", user.Middle_Name);
    q.bindValue(":email",       user.email);
    q.bindValue(":phone",       user.phone_number);
    q.bindValue(":about",       user.About);
    q.bindValue(":avatar",      user.Avatar);
    q.bindValue(":user_id",     user.user_id);
    q.bindValue(":birth_date",  user.DateBirth.isValid() ? QVariant(user.DateBirth) : QVariant());
    q.bindValue(":login",       user.login);
    if (!q.exec()) {
        SPDLOG_ERROR("updateUserData: UPDATE users: {}",
                     q.lastError().text().toStdString());
        return false;
    }
    currentUserLoaded = false;
    SPDLOG_INFO("Данные успешно обновлены");
    return true;
}


qint64 Data_Manager::sendNewTask(const Task_Info& t)
{
    if (!db.isOpen() || t.user_id < 0) {
        SPDLOG_ERROR("sendNewTask: база не открыта или user_id < 0");
        return -1;
    }

    QSqlQuery q(db);
    q.prepare("INSERT INTO task (task_name, user_id, created_at) "
              "VALUES (:name, :uid, :created) "
              "ON CONFLICT(user_id, task_name) DO NOTHING");
    q.bindValue(":name",    t.task_name);
    q.bindValue(":uid",     t.user_id);
    q.bindValue(":created", t.created_at.isValid()
                                ? t.created_at
                                : QDateTime::currentDateTime());

    if (!q.exec()) {
        SPDLOG_ERROR("sendNewTask INSERT: {}",
                     q.lastError().text().toStdString());
        return -1;
    }

    QSqlQuery sel(db);
    sel.prepare("SELECT task_id FROM task "
                "WHERE user_id = :u AND task_name = :n");
    sel.bindValue(":u", t.user_id);
    sel.bindValue(":n", t.task_name);
    if (!sel.exec() || !sel.next()) {
        SPDLOG_ERROR("sendNewTask SELECT: {}",
                     sel.lastError().text().toStdString());
        return -1;
    }
    const qint64 id = sel.value(0).toLongLong();
    SPDLOG_INFO("sendNewTask: task_id = {}", id);
    return id;
}

QList<Task_Info> Data_Manager::getTasksForUser(qint64 user_id)
{
    QList<Task_Info> list;

    if (!db.isOpen()) {
        SPDLOG_ERROR("getTasksForUser: база не открыта");
        return list;
    }
    if (user_id < 0) {
        SPDLOG_WARN("getTasksForUser: некорректный user_id = {}", user_id);
        return list;
    }

    QSqlQuery q(db);
    q.prepare("SELECT task_id, task_name, user_id, created_at "
              "FROM task WHERE user_id = :u ORDER BY created_at DESC");
    q.bindValue(":u", user_id);

    if (!q.exec()) {
        SPDLOG_ERROR("getTasksForUser: ошибка SELECT: {}",
                     q.lastError().text().toStdString());
        return list;
    }

    while (q.next()) {
        Task_Info t;
        t.task_id    = q.value(0).toLongLong();
        t.task_name  = q.value(1).toString();
        t.user_id    = q.value(2).toLongLong();
        t.created_at = q.value(3).toDateTime();
        list.append(t);
    }

    SPDLOG_INFO("getTasksForUser: user_id = {}, найдено задач: {}",
                user_id, list.size());
    return list;
}

User_Profile Data_Manager::getCurrentUserProfile()
{
    if (currentUserId < 0){
        SPDLOG_WARN("getCurrentUserProfile: текущий пользователь не определён "
                    "(currentUserId = {})", currentUserId);
        return User_Profile();}
    if (!currentUserLoaded) {
        SPDLOG_DEBUG("getCurrentUserProfile: кеш пуст, загружаем профиль "
                     "для user_id = {}", currentUserId);
        currentUser       = getUserInfo(currentUserId);
        currentUserLoaded = true;
    }
    SPDLOG_INFO("getCurrentUserProfile: профиль загружен из БД, "
                "user_id = {}, login = '{}'",
                currentUser.user_id,
                currentUser.login.toStdString());
    return currentUser;
}


qint64 Data_Manager::sendSessionData(const Session_info& s)
{
    if (!db.isOpen()||s.user_id < 0) {
        SPDLOG_ERROR("sendSessionData: База данных не открыта или пользователь не определен");
        return -1;}

    QSqlQuery q(db);
    q.prepare("INSERT INTO sessions "
              "(user_id, task_id, technic_name, phase, "
              " start_time, finish_time, duration_sec, completed) "
              "VALUES (:uid, :tid, :tech, :phase, "
              "        :st, :ft, :dur, :done)");
    q.bindValue(":uid",   s.user_id);
    q.bindValue(":tid",   s.task_id >= 0 ? QVariant(s.task_id) : QVariant());
    q.bindValue(":tech",  technicToStr(s.technic));
    q.bindValue(":phase", phaseToStr(s.phase));
    q.bindValue(":st",    s.start_time);
    q.bindValue(":ft",    s.finish_time.isValid() ? QVariant(s.finish_time): QVariant());
    q.bindValue(":dur",   s.duration_sec);
    q.bindValue(":done",  s.completed ? 1 : 0);
    if (!q.exec()) {
        SPDLOG_ERROR("sendSessionData INSERT: {}",
                     q.lastError().text().toStdString());
        return -1;
    }
    const qint64 newId = q.lastInsertId().toLongLong();
    SPDLOG_INFO("sendSessionData: добавлена сессия session_id = {}, "
            "user_id = {}, task_id = {}, technic = '{}', phase = '{}', "
            "duration = {} сек",
            newId,
            s.user_id,
            s.task_id,
            technicToStr(s.technic).toStdString(),
            phaseToStr(s.phase).toStdString(),
            s.duration_sec);
    return newId;
}
QList<Session_info> Data_Manager::getSessionsForUser(qint64 user_id,
                                                     const QDateTime& from,
                                                     const QDateTime& to)
{
    QList<Session_info> list;
    if (!db.isOpen()) {
        SPDLOG_ERROR("getSessionsForUser: база не открыта,"
                     "возвращен список из {} сессий ",list.size());
        return list;
    }
    if (user_id < 0) {
        SPDLOG_ERROR("getSessionsForUser: некорректный user_id = {},"
                     "возвращен список из {} сессий", user_id,list.size());
        return list;
    }

    QSqlQuery q(db);
    q.prepare("SELECT session_id, user_id, task_id, technic_name, phase, "
              "start_time, finish_time, duration_sec, completed "
              "FROM sessions "
              "WHERE user_id = :u AND start_time BETWEEN :f AND :t "
              "ORDER BY start_time DESC");
    q.bindValue(":u", user_id);
    q.bindValue(":f", from);
    q.bindValue(":t", to);

    if (!q.exec()) {
        SPDLOG_ERROR("getSessionsForUser: {}",q.lastError().text().toStdString());
        return list;
    }
    while (q.next()) {
        Session_info s;
        s.session_id   = q.value(0).toLongLong();
        s.user_id      = q.value(1).toLongLong();
        s.task_id      = q.value(2).isNull() ? -1 : q.value(2).toLongLong();
        s.technic      = technicFromStr(q.value(3).toString());
        s.phase        = phaseFromStr(q.value(4).toString());
        s.start_time   = q.value(5).toDateTime();
        s.finish_time  = q.value(6).toDateTime();
        s.duration_sec = q.value(7).toUInt();
        s.completed    = q.value(8).toBool();
        list.append(s);
    }
    SPDLOG_INFO("getSessionsForUser: Возвращен список из {} сессий",list.size());
    return list;
}

// quint64 Data_Manager::totalDurationForTechnic(qint64 user_id,
//                                               FTR::TimerMode technic,
//                                               const QDateTime& from,
//                                               const QDateTime& to)
// {
//     if (!db.isOpen()) {
//         SPDLOG_ERROR("totalDurationForTechnic: база не открыта");
//         return 0;
//     }
//     if (user_id < 0) {
//         SPDLOG_WARN("totalDurationForTechnic: некорректный user_id = {}", user_id);
//         return 0;
//     }
//     QSqlQuery q(db);
//     q.prepare("SELECT COALESCE(SUM(duration_sec),0) FROM sessions "
//               "WHERE user_id = :u AND technic_name = :t "
//               "AND start_time BETWEEN :f AND :e");
//     q.bindValue(":u", user_id);
//     q.bindValue(":t", technicToStr(technic));
//     q.bindValue(":f", from);
//     q.bindValue(":e", to);
//     if (!q.exec()) {
//         SPDLOG_ERROR("totalDurationForTechnic: ошибка SELECT: {}",
//                      q.lastError().text().toStdString());
//         return 0;
//     }
//     if (!q.next()) {
//         SPDLOG_WARN("totalDurationForTechnic: запрос не вернул строку "
//                     "(user_id = {}, technic = {})",
//                     user_id, technic);
//         return 0;
//     }
//      const quint64 total = q.value(0).toULongLong();
//     SPDLOG_DEBUG("totalDurationForTechnic: user_id = {}, technic = '{}', "
//                  "{}..{} -> {} сек",
//                  user_id,
//                  technic,
//                  from.toString(Qt::ISODate).toStdString(),
//                  to.toString(Qt::ISODate).toStdString(),
//                  total);
//      return total;
// }


//АНАЛИТИКА


quint64 Data_Manager::totalDurationForPeriod(qint64 user_id,
                                             qint64 task_id,
                                             const QDateTime& from,
                                             const QDateTime& to)
{
    if (!db.isOpen()) {
        SPDLOG_ERROR("totalDurationForPeriod: база не открыта");
        return 0;
    }
    if (user_id < 0) {
        SPDLOG_WARN("totalDurationForPeriod: некорректный user_id = {}",
                    user_id);
        return 0;
    }
    if (!from.isValid() || !to.isValid() || from > to) {
        SPDLOG_WARN("totalDurationForPeriod: некорректный период "
                    "(from = {}, to = {})",
                    from.toString(Qt::ISODate).toStdString(),
                    to.toString(Qt::ISODate).toStdString());
        return 0;
    }

    // task_id < 0 => без фильтра по задаче
    QString sql = "SELECT COALESCE(SUM(duration_sec), 0) FROM sessions "
                  "WHERE user_id = :u AND phase = 'work' "
                  "AND start_time BETWEEN :f AND :e";
    if (task_id >= 0)
        sql += " AND task_id = :t";

    QSqlQuery q(db);
    q.prepare(sql);
    q.bindValue(":u", user_id);
    q.bindValue(":f", from);
    q.bindValue(":e", to);
    if (task_id >= 0)
        q.bindValue(":t", task_id);

    if (!q.exec()) {
        SPDLOG_ERROR("totalDurationForPeriod: ошибка SELECT: {}",
                     q.lastError().text().toStdString());
        return 0;
    }
    if (!q.next()) {
        SPDLOG_WARN("totalDurationForPeriod: запрос не вернул строку "
                    "(user_id = {}, task_id = {})", user_id, task_id);
        return 0;
    }

    const quint64 total = q.value(0).toULongLong();
    SPDLOG_DEBUG("totalDurationForPeriod: user_id = {}, task_id = {}, "
                 "{}..{} -> {} сек",
                 user_id, task_id,
                 from.toString(Qt::ISODate).toStdString(),
                 to.toString(Qt::ISODate).toStdString(),
                 total);
    return total;
}

QString Data_Manager::favoriteTechnic(qint64 user_id)
{
    if (!db.isOpen()) {
        SPDLOG_ERROR("favoriteTechnic: база не открыта");
        return QString();
    }
    if (user_id < 0) {
        SPDLOG_WARN("favoriteTechnic: некорректный user_id = {}", user_id);
        return QString();
    }

    QSqlQuery q(db);
    q.prepare("SELECT technic_name, COUNT(*) AS c "
              "FROM sessions "
              "WHERE user_id = :u AND phase = 'work' "
              "GROUP BY technic_name "
              "ORDER BY c DESC, technic_name ASC "
              "LIMIT 1");
    q.bindValue(":u", user_id);

    if (!q.exec()) {
        SPDLOG_ERROR("favoriteTechnic: ошибка SELECT: {}",
                     q.lastError().text().toStdString());
        return QString();
    }
    if (!q.next()) {
        SPDLOG_DEBUG("favoriteTechnic: у user_id = {} нет work-сессий",
                     user_id);
        return QString();
    }

    const QString technic = q.value(0).toString();
    const int     count   = q.value(1).toInt();
    SPDLOG_INFO("favoriteTechnic: user_id = {}, любимая техника '{}' "
                "({} work-сессий)",
                user_id, technic.toStdString(), count);
    return technic;
}

double Data_Manager::completionRate(qint64 user_id)
{
    if (!db.isOpen()) {
        SPDLOG_ERROR("completionRate: база не открыта");
        return 0.0;
    }
    if (user_id < 0) {
        SPDLOG_WARN("completionRate: некорректный user_id = {}", user_id);
        return 0.0;
    }

    QSqlQuery q(db);
    q.prepare("SELECT COUNT(*) AS t_sessions, "
              "       COALESCE(SUM(completed), 0) AS done "
              "FROM sessions WHERE user_id = :u");
    q.bindValue(":u", user_id);

    if (!q.exec()) {
        SPDLOG_ERROR("completionRate: ошибка SELECT: {}",
                     q.lastError().text().toStdString());
        return 0.0;
    }
    if (!q.next()) {
        SPDLOG_WARN("completionRate: запрос не вернул строку "
                    "(user_id = {})", user_id);
        return 0.0;
    }

    const int total = q.value(0).toInt();
    const int done  = q.value(1).toInt();

    if (total == 0) {
        SPDLOG_DEBUG("completionRate: у user_id = {} нет сессий", user_id);
        return 0.0;
    }

    const double res = static_cast<double>(done) / static_cast<double>(total);
    SPDLOG_INFO("completionRate: user_id = {}, завершено {}/{} ({:.2f}%)",
                user_id, done, total, res * 100.0);
    return res;
}

quint32 Data_Manager::longestSessionSec(qint64 user_id)
{
    if (!db.isOpen() || user_id < 0) {
         SPDLOG_ERROR("longestSessionSec: "
                     "База данных не открыта или пользователь не определен");
        return 0;}
    QSqlQuery q(db);
    q.prepare("SELECT duration_sec FROM sessions "
              "WHERE user_id = :u "
              "ORDER BY duration_sec DESC "
              "LIMIT 1 ");
    q.bindValue(":u", user_id);
    if (!q.exec() || !q.next()) {
        SPDLOG_WARN("longestSessionSec: {}",q.lastError().text().toStdString());
        return 0;
    }
    SPDLOG_INFO("longestSessionSec:Длительность сессии {} с",q.value(0).toUInt());
    return q.value(0).toUInt();
}

quint32 Data_Manager::averageSessionSec(qint64 user_id)
{
    if (!db.isOpen() || user_id < 0) {
         SPDLOG_ERROR("averageSessionSec:"
                     "База данных не открыта или пользователь не определен");
        return 0;}
    QSqlQuery q(db);
    q.prepare("SELECT AVG(duration_sec) FROM sessions "
              "WHERE user_id = :u");
    q.bindValue(":u", user_id);
    if (!q.exec()||!q.next()) {
        SPDLOG_ERROR("averageSessionSec: {}",
                    q.lastError().text().toStdString());
        return 0;
    }
    SPDLOG_INFO("averageSessionSec: Средняя длина сессии {} с",
                q.value(0).toDouble());
    return static_cast<quint32>(q.value(0).toDouble());
}

std::pair<int,int> Data_Manager::workBreakRatio(qint64 user_id)
{
    if (!db.isOpen() || user_id < 0) {
        SPDLOG_ERROR("База данных не открыта или пользователь не определен");
        return {0,0};}
    QSqlQuery q(db);
    q.prepare("SELECT SUM(duration_sec) "
              "FROM sessions "
              "WHERE user_id = :u AND phase= 'work'"
              );
    q.bindValue(":u", user_id);
    QSqlQuery q1(db);
    q1.prepare("SELECT SUM(duration_sec) "
              "FROM sessions "
              "WHERE user_id = :u AND phase= 'break'");
    q1.bindValue(":u", user_id);
    if (!q.exec() || !q.next()) {
        SPDLOG_ERROR("workBreakRatio: {}",q.lastError().text().toStdString());

        return {0,0};
    }
    if (!q1.exec() || !q1.next()) {
         SPDLOG_ERROR("workBreakRatio: {}", q1.lastError().text().toStdString() );
        return {0,0};
    }
    const quint64 w = q.value(0).toULongLong();
    const quint64 b = q1.value(0).toULongLong();
    quint64 total = w+b;
    if (total==0)return {0,0};
    int w_ = static_cast<int>(100.0*w/(double)total);
    int b_ = 100-w_;
    SPDLOG_INFO("Соотношение работа/отдых {}/{}",w_,b_);
    return {w_,b_};
}

int Data_Manager::procrastinationIndex(qint64 user_id)
{
    if (!db.isOpen() || user_id < 0) {
        SPDLOG_ERROR("procrastinationIndex: База не открыта "
                     "или пользователь не существует,"
                     "возвращен 0");
        return 0;}
    QSqlQuery q(db);
    q.prepare("SELECT COUNT(*) AS total, "
              "       SUM(CASE WHEN completed = 0 THEN 1 ELSE 0 END) AS undone "
              "FROM sessions WHERE user_id = :u");
    q.bindValue(":u", user_id);
    if (!q.exec() || !q.next()) {
        SPDLOG_ERROR("procrastinationIndex: {}",
                     q.lastError().text().toStdString());
        return 0;
    }
    const int total  = q.value(0).toInt();
    const int undone = q.value(1).toInt();
    if (total == 0) {
        SPDLOG_WARN("procrastinationIndex: total = {},возвращён 0",total);
        return 0;}
    auto res=100*double(undone) / double(total);
    SPDLOG_INFO("procrastinationIndex: Индекс прокрастинации: {}",res);
    return res;
}



Analytics_Summary Data_Manager::getAnalyticsSummary(qint64 user_id, qint64 task_id)
{
    Analytics_Summary s;
    if (!db.isOpen() || user_id < 0) {
        SPDLOG_ERROR("getAnalyticsSummary: база не открыта или user_id < 0");
        return s;}

    const QDate today = QDate::currentDate();

    const QDateTime dayFrom  (today,QTime(0,0,0));
    const QDateTime dayTo    (today,QTime(23,59,59));
    const QDateTime monthFrom(QDate(today.year(), today.month(),1),QTime(0,0,0));
    const QDateTime monthTo  (QDate(today.year(), today.month(),
                                  today.daysInMonth()),QTime(23,59,59));
    const QDateTime yearFrom (QDate(today.year(), 1, 1),QTime(0,0,0));
    const QDateTime yearTo   (QDate(today.year(), 12, 31),QTime(23,59,59));

    // ── время активности по периодам ──
    s.total_day_sec   = totalDurationForPeriod(user_id, task_id, dayFrom,   dayTo);
    s.total_month_sec = totalDurationForPeriod(user_id, task_id, monthFrom, monthTo);
    s.total_year_sec  = totalDurationForPeriod(user_id, task_id, yearFrom,  yearTo);

    s.avg_session_sec = averageSessionSec(user_id);
    s.completion_rate=completionRate(user_id);
    s.longest_session_sec=longestSessionSec(user_id);
    s.favorite_technic = favoriteTechnic(user_id);
    s.work_break_ratio = workBreakRatio(user_id);
    s.procrastination_index=procrastinationIndex(user_id);
    SPDLOG_INFO("getAnalyticsSummary: Объект заполнен");
    return s;
}
