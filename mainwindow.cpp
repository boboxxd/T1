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
    timer=new QTimer(this);
    mythread=new Thread;
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
    cap->open(QString("rtsp://"+info->user+":"+info->passwd+"@"+info->ip+":"+info->port+"/h264/ch1/main/av_stream").toStdString());
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

