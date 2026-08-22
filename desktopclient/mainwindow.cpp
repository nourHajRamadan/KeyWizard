#include "mainwindow.h"
#include "ui_mainwindow.h"

enum class ListUpdateState
{
    None,
    WaitingForPasswords,
    WaitingForFavourites
};

ListUpdateState listUpdateState = ListUpdateState::None;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("KeyWizard");

    serialProcess = new QProcess(this);

#ifdef Q_OS_WIN
    serialProcess->setCreateProcessArgumentsModifier([](QProcess::CreateProcessArguments *args)
     {
         args->startupInfo->dwFlags |= STARTF_USESHOWWINDOW;
         args->startupInfo->wShowWindow = SW_HIDE;
     });
#endif

    connect(serialProcess, &QProcess::readyReadStandardOutput, this, &MainWindow::serialReadyRead);
    connect(serialProcess, &QProcess::readyReadStandardError, this, &MainWindow::serialReadyRead);
    connect(serialProcess, &QProcess::errorOccurred, this, &MainWindow::serialError);
    connect(serialProcess, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this, &MainWindow::serialFinished);

    refreshSerialPorts();

    if(ui->COMsComboBox->count() > 0)
        openSerialPort();

    ui->commandLineEdit->installEventFilter(this);


}


MainWindow::~MainWindow()
{
    delete ui;
    delete model1;
    delete model2;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    sendCommand("exit");

    QThread::msleep(200);

    if(serialProcess->state() != QProcess::NotRunning)
    {
        serialProcess->kill();
        serialProcess->waitForFinished(1000);
    }

    event->accept();
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui->commandLineEdit && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if (keyEvent->key() == Qt::Key_Up) {
            if (!prevCommands.empty()) {

                if (currentcommandindex > 0) {
                    currentcommandindex--;
                }
                ui->commandLineEdit->setText(prevCommands[currentcommandindex]);
            }
            return true;
        }

        else if (keyEvent->key() == Qt::Key_Down) {
            if (!prevCommands.empty()) {

                if (currentcommandindex < prevCommands.length() - 1) {
                    currentcommandindex++;
                    ui->commandLineEdit->setText(prevCommands[currentcommandindex]);
                }

                else if (currentcommandindex == prevCommands.length() - 1) {
                    currentcommandindex = prevCommands.length();
                    ui->commandLineEdit->clear();
                }
            }
            return true;
        }
    }

    return QMainWindow::eventFilter(watched, event);
}

QString MainWindow::getArduinoPath()
{
    QDir appDir(QCoreApplication::applicationDirPath());
    QString arduinoPath = appDir.absoluteFilePath("arduino-cli.exe");

    while(!QFile::exists(arduinoPath))
    {
        QMessageBox::warning(this, "Warning!", "Arduino CLI executable missing!");

        QString src = QFileDialog::getOpenFileName(
            this,
            "Find arduino-cli.exe",
            QDir::homePath(),
            "Arduino CLI (arduino-cli.exe)"
            );

        if(src.isEmpty())
            return QString();

        if(QFileInfo(src).fileName().compare("arduino-cli.exe", Qt::CaseInsensitive) != 0)
        {
            QMessageBox::warning(
                this,
                "Failure!",
                "The executable must be named \"arduino-cli.exe\"!"
                );
            continue;
        }

        if(!QFile::copy(src, arduinoPath))
        {
            QMessageBox::warning(
                this,
                "Failure!",
                "The executable wasn't copied successfully."
                );
            continue;
        }
    }

    return arduinoPath;
}

void MainWindow::refreshSerialPorts()
{
    ui->COMsComboBox->clear();

    const auto ports = QSerialPortInfo::availablePorts();

    for(const QSerialPortInfo &port : ports)
    {
        ui->COMsComboBox->addItem(port.portName(), port.portName());
    }
}

bool MainWindow::openSerialPort()
{
    if(serialProcess->state() != QProcess::NotRunning)
    {
        serialProcess->kill();
        serialProcess->waitForFinished(2000);
    }

    serialBuffer.clear();

    QString portName = ui->COMsComboBox->currentData().toString();

    if(portName.isEmpty())
        return false;

    QStringList arguments;

    arguments << "monitor";
    arguments << "-p";
    arguments << portName;
    arguments << "--config";
    arguments << "baudrate=115200";

    ui->dispLabel->setText("Connecting to " + portName + "...");
    QString cliPath = getArduinoPath();
    serialProcess->start(cliPath, arguments);

    qDebug() << "Arduino CLI path:" << cliPath;
    qDebug() << "Exists:" << QFile::exists(cliPath);

    if(!serialProcess->waitForStarted(3000))
    {
        ui->dispLabel->setText("Could not start Arduino CLI.");
        return false;
    }


    ui->dispLabel->setText("Connected to " + portName);

    return true;
}

