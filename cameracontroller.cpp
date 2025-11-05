#include "cameracontroller.h"
#include "QsLog/QsLog.h"
CameraController::CameraController(QObject *parent):QObject(parent)
{

}

CameraController::~CameraController()
{

}

void CameraController::initCamera()
{
//    Dev::ThreeDCameraApi::ScanCameras(cameras);

//    if (cameras.size() == 0)
//    {
//        //std::cout << "No device found" << std::endl;
//        //ui->label_message->setText(QString::fromUtf8("找不到设备!!!"));
//        QLOG_DEBUG()<<"camera not found";
//        return;
//    }else{
//        QLOG_DEBUG()<<"found camera :"<<cameras.size() ;
//    }
//    //// [3]创建设备句柄,连接设备0
//    //Dev::ThreeDCameraApi cam_handle(cameras[0]);
//    // 初始化相机

//    if (cam_Handle.Init3DCamera(cameras[0]) < 0)
//    {
//        //std::cerr << "Init camera calculate core failed" << std::endl;
//        //ui.lineEditMsg->setText(QString::fromUtf8("初始化相机失败!!!"));
//        QLOG_DEBUG()<<"camera init failed";
//        return;
//    }

}

void CameraController::unInitCamera()
{
//    QLOG_DEBUG()<<" CameraController::unInitCamera";
//    cam_Handle.Uninit3DCamera();
//    return;
}

void CameraController::captureImage()
{
//    QLOG_DEBUG()<<" CameraController::captureImage";
//    if (cam_Handle.StartScan() < 0)
//    {
//        QLOG_DEBUG() << "Failed to StartScan!";
//        //ui->label_message->setText(QString::fromUtf8("扫描失败!!!"));
//        return;
//    }
//    // [6] 获取扫描结果
//    cv::Mat cvMatDeepMat;
//    if (0 != cam_Handle.GetDeepMat(cvMatDeepMat))
//    {
//        QLOG_DEBUG() << "Failed to get result" ;
//        //ui->label_message->setText(QString::fromUtf8("获取扫描结果失败!!!"));
//        return;
//    }
//    else {
//        cvMatDeepMat.convertTo(resMat, CV_8UC3, 1, 0);
//        DisplayMat(resMat);
//        exportCloudPoint(cvMatDeepMat);
//    }
}

void CameraController::exportCloudPoint(const cv::Mat &mat)
{
//    //深度图转点云
//    pcl::PointCloud<pcl::PointXYZ>::Ptr scandCloud(new pcl::PointCloud<pcl::PointXYZ>);
//    //点云尺寸信息

//        scandCloud->width=mat.rows * mat.cols;
//        //scandCloud->width=cvMatDeepMat.height * cvMatDeepMat.width;
//        scandCloud->height=1;
//        scandCloud->is_dense=false;
//        scandCloud->resize(mat.rows * mat.cols);
//        //scandCloud->resize(cam_handle.m_iImgHeight*cam_handle.m_iImgWidth);
//        mat.forEach<cv::Point3f>([&](const cv::Point3f& val, const int* pos)
//        {
//            const int r = pos[0];
//            const int c = pos[1];
//            if (val.z > 1)
//            {
//                //对点云进行XYZ赋值
//                scandCloud->points[mat.rows * c + r].x=val.x;
//                scandCloud->points[mat.rows * c + r].y=val.y;
//                scandCloud->points[mat.rows * c + r].z=val.z;
//            }
//        });

//      //点云测试
////        boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer (new pcl::visualization::PCLVisualizer ("3D Viewer"));
////        viewer->setBackgroundColor (0, 0, 0);
////        viewer->addPointCloud<pcl::PointXYZ>(scandCloud, "sample cloud");
////        viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "sample cloud");
////        viewer->addCoordinateSystem (1.0);
////        viewer->initCameraParameters ();
////        // 主循环
////         while (!viewer->wasStopped ())
////         {
////               viewer->spinOnce (100);
////               //boost::this_thread::sleep (boost::posix_time::microseconds (100000));
//        //         }
//        calculateByCloudPoint(scandCloud);
}

