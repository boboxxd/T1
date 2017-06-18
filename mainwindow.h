#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include<QThread>
#include <QtCore>
#include<QTimer>
using namespace cv;
namespace Ui {
class MainWindow;
}

struct INFO
{
    QString user;
    QString passwd;
    QString ip;
    QString port;
};

class Thread : public QThread
{
    Q_OBJECT
private:
    int state;
    VideoCapture *cap;
    INFO *info;
    int fps;
protected:
    void run();

public:
    Thread(QObject *parent=0);
    ~Thread();

signals:
    void UpdateSignal(int state,int fps);
public slots:
    void State();
    void setInfo(INFO *info);
};




class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void updatestate(int state,int fps);
    void onconnectbtn();
    void oncancelbtn();
    void updateuser(QString user);
    void updatepasswd(QString passwd);
    void updateip(QString ip);
    void updateport(QString port);

signals:
    void updateinfo(INFO *info);
private:
    Ui::MainWindow *ui;
    Thread *mythread;
    QTimer *timer;
    INFO *info;
    QString time;
};

#endif // MAINWINDOW_H
