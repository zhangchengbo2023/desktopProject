#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "controller.h"
#include "cameracontroller.h"
#include "UI/CammerWidget.h"
#include "IMVApi.h"
#include "opencv2/opencv.hpp"
#include "ThreeDCameraApi/ThreeDCameraApi.hpp"
#include "opencv2/imgproc/types_c.h"
#include "pcl/io/io.h"
#include "pcl/io/pcd_io.h"
#include "pcl/io/ply_io.h"
#include "pcl/PCLPointCloud2.h"
#include "pcl/point_types.h"
#include "pcl/point_cloud.h"
#include "pcl/filters/voxel_grid.h"
#include "pcl/filters/passthrough.h"
#include "pcl/filters/uniform_sampling.h"
#include "pcl/filters/statistical_outlier_removal.h"
#include "pcl/filters/radius_outlier_removal.h"
#include "pcl/filters/approximate_voxel_grid.h"
#include "pcl/ModelCoefficients.h"
#include "pcl/segmentation/sac_segmentation.h"
#include "pcl/segmentation/progressive_morphological_filter.h"
#include "pcl/features/moment_of_inertia_estimation.h"
#include "pcl/filters/extract_indices.h"
#include "pcl/filters/model_outlier_removal.h"
#include "pcl/segmentation/extract_clusters.h"
#include "pcl/surface/poisson.h"
#include "pcl/surface/gp3.h"
#include "pcl/kdtree/kdtree_flann.h"
#include "pcl/features/normal_3d.h"
#include "pcl/features/normal_3d_omp.h"
#include "pcl/visualization/pcl_visualizer.h"
#include "pcl/common/transforms.h"
#include "pcl/registration/ndt.h"
#include "pcl/registration/gicp.h"
#include "boost/thread/thread.hpp"
#include "boost/shared_ptr.hpp"
#include "HalconCpp.h"
#include <omp.h>
#include <QDateTime>
#include <QCoreApplication>
#include "CloudProcess.h"
#include "QVTKWidget.h"
#include <opencv2/opencv.hpp>
#include <QImage>
#include "detectionsystem.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void initUiLogger(MainWindow* window);
    QImage cvMat2QImage(const cv::Mat& mat){

        if(mat.type() == CV_8UC3){
            //qDebug()<<"cv 8uc3";
            const uchar* pSrc = (const uchar*)mat.data;
                    // Create QImage with same dimensions as input cv::Mat
                    QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
                    return image.rgbSwapped();
        }
    }
    void toPointCloud();
    bool LoadImageToHObject(HObject &ho_Image);
    void generateTemplate();
    unsigned short crc16(const QByteArray &data);
signals:
    void templatePos(QVector<HTuple>&);
    void rowDataChanged(int,int);
    void colDataChanged(int,int);
    void grayDataChanged(int,int);
private slots:


    void logforUI(const QString& message);


    void displayImage(const QImage& image);


    void on_pushButton_init3_clicked();

    void on_pushButton_grab3_clicked();

    void on_pushButton_save3_clicked();

    void on_pushButton_close3_clicked();



    void on_pushButton_testSDK_clicked();



    void on_pushButton_config_clicked();

    //同步检测状态
    void onDetectionStateChanged(DetectionSystem::State state);






    void on_pushButton_detectstart_clicked();

    void on_pushButton_detectstop_clicked();


    void on_pushButton_3DCamera_clicked();



    void on_pushButton_2DCamera1_clicked();

    void on_pushButton_2DCamera2_clicked();

    void on_pushButton_setRow_clicked();


    void on_pushButton_setGray_clicked();

    void on_pushButton_setPr1_clicked();

    void on_pushButton_setPr0_clicked();

    void on_pushButton_setJOG_clicked();

    void on_pushButton_setPr0_2_clicked();

    void on_pushButton_JOGpositive_clicked();

    void on_pushButton_setJOGnagetive_clicked();

private:
    Ui::MainWindow *ui;
    void initUi();
    QImage img_3d;//保存图像
    IMV_DeviceList m_deviceInfoList;//cameras

    Controller* controller;

    int rowNum=4;
    int colNum=6;
    int rowGap=515;
    int colGap=515;
    int garyMin=0;
    int grayMax=255;

};
#endif // MAINWINDOW_H
