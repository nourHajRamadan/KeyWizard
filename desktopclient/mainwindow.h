#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QThread>
#include <QDebug>
#include <QTimer>
#include <QCloseEvent>
#include <QSerialPortInfo>
#include <QInputDialog>
#include <QStringListModel>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void closeEvent(QCloseEvent *event) override;
    QString getArduinoPath();
    void refreshSerialPorts();
    bool openSerialPort();
    void sendCommand(QString command);
    void resetSerial();
    void serialFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void serialError(QProcess::ProcessError error);

    void updateLists();

private slots:
    void serialReadyRead();

    void on_commandLineEdit_returnPressed();

    void on_COMsComboBox_activated(int index);

    void on_resetButton_clicked();

    void on_getButton_clicked();

    void on_unlockButton_clicked();

    void on_commandLineEdit_textChanged(const QString &arg1);

    void on_passwordsListView_clicked(const QModelIndex &index);

    void on_favouritesListView_clicked(const QModelIndex &index);

protected:

    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    Ui::MainWindow *ui;

    QProcess *serialProcess;
    QByteArray serialBuffer;
    QVector<QString> prevCommands;
    unsigned int currentcommandindex=0;
    QString lastoutput;

    bool unlocked=false;
    QTimer* updateListsTimer=NULL;

    QStringListModel *model1,*model2;
};

#endif // MAINWINDOW_H


