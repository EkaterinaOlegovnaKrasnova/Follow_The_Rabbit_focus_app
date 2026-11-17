#ifndef PASSWORD_UTILS_H
#define PASSWORD_UTILS_H

#include <QString>
#include <QWidget>

// Соль — строка hex (16 случайных байт → 32 hex-символа).
// Генерируется при регистрации/смене пароля и хранится в БД рядом с хешем.
// Нужна, чтобы одинаковые пароли у разных пользователей давали разные хеши
QString generateSalt();
// Хеш = SHA-256(salt + password) → hex.
QString hashPassword(const QString& password, const QString& saltHex);

// Проверка пароля: пересчитывает хеш от введённого пароля с той же солью
// и сравнивает с сохранённым хешем. Возвращает true, если совпало.
bool verifyPassword(const QString& password,
                    const QString& saltHex,
                    const QString& storedHashHex);
// Полная проверка: длина, регистр, цифры, символы, пробелы
bool password_check(QWidget* window,const QString& password);
// Только разрешённые символы (латиница, цифры, !@#$%^&*)
bool password_letters_allowed(const QString& password);
 // Есть хотя бы одна заглавная латинская буква
bool password_letters_has_upper_case(const QString& password);
 // Есть хотя бы одна строчная латинская буква
bool password_letters_has_lower_case(const QString& password);
bool password_has_digits(const QString& password);// Есть хотя бы одна цифра
bool password_has_symbols(const QString& password);// Есть хотя бы одна цифра
 // Есть пробельный символ (пробел, таб, перевод строки и т.д.)
bool password_has_space(const QString& password);
// Валидация логина
// Логин: не пустой, минимум 3 символа, только латиница и цифры.
bool check_login(QWidget* window,const QString& password);
//Валидация телефона
// Телефон: не пустой, только цифры, длина 10 или 11 (РФ).
bool check_phone(QWidget* window,const QString& phone);
//Валидация email
// Email: не пустой, есть @, есть точка после @, без пробелов,
// только латиница, цифры и символы @ . - _.
bool check_email(QWidget* window,const QString& email);
// Извлекает только цифры из строки .Возвращает пустую строку, если после фильтрации осталась только "7"
//и в исходной строке был символ '_' (признак незаполненной маски).
QString extractDigits(const QString& text);

#endif // PASSWORD_UTILS_H
