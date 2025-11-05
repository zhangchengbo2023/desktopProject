#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include <QImage>
#include <QObject>
#include <QThread>
#include <QString>

#include <opencv2/opencv.hpp>
#include <ThreeDCameraApi/ThreeDCameraApi.hpp>
#include <QDebug>
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
#include <QString>
class CameraController : public QObject
{
    Q_OBJECT
public:
    CameraController(QObject *parent = nullptr);

    ~CameraController();
    void initCamera();
    void unInitCamera();
    void captureImage();
    void exportCloudPoint(const cv::Mat& mat);
    void calculateByCloudPoint(pcl::PointCloud<pcl::PointXYZ>::Ptr cloudpoint);
    void detectTest();
signals:
    void imageCaptured(const QImage& image);
    void posDetected(QVector<QString>);
private:
    QImage cvMat2QImage(const cv::Mat& mat);
    void DisplayMat(cv::Mat& image);
    QString version;
    std::vector <Dev::ThreeDCameraApi::ThreeDCameraProperties> cameras;
    //handle
    Dev::ThreeDCameraApi cam_Handle;
    cv::Mat resMat;

};

#endif // CAMERACONTROLLER_H
