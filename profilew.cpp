#include "profilew.h"
#include "analythic_page.h"
#include "basetracker.h"
#include "mainwindow.h"
#include "qpainter.h"
#include "ui_profilew.h"
#include "profilew.h"
#include "qpainter.h"
#include "ui_profilew.h"
#include "data_manager.h"
#include "user_settings.h"
#include <QFileDialog>
#include <QBuffer>
#include <QMessageBox>



profileW::profileW(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::profileW)
{
    ui->setupUi(this);
    this->setWindowTitle("Профиль");

    ui->label_3->setFixedSize(180, 180);
    ui->label_3->setAlignment(Qt::AlignCenter);
    ui->label_3->setScaledContents(false);

    loadUserProfile();

    // Кнопка "Изменить фотографию"
    connect(ui->pushButton, &QPushButton::clicked,
            this, &profileW::onChangeAvatarClicked);

    // Кнопка "Сохранить"
    connect(ui->pushButton_5, &QPushButton::clicked,
            this, &profileW::onSaveClicked);
    SPDLOG_TRACE("Пользовательская страница создана");
}
void profileW::paintEvent(QPaintEvent *) {
    QPainter p(this);
    QPixmap bg(":/img/clock (1).jpg");
    if (bg.isNull()) {
        SPDLOG_WARN("Изображение не загружено");
        return;}
    QSize s = bg.size().scaled(size(), Qt::KeepAspectRatioByExpanding);
    QPixmap scaled = bg.scaled(s, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    p.drawPixmap((width()  - scaled.width())  / 2,
                 (height() - scaled.height()) / 2, scaled);
}

profileW::~profileW()
{SPDLOG_TRACE("Пользовательская страница удалена");
    delete ui;
}

void profileW::getback_clicked()
{
    if (!openWindow) {
        SPDLOG_ERROR("Указатель на родительское окно пуст");
        return;}
    if (auto* mw = qobject_cast<MainWindow*>(openWindow)) {
        mw->loadUserProfile();
        mw->loadCurrentUser();
    } else if (auto* bt = qobject_cast<BaseTracker*>(openWindow)) {
        bt->loadUserProfile();
        bt->loadCurrentUser();
    }
        openWindow->show();

    this->hide();
}

void profileW::on_pushButton_3_clicked()
{
    getback_clicked();
}


void profileW::loadUserProfile()
{
    Data_Manager& dm = Data_Manager::instance();
    if(!me){
        me = std::make_shared<User_Profile>(dm.getCurrentUserProfile());}

    //ГОСТЬ
    if (me->user_id < 0) {
        ui->lineEdit->setText(tr("Гость"));
        ui->lineEdit->setReadOnly(true);

        ui->plainTextEdit->setPlainText(
            tr("Вы вошли как гость. Войдите в аккаунт, чтобы сохранять прогресс."));
        ui->plainTextEdit->setReadOnly(true);

        // Аватарка-заглушка
        QPixmap pix(":/img/exit (1).jpg");
        if (!pix.isNull()) {
            ui->label_3->setPixmap(pix.scaled(180, 180,
                                              Qt::KeepAspectRatio,
                                              Qt::SmoothTransformation));
        }

        ui->pushButton->setEnabled(false);   // Изменить фотографию
        ui->pushButton_2->setEnabled(false); // Аккаунт
        ui->pushButton_5->setEnabled(false); // Сохранить
        return;
    }

    //ПОЛЬЗОВАТЕЛЬ
    ui->lineEdit->setText(me->Name + " " + me->Surname);
    ui->plainTextEdit->setPlainText(me->About);

    updateAvatar();
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_5->setEnabled(true);
    loadTasks();
}
void profileW::updateAvatar()
{
    if (!me) return;

    QPixmap src;
    if (!me->Avatar.isEmpty()) {
        if (!src.loadFromData(me->Avatar)) {
            src = QPixmap(":/img/icon_question.png");
        }
    } else {
        src = QPixmap(":/img/icon_question.png");
    }

    if (src.isNull()) {
        ui->label_3->setText("?");
        ui->label_3->setStyleSheet(
            "QLabel { color: white; font-size: 100px; font-weight: bold; }");
        return;
    }

    QSize target = ui->label_3->size();
    if (target.isEmpty()) return;

    ui->label_3->setPixmap(src.scaled(
        target,
        Qt::KeepAspectRatioByExpanding,   // заполняет лейбл, обрезая лишнее
        Qt::SmoothTransformation));
}
void profileW::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateAvatar();
}
void profileW::showQuestionMark()
{
    // Если есть картинка — используем её, иначе рисуем "?" текстом
    QPixmap q(":/img/icon_question.png");
    if (!q.isNull()) {
        ui->label_3->setPixmap(q);
    } else {
        ui->label_3->setText("?");
        ui->label_3->setStyleSheet(
            "QLabel { color: white; font-size: 100px; font-weight: bold; }");
    }
}

void profileW::onChangeAvatarClicked()
{
    QString path = QFileDialog::getOpenFileName(
        this, tr("Выберите изображение"), QString(),
        tr("Изображения (*.png *.jpg *.jpeg *.bmp)"));

    if (path.isEmpty()) return;

    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) return;
    QByteArray bytes = f.readAll();
    f.close();

    QPixmap pix;
    if (!pix.loadFromData(bytes)) {
        QMessageBox::warning(this, tr("Ошибка"),
                             tr("Не удалось загрузить изображение."));
        return;
    }
    QSize target = ui->label_3->size();
    // Показать в label
    ui->label_3->setPixmap(pix.scaled(
        target,
        Qt::KeepAspectRatioByExpanding, // сохраняет пропорции, заполняя всё пространство
        Qt::SmoothTransformation        // сглаживание, чтобы не было пикселей
        ));
    // Запомнить байты
    pendingAvatar = bytes;
    avatarChanged = true;
}

