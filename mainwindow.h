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
#include<QButtonGroup>
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
    int sort;//摄像机品牌
    QString url;
protected:
    void run();

public:
    Thread(QObject *parent=0);
    ~Thread();
    QString othertxt;
signals:
    void UpdateSignal(int state,int fps);
    void sendMessage (QString msg);
public slots:
    void selectcamera(int sort);
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
    void selectcamera(int);
    void showMessage(QString msg);
void setothertxt();

signals:
    void updateinfo(INFO *info);

private:
    Ui::MainWindow *ui;
    Thread *mythread;
    QTimer *timer;
    INFO *info;
    QString time;
    QButtonGroup buttongroup;
};

#endif // MAINWINDOW_H