void CameraController::calculateByCloudPoint(pcl::PointCloud<pcl::PointXYZ>::Ptr cloudpoint)
{
    //点云算法处理对象
//    OD::CloudProcess cp;
//    //定义PCL(XYZ类型)点云
//    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud1(new pcl::PointCloud<pcl::PointXYZ>);
//    pcl::PointCloud<pcl::PointXYZ>::Ptr cloudFilter(new pcl::PointCloud<pcl::PointXYZ>);
//    pcl::PointCloud<pcl::PointXYZ>::Ptr cloudFilter1(new pcl::PointCloud<pcl::PointXYZ>);
//    pcl::PointCloud<pcl::PointXYZ>::Ptr cloudFilter2(new pcl::PointCloud<pcl::PointXYZ>);

////    /************************读取本地文件****************************/
//    QString executableDir = QCoreApplication::applicationDirPath();
//    //ui->plainTextEdit->appendPlainText(u8"程序可执行文件路径：" + executableDir);
//    qDebug() << QString("程序可执行文件路径：" + executableDir) ;
//    QDir dir(executableDir);
//    dir.cdUp();
//    QString proDir = dir.absolutePath();
//    //读取手眼标定文件
//    std::vector<double> he;
//    cp.readHandEyePara(proDir + "/files/HandEye.yml",he);
//    if(he.size()>0)
//    {
//        //ui->plainTextEdit->appendPlainText(u8"读取手眼标定文件成功。");
//        qDebug() << QString("读取手眼标定文件成功");
//    }
//    //读取本地点云
//    QString filePath = QString::fromLocal8Bit(proDir.toLatin1() + "/files/cloud.ply");
//    cp.readPLYCloud(filePath,cloud1);//读取点云
//    //ui->plainTextEdit->appendPlainText(u8"cloud1点云大小:" + QString::number(cloud1->size()));
//    qDebug() <<QString("cloud1点云大小:");
//    qDebug() <<QString::number(cloud1->size());

//    /************************点云预处理****************************/
//    //底面过滤,手动建立底面平面参数矩阵
//    Eigen::Matrix4f m4f = Eigen::Matrix4f::Ones();
//    m4f(0,0) = 1;
//    m4f(0,1) = 0;
//    m4f(0,2) = -0.08;
//    m4f(1,0) = 0;
//    m4f(1,1) = -1;
//    m4f(1,2) = -0.04;
//    m4f(2,0) = -0.08;
//    m4f(2,1) = 0.03;
//    m4f(2,2) = -1;
//    m4f(0,3) = 32.72;
//    m4f(1,3) = 39.81;
//    m4f(2,3) = 826.17;
//    m4f(3,0) = 0;
//    m4f(3,1) = 0;
//    m4f(3,2) = 0;
//    m4f(3,3) = 1;
//    std::vector<double> posePlane;//平面位姿
//    //底面过滤，输出点云结果及平面位姿
//    cp.planeFilterZUp(cloudpoint,m4f,10.00,700.00,cloudFilter,posePlane);
//    //打印平面位姿
//    if(posePlane.size()>0)
//    {
//        //ui->plainTextEdit->appendPlainText(u8"平面位姿生成成功。");
//        qDebug() << QString("平面位姿生成成功") ;
//    }

//    //ui->plainTextEdit->appendPlainText(u8"底面过滤后点云大小:" + QString::number(cloudFilter->size()));//打印滤波后点云大小
//    qDebug() << QString("底面过滤后点云大小:");
//    qDebug() << QString::number(cloudFilter->size());
//    cp.rangeFilter(cloudFilter,-221.83,-152.12,769.62,301.40,167.80,864.62,cloudFilter1);//范围过滤
//    //ui->plainTextEdit->appendPlainText(u8"范围过滤后点云:" + QString::number(cloudFilter1->size()));//打印滤波后点云大小
//    qDebug() << QString("范围过滤后点云:" + QString::number(cloudFilter1->size()));
//    cp.CloudClustering(cloudFilter1,1,100,999999999,cloudFilter2);//点云聚类分割
//    //ui->plainTextEdit->appendPlainText(u8"聚类分割后点云大小:" + QString::number(cloudFilter2->size()));//打印滤波后点云大小
//    qDebug() << QString("聚类分割后点云大小:" + QString::number(cloudFilter2->size()));

//    /************************匹配建模及匹配****************************/
//    //匹配
//    HalconCpp::HTuple surfaceModelID,testCloud,hv_HasPoint;
//    std::vector<double> scoreVec,poseVec;
//    std::vector<pcl::PointCloud<pcl::PointXYZ>::Ptr> matchCloudVec;
//    cp.cloudPclToCloudHalcon(testCloud,cloudFilter2,cloudFilter2->size());
//    pcl::PointCloud<pcl::PointXYZ>::Ptr modelCloud(new pcl::PointCloud<pcl::PointXYZ>);
//    cp.createSurfaceModel(proDir + "/files/model.ply",0.03,modelCloud,surfaceModelID);//创建模板
//    GetObjectModel3dParams(testCloud, "has_points", &hv_HasPoint);
//    //目标点云存在就执行匹配
//    if(hv_HasPoint == HTuple("true"))
//    {
//        cp.findSurfaceModel(surfaceModelID,cloudFilter2,modelCloud,0.03,0.2,0.35,24,180,0.6,scoreVec,poseVec,matchCloudVec,false);
//        //ui->plainTextEdit->appendPlainText(u8"匹配结果数量:" + QString::number(scoreVec.size()));
//        qDebug() << QString("匹配结果数量");
//        qDebug() <<QString::number(scoreVec.size());
//    }

//    /************************位姿转换及处理****************************/
//    std::vector<double> matchPose;//机器人坐标系下的匹配结果位姿
//    std::vector<double> piecePose;//基准物料位姿(机器人坐标系下的)
//    std::vector<double> matchPoseAngle;//匹配到的物料相比于基准物料的角度
//    std::vector<float> camPara;
//    cp.readHandEyePara(proDir + "/files/piecePose.yml",piecePose);
//    cp.poseTransform(poseVec,he,"XYZ",matchPose);//匹配结果位姿转到机器人坐标系下，位姿欧拉角格式为XYZ
//    cp.poseAngleCal(matchPose,piecePose,"Z",matchPoseAngle);//计算匹配结果位姿相对于基准物料位姿在Z向角度
//    //ui->plainTextEdit->appendPlainText(u8"计算的角度数量:" + QString::number(matchPoseAngle.size()));//
//    qDebug() << QString("计算的角度数量:" + QString::number(matchPoseAngle.size())) ;
//    for (int i=0;i< int (matchPoseAngle.size());i++)
//    {
//        //ui->plainTextEdit->appendPlainText(u8"计算的角度第"  + QString::number(i) +u8"个为：" + QString::number(matchPoseAngle[i]));
//        qDebug() << QString("计算的角度第");
//        qDebug() << QString::number(i);
//        qDebug() << QString("个为");
//        qDebug() << QString::number(matchPoseAngle[i]);
//    }

//    /************************叠料筛选****************************/
//    //基准物料位姿的Z向和物料Z向反向，所以180度附近的为水平物料，假设设置叠料倾角阈值在0-176，筛选出叠料
//    double angleMax = 170.0;
//    std::vector<double> resPoseVec;//叠料物料位姿
//    for (int i = 0;i< int(matchPoseAngle.size());i++)
//    {
//        if(matchPoseAngle[i]<angleMax)
//        {
//            resPoseVec.push_back(matchPose[i * 7]);
//            resPoseVec.push_back(matchPose[i * 7 + 1]);
//            resPoseVec.push_back(matchPose[i * 7 + 2]);
//            resPoseVec.push_back(matchPose[i * 7 + 3]);
//            resPoseVec.push_back(matchPose[i * 7 + 4]);
//            resPoseVec.push_back(matchPose[i * 7 + 5]);
//            resPoseVec.push_back(matchPose[i * 7 + 6]);
//            //ui->plainTextEdit->appendPlainText(u8"叠料角度为：" + QString::number(matchPoseAngle[i]));//打印叠料角度
//            qDebug() << QString("叠料角度:");
//            qDebug() << QString::number(matchPoseAngle[i]);
//        }
//    }
//    //ui->plainTextEdit->appendPlainText(u8"叠料物料个数:" + QString::number(resPoseVec.size()/7));
//    qDebug() << QString("叠料物料个数:");
//    qDebug() << QString::number(resPoseVec.size()/7);
//    std::vector<double> outPose;
//    cp.poseSort(resPoseVec,u8"Z 从大到小",outPose);
//    for (int i=0;i<int(outPose.size());i++)
//    {
//        if(i%7==0)
//        {
//            //ui->plainTextEdit->appendPlainText(u8"第" + QString::number(i/7 + 1) + "个叠料位姿为：");
//            //qDebug()<<QString("第");
//            qDebug()<<QString::number(i/7 + 1);
//            qDebug()<<QString("个叠料位姿为");
//        }
//        //ui->plainTextEdit->appendPlainText(QString::number(outPose[i]));//打印叠料位姿
//        qDebug()<<QString(QString::number(outPose[i]));
 //   }
}

