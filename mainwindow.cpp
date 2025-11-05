#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QsLog/QsLog.h"
#include <QFileDialog>
#include "settings.h"
#include "UI/CammerWidget.h"
#include "HalconCpp.h"
#include "halconcpp/HDevThread.h"
#include <QRegExpValidator>
using namespace HalconCpp;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initUiLogger(this);//启动界面日志输出
    //setFixedSize(1280,1024);
    //resize(1280,1024);
    //PLC收到信号 控制相机拍照
    connect(&Controller::getInstance(),&Controller::start2DCapture,ui->widget_Camera2D_1,&CammerWidget::onStart2DCapture);

    connect(&Controller::getInstance(),&Controller::detectRemains,ui->widget_Camera2D_1,&CammerWidget::onDetectRemains);


    //启动检测
    connect(ui->pushButton_detectstart, &QPushButton::clicked, this, [=]() {
        DetectionSystem::getInstance()->startDetection();
    });

    //停止检测
    connect(ui->pushButton_detectstop, &QPushButton::clicked, this, [=]() {
        DetectionSystem::getInstance()->stopDetection();
    });

    connect(DetectionSystem::getInstance(), &DetectionSystem::stateChanged, this,&MainWindow::onDetectionStateChanged);
    connect(&Controller::getInstance(),&Controller::imgCaptured,this,&MainWindow::displayImage);
    showMaximized();
    ui->stackedWidget->setCurrentIndex(1);



    QRegExp regExp("^(-?\\d+)(\\.\\d+)?$");
    QRegExpValidator *validator = new QRegExpValidator(regExp, this);
    ui->lineEdit_rowNum->setValidator(validator);
    ui->lineEdit_ColNum->setValidator(validator);
    ui->lineEdit_rowGap->setValidator(validator);
    ui->lineEdit_ColGap->setValidator(validator);
    ui->lineEdit_gray_minimum->setValidator(validator);
    ui->lineEdit_gray_maximum->setValidator(validator);
    QString str_rowNum = QString::number(rowNum);
    QString str_colNum = QString::number(colNum);
    QString str_rowGap = QString::number(rowGap);
    QString str_colGap = QString::number(colGap);
    QString str_garyMin = QString::number(garyMin);
    QString str_grayMax = QString::number(grayMax);
    ui->lineEdit_rowNum->setText(str_rowNum);
    ui->lineEdit_ColNum->setText(str_colNum);
    ui->lineEdit_rowGap->setText(str_rowGap);
    ui->lineEdit_ColGap->setText(str_colGap);
    ui->lineEdit_gray_minimum->setText(str_garyMin);
    ui->lineEdit_gray_maximum->setText(str_grayMax);



    connect(this,&MainWindow::rowDataChanged,ui->widget_Camera2D_1,&CammerWidget::onRowDataChanged);

    connect(this,&MainWindow::colDataChanged,ui->widget_Camera2D_1,&CammerWidget::onColDataChanged);

    connect(this,&MainWindow::grayDataChanged,ui->widget_Camera2D_1,&CammerWidget::onGrayDataChanged);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initUiLogger(MainWindow *window)
{
    //init ui logger
    QsLogging::Logger& logger =  QsLogging::Logger::instance();
    logger.setLoggingLevel(QsLogging::DebugLevel);
    QsLogging::DestinationPtr objectDestination(QsLogging::DestinationFactory::MakeFunctorDestination(window,SLOT(logforUI(QString))));
    logger.addDestination(objectDestination);
}




void MainWindow::logforUI(const QString &message)
{
    ui->logtextEdit->append(message);
}


void MainWindow::displayImage(const QImage &image)
{
    //qDebug()<<"MainWindow::displayImage";
    ui->label_3d->setPixmap(QPixmap::fromImage(image).scaled(ui->label_3d->size()));
    img_3d = image;
}





void MainWindow::on_pushButton_init3_clicked()
{
     // Controller::getInstance().cameracontroller->initCamera();
    Controller::getInstance().cameracontroller->detectTest();
}

void MainWindow::on_pushButton_grab3_clicked()
{

     //Controller::getInstance().cameracontroller->captureImage();
    Controller::getInstance().onPlcRead(4502,2);
}