void profileW::onSaveClicked()
{
    Data_Manager& dm = Data_Manager::instance();
    if (dm.get_currentUser() < 0) return;

    User_Profile me = dm.getCurrentUserProfile();

    me.Name    = ui->lineEdit->text().section(' ', 0, 0);
    me.Surname = ui->lineEdit->text().section(' ', 1, 1);
    me.About   = ui->plainTextEdit->toPlainText();

    if (avatarChanged) {
        me.Avatar = pendingAvatar;
    }

    if (dm.updateUserData(me)) {
        QMessageBox::information(this, tr("Готово"), tr("Профиль сохранён."));
        emit profileUpdated();
    }else {
        QMessageBox::warning(this, tr("Ошибка"),
                             tr("Не удалось сохранить профиль."));
    }


}

void profileW::on_pushButton_2_clicked()
{
    if (!settings) {
        settings = new user_settings();
        settings->set_me(this->me);
        // Настройки должны возвращаться в MainWindow, а не в profileW
        settings->setMainWindow(qobject_cast<MainWindow*>(this->openWindow));
        settings->loadCurrentUser();
    }

    settings->show();
    this->hide();
}

void profileW::on_pushButton_6_clicked()
{   SPDLOG_TRACE("Нажата кнопка выйти");
    Data_Manager& dm = Data_Manager::instance();

    dm.setCurrentUser(-1);

    me.reset();
    avatarChanged = false;
    pendingAvatar.clear();

    if (settings) {
        settings->hide();
        settings->set_me(nullptr);
        delete settings;
        settings = nullptr;
    }

    loadUserProfile();


    if (auto* mw = qobject_cast<MainWindow*>(openWindow)) {
        mw->loadUserProfile();
        mw->loadCurrentUser();
    }
    getback_clicked();
}




void profileW::on_pushButton_4_clicked()
{   SPDLOG_TRACE("Нажата кнопка аналитика");
    Analythic_page* page = new Analythic_page();
    page->setAttribute(Qt::WA_DeleteOnClose);
    page->set_profileW(this);
    page->set_profile();
    page->loadUserProfile();
    page->updateAvatar();
    page->show();
    this->hide();
}
// profilew.cpp — добавляем include, если его нет
#include <QLabel>
#include <QScrollArea>

// ── Очистка контейнера задач ──────────────────────────────────────
void profileW::clearTasksLayout()
{
    QLayout* lay = tasksLayout();
    if (!lay) return;

    while (QLayoutItem* item = lay->takeAt(0)) {
        if (QWidget* w = item->widget()) {
            w->deleteLater();          // удаляем виджет-лейбл
        }
        delete item;                   // удаляем сам layout-item
    }
}

// ── Доступ к layout, куда кладём задачи ──────────────────────────
QVBoxLayout* profileW::tasksLayout() const
{
    // В .ui это verticalLayout_9 внутри scrollAreaWidgetContents_2.
    // Если label_5/label_6 удалены — layout остался пустым, и это ок.
    return qobject_cast<QVBoxLayout*>(
        ui->scrollAreaWidgetContents_2->layout());
    // если хочешь класть прямо в verticalLayout_10 — верни его вместо этого
}

void profileW::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    loadUserProfile();
    //loadTasks();
}
void profileW::loadTasks()
{
    if (!tasksLayout()) {
        SPDLOG_ERROR("loadTasks: layout для задач не найден");
        return;
    }

    clearTasksLayout();

    Data_Manager& dm = Data_Manager::instance();

    // Гость — задач нет
    const qint64 uid = dm.get_currentUser();
    if (uid < 0) {
        QLabel* hint = new QLabel(tr("Войдите в аккаунт, чтобы видеть задачи."),
                                  ui->scrollAreaWidgetContents_2);
        hint->setStyleSheet("color: rgba(255,255,255,0.6); padding: 8px;");
        hint->setWordWrap(true);
        tasksLayout()->addItem(new QSpacerItem(0, 0)); // не обязательно
        tasksLayout()->addWidget(hint);
        tasksLayout()->addStretch();
        return;
    }

    const QList<Task_Info> tasks = dm.getTasksForUser(uid);

    if (tasks.isEmpty()) {
        QLabel* hint = new QLabel(tr("Пока нет ни одной задачи."),
                                  ui->scrollAreaWidgetContents_2);
        hint->setStyleSheet("color: rgba(255,255,255,0.6); padding: 8px;");
        hint->setWordWrap(true);
        tasksLayout()->addWidget(hint);
        tasksLayout()->addStretch();
        return;
    }

    // ── Создаём лейбл на каждую задачу ──
    for (const Task_Info& t : tasks) {
        // Вариант A: обычный QLabel
        QLineEdit* le = new QLineEdit(t.task_name, ui->scrollAreaWidgetContents_2);
        le->setReadOnly(true);                    // только просмотр
        le->setStyleSheet(
            "QLineEdit {"
            "  background: rgba(0,0,0,0.45);"
            "  color: white;"
            "  border: 1px solid rgba(255,255,255,0.25);"
            "  border-radius: 8px;"
            "  padding: 8px 12px;"
            "  font-size: 14px;"
            "  font-weight: bold;"
            "}");
        le->setProperty("task_id", QVariant::fromValue(t.task_id));
        tasksLayout()->addWidget(le);

    }

    tasksLayout()->addStretch();  // прижать задачи к верху

    SPDLOG_INFO("loadTasks: пользователю user_id = {} показано {} задач",
                uid, tasks.size());
}
