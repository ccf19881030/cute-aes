#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aes.h"

#include <QDebug>
#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    aes = new CuteAES(CuteAES::AES_128_MODE, CuteAES::ECB_MODE);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_encryptButton_clicked()
{
    QString text = ui->eTextEdit->toPlainText();
    QString key = ui->eKeyEdit->toPlainText();

    QByteArray text_bin = text.toUtf8();
    QByteArray key_bin = key.toUtf8();

    QByteArray encrypted = aes->encrypt(text_bin, key_bin, nullptr);

    ui->RawTextBrowser->setText(encrypted.toHex());
    ui->UtfTextBrowser->setText(QTextCodec::codecForMib(1015)->toUnicode(encrypted));
    ui->AsciiTextBrowser->setText(QString::fromStdString(
        std::string(encrypted.constData(),
        static_cast<unsigned long long>(encrypted.length()))
    ));
}

void MainWindow::on_decryptButton_clicked()
{
    QString text = ui->dTextEdit->toPlainText();
    QString key = ui->dKeyEdit->toPlainText();

    QByteArray text_bin;
    if (ui->dTextModeEdit->currentIndex() == 0) {
        text_bin = QByteArray::fromHex(text.toUtf8());
    } else if (ui->dTextModeEdit->currentData() == 1) {
        QTextCodec *codec = QTextCodec::codecForName("UTF-16");
        QTextEncoder *encoder = codec->makeEncoder(QTextCodec::IgnoreHeader);

        text_bin = encoder->fromUnicode(text);
    } else if (ui->dTextModeEdit->currentData() == 2) {
        text_bin = text.toLocal8Bit();
    }

    QByteArray key_bin = key.toUtf8();

    QByteArray decrypted = aes->decrypt(text_bin, key_bin, nullptr);

    ui->RawTextBrowser->setText(decrypted.toHex());
    ui->UtfTextBrowser->setText(QTextCodec::codecForMib(1015)->toUnicode(decrypted));
    ui->AsciiTextBrowser->setText(QString::fromStdString(
        std::string(decrypted.constData(),
        static_cast<unsigned long long>(decrypted.length()))
    ));
}

void MainWindow::on_ExitAction_triggered()
{
    close();
}