void MainWindow::on_pushButton_save3_clicked()
{
   // QString fileName = QFileDialog::getSaveFileName(this, "Save File", "", "Image Files (*.bmp)");
//    if (!fileName.isEmpty())
//    {
//        if (resMat.empty())
//        {
//            //ui->label_message->setText(QString::fromUtf8("请先采集一张图片!!!"));
//            return;
//        }
//        //cv::imwrite(fileName.toStdString(), cv::Mat::zeros(100,100,CV_8UC3));
//        cv::imwrite(fileName.toStdString(), resMat);
//    }
    //使用QPixmap保存
    //QPixmap::fromImage(img_3d).save(fileName);
    Controller::getInstance().onPlcRead(4502,1);
}

void MainWindow::on_pushButton_close3_clicked()
{
    /*Controller::getInstance().cameracontroller->unInitCamera();

    return;*/

    Controller::getInstance().onPlcRead(4520,2);
}


void MainWindow::on_pushButton_testSDK_clicked()
{

   // Controller::getInstance().cameracontroller->calculateByCloudPoint();
    //Controller::getInstance().cameracontroller->captureImage();
    //Controller::getInstance().cameracontroller->detectTest();

    Controller::getInstance().onPlcRead(4520,1);
}



void MainWindow::on_pushButton_config_clicked()
{

    ui->stackedWidget->setCurrentIndex(2);
    //Controller::getInstance().onPlcRead(4526,13);
}

void MainWindow::onDetectionStateChanged(DetectionSystem::State state)
{
    if(state == DetectionSystem::State::Idle){
        //qDebug()<<"idle now";
        ui->pushButton_detectstart->setEnabled(true);
        ui->pushButton_detectstop->setEnabled(false);
    }else{
        //qDebug()<<"detecting  now";
        ui->pushButton_detectstart->setEnabled(false);
        ui->pushButton_detectstop->setEnabled(true);
    }
}




void MainWindow::on_pushButton_detectstart_clicked()
{
    Controller::getInstance().onTimerStart();
}

void MainWindow::on_pushButton_detectstop_clicked()
{
    Controller::getInstance().onTimerStop();

}




void MainWindow::on_pushButton_3DCamera_clicked()
{
    //ui->stackedWidget->setCurrentIndex(0);
}



void MainWindow::on_pushButton_2DCamera1_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_2DCamera2_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_pushButton_setRow_clicked()
{
    //row
    if(ui->lineEdit_rowNum->text().isEmpty())
        return;
    if(ui->lineEdit_rowGap->text().isEmpty())
        return;

    emit rowDataChanged(ui->lineEdit_rowNum->text().toInt(),ui->lineEdit_rowGap->text().toInt());

    if(ui->lineEdit_ColNum->text().isEmpty())
        return;
    if(ui->lineEdit_ColGap->text().isEmpty())
        return;

    emit colDataChanged(ui->lineEdit_ColNum->text().toInt(),ui->lineEdit_ColGap->text().toInt());

}



void MainWindow::on_pushButton_setGray_clicked()
{
    //col
    if(ui->lineEdit_gray_minimum->text().isEmpty())
        return;
    if(ui->lineEdit_gray_maximum->text().isEmpty())
        return;

    emit grayDataChanged(ui->lineEdit_gray_minimum->text().toInt(),ui->lineEdit_gray_maximum->text().toInt());
}

void MainWindow::on_pushButton_setPr1_clicked()
{
    //
    QLOG_DEBUG()<<"step val:"<<ui->lineEdit_pr1Step->text();
    int val_step = ui->lineEdit_pr1Step->text().toInt();
    Controller::getInstance().setLedShinePos(val_step,4);
}

void MainWindow::on_pushButton_setPr0_clicked()
{
    //Controller::getInstance().onPosZero();
    Controller::getInstance().onPlcRead(4526,13);
}

void MainWindow::on_pushButton_setJOG_clicked()
{
    Controller::getInstance().onPosZero();
}

void MainWindow::on_pushButton_setPr0_2_clicked()
{
    Controller::getInstance().onZeroSet();
}

void MainWindow::on_pushButton_JOGpositive_clicked()
{
    Controller::getInstance().onPosJOG();
}

void MainWindow::on_pushButton_setJOGnagetive_clicked()
{
    Controller::getInstance().newCaculate();
}
