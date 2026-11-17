#include "password_utils.h"
#include "data_manager.h"
#include "qmessagebox.h"
#include <QRandomGenerator>
#include <QCryptographicHash>
#include <QPasswordDigestor>
#include <QByteArray>

QString generateSalt()
{
    QByteArray salt(16, 0);
    for (int i = 0; i < 16; ++i)
        salt[i] = char(QRandomGenerator::system()->bounded(256));
    return QString::fromLatin1(salt.toHex());}

QString hashPassword(const QString& password, const QString& salt)
{
    QByteArray data = (salt + password).toUtf8();
    QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha256);
    return QString::fromLatin1(hash.toHex());
}

bool verifyPassword(const QString& password,
                    const QString& saltHex,
                    const QString& storedHashHex)
{
    return hashPassword(password, saltHex) == storedHashHex;
}

bool password_check(QWidget* window,const QString& password)
{
    SPDLOG_INFO("password_check: len={}, bytes=[{}]",
                password.size(),
                password.toUtf8().toHex().toStdString());
    if (password.size()<8) {
        QMessageBox::information(window,"Ошибка","Пароль должен содержать не менее 8ми символов");
        return false;
    }
    if (!password_letters_allowed(password)){
        QMessageBox::information(window,"Ошибка","Пароль должен состоять из латинских букв");
        return false;
    }
    if (!password_letters_has_upper_case(password)){
        QMessageBox::information(window,"Ошибка","Пароль должен содержать буквы верхнего регистра");
        return false;
    }
    if (!password_letters_has_lower_case (password)){
        QMessageBox::information(window,"Ошибка","Пароль должен содержать буквы нижнего регистра");
        return false;}
    if (!password_has_digits(password)){
        QMessageBox::information(window,"Ошибка","Пароль должен содержать цифры");
        return false;
    }
    if (!password_has_symbols(password)){
        QMessageBox::information(window,"Ошибка","Пароль должен содержать символы");
        return false;
    }
    if (password_has_space(password)){
        QMessageBox::information(window,"Ошибка","Пароль не должен содержать пробелы");
        return false;
    }
    SPDLOG_INFO("Пароль корректный");
    return true;
}

bool password_letters_allowed(const QString& password)
{
    static const QString specials = QStringLiteral("!@#$%^&*");
    for (QChar c : password) {
        const ushort u = c.unicode();
        if ((u >= 'A' && u <= 'Z') ||
            (u >= 'a' && u <= 'z') ||
            (u >= '0' && u <= '9') ||
            specials.contains(c))
            continue;
        return false;
    }
    return true;
}
bool password_letters_has_upper_case(const QString& password)
{
    for (QChar c : password) {
        if (c >= 'A' && c <= 'Z') return true;
    }
    return false;
}

bool password_letters_has_lower_case(const QString& password)
{
    for (QChar c : password){
        if (c >= 'a'&& c <= 'z') return true;
    }
    return false;
}

bool password_has_digits(const QString& password)
{
    for (QChar c : password) {
        if (c.isDigit()) return true;
    }
    return false;
}
bool password_has_symbols(const QString& password)
{
    static const QString specials = "!@#$%^&*";
    for (QChar c : password){
        if (specials.contains(c)) return true;
    }
    return false;
}

bool password_has_space(const QString& password)
{
    for (QChar c : password)
    {if (c ==' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v') return true;}
    return false;
}

bool check_login(QWidget* window, const QString& login){
    if (login.isEmpty()) {
        QMessageBox::information(window, "Ошибка", "Логин не может быть пустым");
        return false;
    }
    if (login.size() < 3) {
        QMessageBox::information(window, "Ошибка", "Логин должен содержать не менее 3 символов");
        return false;
    }
    // В логине разрешены только латинские буквы и цифры
    for (QChar c : login) {
        const ushort u = c.unicode();
        if ((u >= 'A' && u <= 'Z') || (u >= 'a' && u <= 'z') || (u >= '0' && u <= '9'))
            continue;
        QMessageBox::information(window, "Ошибка", "Логин может содержать только латинские буквы и цифры");
        return false;
    }
    SPDLOG_INFO("Логин корректный");
    return true;
}

bool check_phone(QWidget* window, const QString& phone){
    if (phone.isEmpty()){
        QMessageBox::information(window, "Ошибка", "Телефон не может быть пустым");
        return false;
    }
    for (QChar c : phone){
        if (c < '0' || c > '9'){
            QMessageBox::information(window,"Ошибка","Телефон должен содержать только цифры");
            SPDLOG_ERROR("check_phone: Введено не число");
            return false;
        }
    }
    // Проверка длины
    if (phone.size() != 10 && phone.size() != 11) {
        QMessageBox::information(window, "Ошибка", "Телефон должен содержать 10 или 11 цифр");
        return false;
    }
    SPDLOG_INFO("Телефон корректный");
    return true;
}

bool check_email(QWidget* window, const QString& email)
{
    if (email.isEmpty()) {
        QMessageBox::information(window, "Ошибка", "Email не может быть пустым");
        return false;
    }
    if (!email.contains('@')) {
        QMessageBox::information(window, "Ошибка", "Email должен содержать @");
        return false;
    }
    const int at = email.indexOf('@');
    if (email.indexOf('.', at) == -1){
        QMessageBox::information(window, "Ошибка", "Email должен содержать точку после @");
        return false;
    }
    if (password_has_space(email)) {
        QMessageBox::information(window, "Ошибка", "Email не должен содержать пробелы");
        return false;
    }
    //только латиница, цифры и символы @ . - _
    for (QChar c : email) {
        const ushort u = c.unicode();
        if ((u >= 'A' && u <= 'Z') || (u >= 'a' && u <= 'z') || (u >= '0' && u <= '9') ||
            c == '@' || c == '.' || c == '-' || c == '_') {
            continue;
        }
        QMessageBox::information(window, "Ошибка", "Email содержит недопустимые символы (только латиница)");
        return false;
    }
    SPDLOG_INFO("Email корректный");
    return true;
}

QString extractDigits(const QString& text)
{
    QString digits;
    for (QChar c : text)
        if (c >= '0' && c <= '9') digits += c;

    if (digits == "7" && text.contains('_'))
        return {};
    return digits;
}