void MainWindow::sendCommand(QString command)
{
    if(serialProcess->state() != QProcess::Running)
        return;

    command += '\n';

    serialProcess->write(command.toUtf8());
    serialProcess->waitForBytesWritten(500);
}

void MainWindow::serialReadyRead()
{
    QByteArray data = serialProcess->readAllStandardOutput();

    if(data.isEmpty())
        data = serialProcess->readAllStandardError();

    if(data.isEmpty())
        return;

    QString text = QString::fromUtf8(data);

    ui->terminaloutput->insertPlainText(text);

    ui->terminaloutput->ensureCursorVisible();

    //ui->dispLabel->setText(text);

    if(text=="KeyWizard unlocked."){ ui->dispLabel->setText("KeyWizard unlocked!"); unlocked=true; updateLists();}
    if(text.contains("False PIN",Qt::CaseInsensitive)) ui->dispLabel->setText("False PIN. Try again!");

    lastoutput=text;


    if(listUpdateState == ListUpdateState::WaitingForPasswords)
    {
        if(text.startsWith("|"))
        {
            QStringList passkeys = text.split('|', Qt::SkipEmptyParts);

            model1 = new QStringListModel(this);
            model1->setStringList(passkeys);
            ui->passwordsListView->setModel(model1);

            listUpdateState = ListUpdateState::WaitingForFavourites;
            sendCommand("get favourite _KEYS_");
        }

        return;
    }

    if(listUpdateState == ListUpdateState::WaitingForFavourites)
    {
        if(text.startsWith("|"))
        {
            QStringList favkeys = text.split('|', Qt::SkipEmptyParts);

            model2 = new QStringListModel(this);
            model2->setStringList(favkeys);
            ui->favouritesListView->setModel(model2);

            listUpdateState = ListUpdateState::None;
        }

        return;
    }
}

void MainWindow::serialFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);

    ui->dispLabel->setText("Serial monitor stopped.");

    serialBuffer.clear();
}

void MainWindow::serialError(QProcess::ProcessError error)
{
    qDebug() << "Arduino CLI process error:" << error;
    qDebug() << serialProcess->errorString();

    ui->dispLabel->setText("Arduino CLI error: " + serialProcess->errorString());
}


void MainWindow::on_commandLineEdit_returnPressed()
{
    QString tmp=ui->commandLineEdit->text();
    sendCommand(tmp);
    if(!tmp.startsWith("unlock"))prevCommands.append(tmp);
    ui->commandLineEdit->clear();
}

void MainWindow::on_commandLineEdit_textChanged(const QString &arg1)
{
    if(arg1.startsWith("unlock")){
        ui->commandLineEdit->setEchoMode(QLineEdit::Password);
    }else{
        ui->commandLineEdit->setEchoMode(QLineEdit::Normal);
    }
}

void MainWindow::on_COMsComboBox_activated(int index)
{
    Q_UNUSED(index);

    openSerialPort();
}

void MainWindow::resetSerial()
{
    if(serialProcess->state() != QProcess::NotRunning)
    {
        serialProcess->kill();
        serialProcess->waitForFinished(2000);
    }

    serialBuffer.clear();

    refreshSerialPorts();

    if(ui->COMsComboBox->count() > 0)
        openSerialPort();
}

void MainWindow::on_resetButton_clicked()
{
    resetSerial();
}


void MainWindow::on_getButton_clicked()
{
    updateLists();
}


void MainWindow::on_unlockButton_clicked()
{
    QString tmp = QInputDialog::getText(this,"KeyWizard","Enter unlock pin:",QLineEdit::Password);
    bool intok; tmp.toInt(&intok);
    if(tmp.isEmpty() || tmp.length()!=6){
        ui->dispLabel->setText("Unlock failed!");
        return;
    }
    ui->dispLabel->setText("Please wait...");
    sendCommand(QString("unlock %1").arg(tmp));
}


void MainWindow::updateLists()
{
    if(!unlocked) return;
    listUpdateState = ListUpdateState::WaitingForPasswords;
    sendCommand("get password _KEYS_");
}

void MainWindow::on_passwordsListView_clicked(const QModelIndex &index)
{
    qDebug()<<index.data();
}


void MainWindow::on_favouritesListView_clicked(const QModelIndex &index)
{
    qDebug()<<index.data();
}

