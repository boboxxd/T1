#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QDebug>
#include<QString>
#include<QTime>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    buttongroup.addButton(ui->haikangradio,0);
    buttongroup.addButton(ui->nanruiradio,1);
    buttongroup.addButton(ui->otherradio,2);
    buttongroup.setExclusive(true);
    ui->haikangradio->setChecked(true);
    ui->otherEdit->setPlainText("rtsp://admin:ad53937301@49.91.240.44:554/h264/ch1/main/av_stream");
    ui->otherEdit->setVisible(false);
    ui->label_5->setVisible(false);

    timer=new QTimer(this);
    mythread=new Thread;
    connect(&buttongroup, SIGNAL(buttonClicked (int)), mythread, SLOT(selectcamera(int)) );
    connect(&buttongroup, SIGNAL(buttonClicked (int)), this, SLOT(selectcamera(int)) );
    connect(mythread,SIGNAL(sendMessage (QString)),this,SLOT(showMessage(QString)));

    info=new INFO;
    ui->userEdit->setText("admin");
    ui->passwdEdit->setText("ad53937301");
    ui->ipEdit->setText("49.91.240.44");
    ui->portEdit->setText("554");
    info->user="admin";
    info->passwd="ad53937301";
    info->ip="49.91.240.44";
    info->port="554";
    connect(mythread,SIGNAL(UpdateSignal(int,int)),this,SLOT(updatestate(int,int)));
    connect(timer,SIGNAL(timeout()),mythread,SLOT(State()));
    connect(ui->connectButton,SIGNAL(clicked()),this,SLOT(onconnectbtn()));
    connect(ui->cancelButton,SIGNAL(clicked()),this,SLOT(oncancelbtn()));
    connect(ui->userEdit,SIGNAL(textChanged(QString)),this,SLOT(updateuser(QString)));
    connect(ui->passwdEdit,SIGNAL(textChanged(QString)),this,SLOT(updatepasswd(QString)));
    connect(ui->ipEdit,SIGNAL(textChanged(QString)),this,SLOT(updateip(QString)));
    connect(ui->portEdit,SIGNAL(textChanged(QString)),this,SLOT(updateport(QString)));
    connect(this,SIGNAL(updateinfo(INFO *)),mythread,SLOT(setInfo(INFO *)));
    connect(ui->otherEdit,SIGNAL(textChanged()),this,SLOT(setothertxt()));
}

void MainWindow::onconnectbtn()
{
    ui->connectButton->setEnabled(false);

    mythread->quit();
    emit updateinfo(info);
    mythread->start();
    timer->start(5000);

}

void MainWindow::oncancelbtn()
{
   ui->connectButton->setEnabled(true);
   mythread->quit();
   timer->stop();
   ui->textBrowser->clear();
}
void MainWindow::updateuser(QString user)
{
    info->user=user;
}
void MainWindow::updatepasswd(QString passwd)
{
    info->passwd=passwd;
}
void MainWindow::updateip(QString ip)
{
    info->ip=ip;
}

void MainWindow::updateport(QString port)
{
    info->port=port;
}

void MainWindow::updatestate(int state,int fps)
{
    time=QTime::currentTime().toString();
    switch(state)
    {
    case -1:

        ui->textBrowser->append("["+time+"] "+"连接失败...");
        break;
    case 0:
        ui->textBrowser->append("["+time+"] "+"正在连接...");

        break;
    case  1:
        ui->textBrowser->append("["+time+"] "+"已连接,FPS 为"+QString::fromStdString(std::to_string(fps)));
        break;
    }
}

void MainWindow::setothertxt()
{
    mythread->othertxt=ui->otherEdit->toPlainText();
}

void MainWindow::selectcamera(int sort)
{
    if(sort==2)
    {
        ui->otherEdit->setVisible(true);
        ui->label_5->setVisible(true);
        ui->userEdit->setDisabled(true);
        ui->passwdEdit->setDisabled(true);
        ui->ipEdit->setDisabled(true);
        ui->portEdit->setDisabled(true);
        mythread->othertxt=ui->otherEdit->toPlainText();
    }else
    {
        ui->otherEdit->setVisible(false);
        ui->label_5->setVisible(false);
        ui->userEdit->setDisabled(false);
        ui->passwdEdit->setDisabled(false);
        ui->ipEdit->setDisabled(false);
        ui->portEdit->setDisabled(false);
    }
}

void MainWindow::showMessage(QString msg)
{
    ui->textBrowser->append(msg);
}


MainWindow::~MainWindow()
{
    delete ui;
}

Thread::Thread(QObject *parent)
{
    state = 0;
    cap=new VideoCapture;
    info=new INFO;
}

Thread::~Thread()
{
    delete cap;
    delete info;

}

void Thread::run()
{
    state=0;
    fps=-1;
    emit UpdateSignal(state,fps);
    switch(sort)
    {
    case 0:
        url="\0";
        url="rtsp://"+info->user.trimmed()+":"+info->passwd.trimmed()+"@"+info->ip.trimmed()+":"+info->port.trimmed()+"/h264/ch1/main/av_stream";
        break;
    case 1:
        url="\0";
        url="rtsp://"+info->user.trimmed()+":"+info->passwd.trimmed()+"@"+info->ip.trimmed()+":"+info->port.trimmed()+"/stream/av0_0";
        break;
    case 2:
        //url="\0";
        url=othertxt.trimmed();
        break;
    }
    qDebug()<<"正在连接："<<url;
    emit sendMessage("  "+url);
    cap->open(url.toStdString());


    //cap->open("rtsp://admin:ad53937301@49.91.240.441:554/h264/ch1/main/av_stream");
    if (!cap->isOpened())
    {
        state=-1;
        emit UpdateSignal(state,-1);
        return;
    }
        state=1;
        fps=cap->get(CV_CAP_PROP_FPS);
        emit UpdateSignal(state,fps);


}

void Thread::State()
{
    emit UpdateSignal(state,fps);
}

void Thread::setInfo(INFO *info)
{
    this->info->user=info->user;
    this->info->passwd=info->passwd;
    this->info->ip=info->ip;
    this->info->port=info->port;
}

void Thread::selectcamera(int sort)
{
    this->sort=sort;
    qDebug()<<this->sort;
}