void CameraController::detectTest()
{
//    //点云算法处理对象
//    OD::CloudProcess cp;
//    //定义PCL(XYZ类型)点云
//    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud1(new pcl::PointCloud<pcl::PointXYZ>);
//    pcl::PointCloud<pcl::PointXYZ>::Ptr cloudFilter(new pcl::PointCloud<pcl::PointXYZ>);
//    pcl::PointCloud<pcl::PointXYZ>::Ptr cloudFilter1(new pcl::PointCloud<pcl::PointXYZ>);
//    pcl::PointCloud<pcl::PointXYZ>::Ptr cloudFilter2(new pcl::PointCloud<pcl::PointXYZ>);

////    /************************读取本地文件****************************/
//    QString executableDir = QCoreApplication::applicationDirPath();
//    //ui->plainTextEdit->appendPlainText(u8"程序可执行文件路径：" + executableDir);
//    qDebug() << QString("程序可执行文件路径：" + executableDir) ;
//    QDir dir(executableDir);
//    dir.cdUp();
//    QString proDir = dir.absolutePath();
//    //读取手眼标定文件
//    std::vector<double> he;
//    cp.readHandEyePara(proDir + "/files/HandEye.yml",he);
//    if(he.size()>0)
//    {
//        //ui->plainTextEdit->appendPlainText(u8"读取手眼标定文件成功。");
//        qDebug() << QString("读取手眼标定文件成功");
//    }
//    //读取本地点云
//    QString filePath = QString::fromLocal8Bit(proDir.toLatin1() + "/files/cloud2.ply");
//    cp.readPLYCloud(filePath,cloud1);//读取点云
//    //ui->plainTextEdit->appendPlainText(u8"cloud1点云大小:" + QString::number(cloud1->size()));
//    qDebug() <<QString("cloud1点云大小:");
//    qDebug() <<QString::number(cloud1->size());

//    /************************点云预处理****************************/
//    //底面过滤,手动建立底面平面参数矩阵
//    Eigen::Matrix4f m4f = Eigen::Matrix4f::Ones();
//    m4f(0,0) = 1;
//    m4f(0,1) = 0;
//    m4f(0,2) = -0.08;
//    m4f(1,0) = 0;
//    m4f(1,1) = -1;
//    m4f(1,2) = -0.04;
//    m4f(2,0) = -0.08;
//    m4f(2,1) = 0.03;
//    m4f(2,2) = -1;
//    m4f(0,3) = 32.72;
//    m4f(1,3) = 39.81;
//    m4f(2,3) = 826.17;
//    m4f(3,0) = 0;
//    m4f(3,1) = 0;
//    m4f(3,2) = 0;
//    m4f(3,3) = 1;
//    std::vector<double> posePlane;//平面位姿
//    //底面过滤，输出点云结果及平面位姿
//    cp.planeFilterZUp(cloud1,m4f,10.00,700.00,cloudFilter,posePlane);
//    //打印平面位姿
//    if(posePlane.size()>0)
//    {
//        //ui->plainTextEdit->appendPlainText(u8"平面位姿生成成功。");
//        qDebug() << QString("平面位姿生成成功") ;
//    }

//    //ui->plainTextEdit->appendPlainText(u8"底面过滤后点云大小:" + QString::number(cloudFilter->size()));//打印滤波后点云大小
//    qDebug() << QString("底面过滤后点云大小:");
//    qDebug() << QString::number(cloudFilter->size());
//    cp.rangeFilter(cloudFilter,-221.83,-152.12,769.62,301.40,167.80,864.62,cloudFilter1);//范围过滤
//    //ui->plainTextEdit->appendPlainText(u8"范围过滤后点云:" + QString::number(cloudFilter1->size()));//打印滤波后点云大小
//    qDebug() << QString("范围过滤后点云:" + QString::number(cloudFilter1->size()));
//    cp.CloudClustering(cloudFilter1,1,100,999999999,cloudFilter2);//点云聚类分割
//    //ui->plainTextEdit->appendPlainText(u8"聚类分割后点云大小:" + QString::number(cloudFilter2->size()));//打印滤波后点云大小
//    qDebug() << QString("聚类分割后点云大小:" + QString::number(cloudFilter2->size()));

//    /************************匹配建模及匹配****************************/
//    //匹配
//    HalconCpp::HTuple surfaceModelID,testCloud,hv_HasPoint;
//    std::vector<double> scoreVec,poseVec;
//    std::vector<pcl::PointCloud<pcl::PointXYZ>::Ptr> matchCloudVec;
//    cp.cloudPclToCloudHalcon(testCloud,cloudFilter2,cloudFilter2->size());
//    pcl::PointCloud<pcl::PointXYZ>::Ptr modelCloud(new pcl::PointCloud<pcl::PointXYZ>);
//    cp.createSurfaceModel(proDir + "/files/model.ply",0.03,modelCloud,surfaceModelID);//创建模板
//    GetObjectModel3dParams(testCloud, "has_points", &hv_HasPoint);
//    //目标点云存在就执行匹配
//    if(hv_HasPoint == HTuple("true"))
//    {
//        cp.findSurfaceModel(surfaceModelID,cloudFilter2,modelCloud,0.03,0.2,0.35,24,180,0.6,scoreVec,poseVec,matchCloudVec,false);
//        //ui->plainTextEdit->appendPlainText(u8"匹配结果数量:" + QString::number(scoreVec.size()));
//        qDebug() << QString("匹配结果数量");
//        qDebug() <<QString::number(scoreVec.size());
//    }

//    /************************位姿转换及处理****************************/
//    std::vector<double> matchPose;//机器人坐标系下的匹配结果位姿
//    std::vector<double> piecePose;//基准物料位姿(机器人坐标系下的)
//    std::vector<double> matchPoseAngle;//匹配到的物料相比于基准物料的角度
//    std::vector<float> camPara;
//    cp.readHandEyePara(proDir + "/files/piecePose.yml",piecePose);
//    cp.poseTransform(poseVec,he,"XYZ",matchPose);//匹配结果位姿转到机器人坐标系下，位姿欧拉角格式为XYZ
//    cp.poseAngleCal(matchPose,piecePose,"Z",matchPoseAngle);//计算匹配结果位姿相对于基准物料位姿在Z向角度
//    //ui->plainTextEdit->appendPlainText(u8"计算的角度数量:" + QString::number(matchPoseAngle.size()));//
//    qDebug() << QString("计算的角度数量:" + QString::number(matchPoseAngle.size())) ;
//    for (int i=0;i< int (matchPoseAngle.size());i++)
//    {
//        //ui->plainTextEdit->appendPlainText(u8"计算的角度第"  + QString::number(i) +u8"个为：" + QString::number(matchPoseAngle[i]));
//        qDebug() << QString("计算的角度第");
//        qDebug() << QString::number(i);
//        qDebug() << QString("个为");
//        qDebug() << QString::number(matchPoseAngle[i]);
//    }

//    /************************叠料筛选****************************/
//    //基准物料位姿的Z向和物料Z向反向，所以180度附近的为水平物料，假设设置叠料倾角阈值在0-176，筛选出叠料
//    double angleMax = 170.0;
//    std::vector<double> resPoseVec;//叠料物料位姿
//    for (int i = 0;i< int(matchPoseAngle.size());i++)
//    {
//        if(matchPoseAngle[i]<angleMax)
//        {
//            resPoseVec.push_back(matchPose[i * 7]);
//            resPoseVec.push_back(matchPose[i * 7 + 1]);
//            resPoseVec.push_back(matchPose[i * 7 + 2]);
//            resPoseVec.push_back(matchPose[i * 7 + 3]);
//            resPoseVec.push_back(matchPose[i * 7 + 4]);
//            resPoseVec.push_back(matchPose[i * 7 + 5]);
//            resPoseVec.push_back(matchPose[i * 7 + 6]);
//            //ui->plainTextEdit->appendPlainText(u8"叠料角度为：" + QString::number(matchPoseAngle[i]));//打印叠料角度
//            qDebug() << QString("叠料角度:");
//            qDebug() << QString::number(matchPoseAngle[i]);
//        }
//    }
//    //ui->plainTextEdit->appendPlainText(u8"叠料物料个数:" + QString::number(resPoseVec.size()/7));
//    qDebug() << QString("叠料物料个数:");
//    qDebug() << QString::number(resPoseVec.size()/7);
//    std::vector<double> outPose;
//    cp.poseSort(resPoseVec,u8"Z 从大到小",outPose);
//    for (int i=0;i<int(outPose.size());i++)
//    {
//        if(i%7==0)
//        {
//            //ui->plainTextEdit->appendPlainText(u8"第" + QString::number(i/7 + 1) + "个叠料位姿为：");
//            //qDebug()<<QString("第");
//            qDebug()<<QString::number(i/7 + 1);
//            qDebug()<<QString("个叠料位姿为");
//        }
//        //ui->plainTextEdit->appendPlainText(QString::number(outPose[i]));//打印叠料位姿
//        qDebug()<<QString(QString::number(outPose[i]));

//    }
//    //QVector<QString> posVector{"100","200","100","50","50","1"};//这是一个产品的6轴坐标
////    QString pos = QString::number(outPose[i]);
//    //emit posDetected(posVector);
}


QImage CameraController::cvMat2QImage(const cv::Mat &mat)
{
    if(mat.type() == CV_8UC3){
        const uchar* pSrc = (const uchar*)mat.data;
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
}

void CameraController::DisplayMat(cv::Mat &image)
{
    QImage qImage = cvMat2QImage(image);
    emit imageCaptured(qImage);
}
