#include "information.h"
#include "qpainter.h"
#include "spdlog/spdlog.h"
#include "ui_information.h"

Information::Information(QWidget *parent,const QString& text)
    : QWidget(parent)
    , ui(new Ui::Information),text(text)
{
    ui->setupUi(this);
    this->setWindowTitle("Information");
    ui->scrollArea->viewport()->setStyleSheet("background: transparent;");
    ui->plainTextEdit->setPlainText(text);
    resize(800,600);
    connect(ui->pushButton,&QPushButton::clicked,this,&Information::on_pushButton_clicked);
    SPDLOG_TRACE("Окно информации создано");
}

void Information::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    QPixmap bg(":/img/cl1 (3)");
    if (bg.isNull()) {
        SPDLOG_WARN("Изображение не загружено");
        return;}
    QSize s = bg.size().scaled(size(), Qt::KeepAspectRatioByExpanding);
    QPixmap scaled = bg.scaled(s, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    p.drawPixmap((width()  - scaled.width())  / 2,
                 (height() - scaled.height()) / 2, scaled);
    SPDLOG_TRACE("Фон установлен");
}

Information::~Information()
{   SPDLOG_TRACE("Окно информации удалено");
    delete ui;
}

void Information::getback_clicked()
{
    if (!parent_window) {
        SPDLOG_ERROR("Родительское окно не загружено");
        return;}
    parent_window ->show();
    this->hide();
    SPDLOG_ERROR("Возвращение к окну {}", parent_window->objectName().toStdString());
}

void Information::on_pushButton_clicked()
{
    SPDLOG_TRACE("Нажата кнопка назад");
    getback_clicked();
}


