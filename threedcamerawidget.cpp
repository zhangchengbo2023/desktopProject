//#include "threedcamerawidget.h"
//#include "ui_threedcamerawidget.h"
//#include <QFileDialog>
//threedCameraWidget::threedCameraWidget(QWidget *parent) :
//    QWidget(parent),
//    ui(new Ui::threedCameraWidget)
//{
//    ui->setupUi(this);
//}

//threedCameraWidget::~threedCameraWidget()
//{
//    delete ui;
//}

//void threedCameraWidget::on_pushButton_init_clicked()
//{
//    Dev::ThreeDCameraApi::ScanCameras(cameras);
//    if (cameras.size() == 0)
//    {
//        //std::cout << "No device found" << std::endl;
//        ui->label_message->setText(QString::fromUtf8("找不到设备!!!"));
//        return;
//    }
//    //// [3]创建设备句柄,连接设备0
//    //Dev::ThreeDCameraApi cam_handle(cameras[0]);
//    // 初始化相机
//    //std::cout << "Init3DCamera()" << std::endl;
//    if (cam_handle.Init3DCamera(cameras[0]) < 0)
//    {
//        //std::cerr << "Init camera calculate core failed" << std::endl;
//        //ui.lineEditMsg->setText(QString::fromUtf8("初始化相机失败!!!"));
//        return;
//    }

//    ui->pushButton_grab->setEnabled(true);
//    ui->pushButton_save->setEnabled(true);
//    ui->pushButton_close->setEnabled(true);
//}

//void threedCameraWidget::on_pushButton_grab_clicked()
//{
//    if (cam_handle.StartScan() < 0)
//    {
//        //std::cerr << "Failed to StartScan!" << std::endl;
//        ui->label_message->setText(QString::fromUtf8("扫描失败!!!"));
//        return;
//    }
//    // [6] 获取扫描结果
//    cv::Mat cvMatDeepMat;
//    if (0 != cam_handle.GetDeepMat(cvMatDeepMat))
//    {
//        //std::cerr << "Failed to get result" << std::endl;
//        ui->label_message->setText(QString::fromUtf8("获取扫描结果失败!!!"));
//        return;
//    }
//    else {
//        cvMatDeepMat.convertTo(resMat, CV_8UC3, 1, 0);
//        DisplayMat(resMat);
//    }
//}
//void threedCameraWidget::DisplayMat(cv::Mat image)
//{
//    cv::Mat rgb;

//    QImage img;

//    if (image.channels() == 3)
//    {
//        cvtColor(image, rgb, CV_BGR2RGB);
//        img = QImage((const unsigned char*)(rgb.data),
//            rgb.cols, rgb.rows, rgb.cols * rgb.channels(),//rgb.cols*rgb.channels()可以替换为image.step
//            QImage::Format_RGB888);
//    }
//    else
//    {
//        img = QImage((const unsigned char*)(image.data),
//            image.cols, image.rows, rgb.cols * image.channels(),
//            QImage::Format_RGB888);
//    }

//    ui->label->setPixmap(QPixmap::fromImage(img).scaled(ui->label->size()));//setPixelmap(QPixmap::fromImage(img));
//    ui->label->resize(ui->label->pixmap()->size());//resize(ui->label->pixmap()->size());
//}

//void threedCameraWidget::on_pushButton_save_clicked()
//{
//    QString fileName = QFileDialog::getSaveFileName(this, "Save File", "", "Image Files (*.bmp)");
//    if (!fileName.isEmpty())
//    {
//        if (resMat.empty())
//        {
//            ui->label_message->setText(QString::fromUtf8("请先采集一张图片!!!"));
//            return;
//        }
//        //cv::imwrite(fileName.toStdString(), cv::Mat::zeros(100,100,CV_8UC3));
//        cv::imwrite(fileName.toStdString(), resMat);
//    }
//}

//void threedCameraWidget::on_pushButton_close_clicked()
//{
//    cam_handle.Uninit3DCamera();
//    return;
//}
