#include "controller.h"
#include "detectionsystem.h"
#include "QsLog/QsLog.h"
#include <QVector>
#include <QTimer>
#include <QThread>
#include <cmath>
Controller::Controller(QObject *parent) : QObject(parent)
{

//    cameraThread = new QThread(this);
//    cameracontroller = new CameraController();
//    cameracontroller->moveToThread(cameraThread);
//    connect(cameraThread,&QThread::finished,cameracontroller,&QObject::deleteLater);
//    connect(this,&Controller::startCapture,cameracontroller,&CameraController::captureImage);
//    connect(cameracontroller,&CameraController::imageCaptured,this,&Controller::onImageCaptured);
//    connect(cameracontroller,&CameraController::posDetected,this,&Controller::onCloudDetected);
//    cameraThread->start();

    getSettings();
    // plc modbusTcp
    plcClient = new Modbusclient(this);
    plcClient->connectToServer(cfg.modbusIp,cfg.port);
    plcTimer = new QTimer(this);
    //plcTimer->start(1000);
    //3d signal
    tcpclient = new TcpClient(this);
    tcpclient->connectToServer("127.0.0.1", 2000);

    //3d result
    QObject::connect(tcpclient, &TcpClient::dataReceived, [this](const QString &data) {
            QLOG_DEBUG() << "Received data from 3Dr:" << data;
            //处理一下数据 转换为坐标  基座 铜件套模多一次交互过程 才获得坐标
//            if(cfg.detecType == 1 || cfg.detecType == 2){
//                //基座 铜件套模 直接转发
//                this->trans3DtoPlc(data);

//            }else{

//                if(data.toInt() == 1200){

//                    plcClient->writeRegister(4524,1);
//                    Sleep(1000);
//                    tcpclient->sendData("1000");
//                }else{

//                    this->trans3DtoPlc(data);
//                }
//            }
            if(data.toInt() == 1200){

                plcClient->writeRegister(4524,1);
                Sleep(500);
                tcpclient->sendData("1000");
            }else{

                this->trans3DtoPlc(data);
            }

        });





    //定时获取寄存器
    connect(plcTimer,&QTimer::timeout,this,&Controller::onPlcTimerOut);
    connect(plcClient,SIGNAL(readValue(int,int)),this,SLOT(onPlcRead(int,int)));
    connect(plcClient,SIGNAL(setZero()),this,SLOT(onZeroSet()));


    connect(DetectionSystem::getInstance(), &DetectionSystem::stateChanged, this, [](DetectionSystem::State newState) {
        if (newState == DetectionSystem::State::Detecting) {
            //qDebug() << "Controller:检测中...";
        } else {
            //qDebug() << "Controller:空闲";
        }
    });

    serialportDevice = new Serialport(this);
    QString com = QString("COM%1").arg(cfg.com);
    QLOG_DEBUG()<<"lesShine com:"<<com;
    serialportDevice->setPortName(com);
    serialportDevice->openSerialPort();


}

Controller::~Controller()
{

    //cameraThread->quit();
    //cameraThread->wait();
    serialportDevice->deleteLater();
    plcClient->deleteLater();
    plcTimer->deleteLater();
}



void Controller::onImageCaptured(const QImage &image)
{

    //qDebug()<<"Controller::onImageCaptured";

    emit imgCaptured(image);

    //TEST:拍照后给PLC测试数据
    //plcClient->writeRegister(4502,11);//检测结果OK
}

void Controller::onCloudDetected(QVector<QString> vec)
{
    //qDebug()<<"slot size:"<<vec.size();
    //从这里发给plc


}

void Controller::onZeroSet()
{
    plcClient->setUpdateStatus(true);
    QLOG_DEBUG()<<"zero set,continue reading...";
}

void Controller::getSettings()
{
    //获取配置文件

    //settings

        QString executableDir = QCoreApplication::applicationDirPath();


        QDir dir(executableDir);
        dir.cdUp();
        QString proDir = dir.absolutePath();
        QString path = proDir + "/files/settingsConf.ini";

    settings = new QSettings(path,QSettings::IniFormat);


    cfg.modbusIp = settings->value("Modbus/ip").toString();
    cfg.port = settings->value("Modbus/port").toInt();
    cfg.com = settings->value("Modbus/com").toInt();
    cfg.detecType = settings->value("Detect/type").toInt();
    cfg.modbusTimeout = settings->value("Detect/timeout").toInt();
    cfg.Xpos3D = settings->value("3DParamter/Xpos").toFloat();
    cfg.Ypos3D = settings->value("3DParamter/Ypos").toFloat();
    cfg.Xpos2D = settings->value("2DParamter/Xpos").toFloat();
    cfg.Ypos2D = settings->value("2DParamter/Ypos").toFloat();
    cfg.segment1_R1 = settings->value("3DParamter/segment1_R1").toFloat();
    cfg.segment2_R1 = settings->value("3DParamter/segment2_R1").toFloat();
    cfg.segment3_R1 = settings->value("3DParamter/segment3_R1").toFloat();
    cfg.segment4_R1 = settings->value("3DParamter/segment4_R1").toFloat();
    cfg.segment1_R1_2D = settings->value("2DParamter/segment1_R1").toFloat();
    cfg.segment2_R1_2D = settings->value("2DParamter/segment2_R1").toFloat();
    cfg.segment3_R1_2D = settings->value("2DParamter/segment3_R1").toFloat();
    cfg.segment4_R1_2D = settings->value("2DParamter/segment4_R1").toFloat();
    cfg.segment1_Offset = settings->value("3DParamter/segment1_Offset").toFloat();
    cfg.segment2_Offset = settings->value("3DParamter/segment2_Offset").toFloat();
    cfg.segment3_Offset = settings->value("3DParamter/segment3_Offset").toFloat();
    cfg.segment4_Offset = settings->value("3DParamter/segment4_Offset").toFloat();
    cfg.segment1_Offset_2D = settings->value("2DParamter/segment1_Offset").toFloat();
    cfg.segment2_Offset_2D = settings->value("2DParamter/segment2_Offset").toFloat();
    cfg.segment3_Offset_2D = settings->value("2DParamter/segment3_Offset").toFloat();
    cfg.segment4_Offset_2D = settings->value("2DParamter/segment4_Offset").toFloat();
    cfg.segmentR = settings->value("3DParamter/R").toFloat();
    cfg.offset_x = settings->value("3DParamter/X_Offset").toFloat();
    cfg.offset_y = settings->value("3DParamter/Y_Offset").toFloat();
    cfg.offset_z = settings->value("3DParamter/Z_Offset").toFloat();
    cfg.offset_c = settings->value("3DParamter/C_Offset").toFloat();
    cfg.LeadShineZeroPoint = settings->value("3DParamter/LeadShineZeroPoint").toInt();
    cfg.cameraIndex2D = settings->value("2DParamter/cameraIndex").toInt();
    cfg.offsetX_2d = settings->value("2DParamter/X_Offset").toFloat();
    cfg.offsetY_2d = settings->value("2DParamter/Y_Offset").toFloat();
    cfg.offsetZ_2d = settings->value("2DParamter/Z_Offset").toFloat();
    cfg.offsetC_2d = settings->value("2DParamter/C_Offset").toFloat();
    cfg.seg1xoffset = settings->value("2DParamter/seg1_xoffset").toFloat();
    cfg.seg1yoffset = settings->value("2DParamter/seg1_yoffset").toFloat();
    cfg.seg2xoffset = settings->value("2DParamter/seg2_xoffset").toFloat();
    cfg.seg2yoffset = settings->value("2DParamter/seg2_yoffset").toFloat();
    cfg.seg3xoffset = settings->value("2DParamter/seg3_xoffset").toFloat();
    cfg.seg3yoffset = settings->value("2DParamter/seg3_yoffset").toFloat();
    cfg.seg4xoffset = settings->value("2DParamter/seg4_xoffset").toFloat();
    cfg.seg4yoffset = settings->value("2DParamter/seg4_yoffset").toFloat();
    cfg.segmentR_2d = settings->value("2DParamter/R").toFloat();
    //cfg.originR = settings->value("2DParamter/OriginR").toFloat();
    cfg.x_range_min = settings->value("2DParamter/x_range_min").toFloat();
    cfg.x_range_max = settings->value("2DParamter/x_range_max").toFloat();
    cfg.y_range_min = settings->value("2DParamter/y_range_min").toFloat();
    cfg.y_range_max = settings->value("2DParamter/y_range_max").toFloat();
    cfg.matchSegment = settings->value("2DParamter/matchsegment").toInt();
    cfg.x_range_min_3D = settings->value("3DParamter/x_range_min").toFloat();
    cfg.x_range_max_3D = settings->value("3DParamter/x_range_max").toFloat();
    cfg.y_range_min_3D = settings->value("3DParamter/y_range_min").toFloat();
    cfg.y_range_max_3D = settings->value("3DParamter/y_range_max").toFloat();
    cfg.x_between_point_min = settings->value("3DParamter/x_between_point_min").toFloat();//x最小值与中心点差值
    cfg.y_between_point_min = settings->value("3DParamter/y_between_point_min").toFloat();//y最小值与中心点差值
    cfg.x_between_point_max = settings->value("3DParamter/x_between_point_max").toFloat();//x最大值与中心点差值
    cfg.y_between_point_max = settings->value("3DParamter/y_between_point_max").toFloat();//y最大值与中心点差值
    cfg.x_min_C_min = settings->value("3DParamter/x_min_C_min").toFloat();//x最小值c最小值
    cfg.x_max_C_min = settings->value("3DParamter/x_max_C_min").toFloat();//x最大值c最大值
    cfg.x_min_C_max = settings->value("3DParamter/x_min_C_max").toFloat();
    cfg.x_max_C_max = settings->value("3DParamter/x_max_C_max").toFloat();
    cfg.y_min_C_min = settings->value("3DParamter/y_min_C_min").toFloat();
    cfg.y_max_C_min = settings->value("3DParamter/y_max_C_min").toFloat();
    cfg.y_min_C_max = settings->value("3DParamter/y_min_C_max").toFloat();
    cfg.y_max_C_max = settings->value("3DParamter/y_max_C_max").toFloat();

    cfg.grey_minimum = settings->value("HalconParamter/grey_minimum").toInt();
    cfg.grey_lightErea = settings->value("HalconParamter/light_area").toInt();
    cfg.length = settings->value("HalconParamter/length").toInt();
    cfg.grayMin = settings->value("HalconParamter/grayMin").toInt();
    cfg.grayMax = settings->value("HalconParamter/grayMax").toInt();
    cfg.posZoom = settings->value("HalconParamter/posZoom").toInt();
    cfg.posRange = settings->value("HalconParamter/posRange").toInt();
}

int Controller::trans3DtoPlc(const QString &data)
{
    QLOG_DEBUG()<<"trans3D:"<<data;//x y z rx ry rz ,flag
    float x_offset,y_offset,z_offset;
    x_offset = cfg.offset_x;
    y_offset = cfg.offset_y;
    z_offset = cfg.offset_z;
    QStringList stringList = data.split(',');
    QVector<float> float_vec(8);
    QVector<int> int_vec(6);
    float_vec.clear();
    int_vec.clear();

    //x坐标 0  y坐标 0  z坐标 0
    if (stringList.size() >= 6) {
            QList<float> numbers;
            for (int i = 0; i < 6; ++i) {
                // 转换每个字符串为整数并添加到列表
                numbers.append(stringList[i].toFloat());
                //QLOG_DEBUG()<<"i:"<<i<<",val:"<<stringList[i].toFloat();
            }
                if(stringList[6].toInt() == 3){
                    //没有叠料 7
                    QLOG_DEBUG()<<"flag 7";
                    if(cfg.detecType != 2){

                        int_vec.push_back(0);//x 3100
                        int_vec.push_back(0);
                        int_vec.push_back(0);//y   3102
                        int_vec.push_back(0);
                        int_vec.push_back(0);//z  3104
                        int_vec.push_back(0);
                        int_vec.push_back(0);//C角度     3106
                        int_vec.push_back(0);
                        int_vec.push_back(0);//第五轴 3108
                        int_vec.push_back(0);
                        int_vec.push_back(0);//预留     3110
                        int_vec.push_back(0);
                        int_vec.push_back(7);//无叠料     3112
                        int_vec.push_back(0);
                        plcClient->writeMultiPleIntRegister(3100,int_vec);//3D合格

                    }else{
                        //铜件 无叠料 4520 写2
                          Controller::getInstance().plcSend(4520,2);
                    }
                }
                if(stringList[6].toInt() == 1)
                {
                    //叠料坐标
                    if(cfg.detecType == 1){

                        //基座3D全部报警
                        plcClient->addDataUnit(stringList.at(0).toDouble(),
                                               stringList.at(1).toDouble(),
                                               stringList.at(2).toDouble(),
                                               0,
                                               0,
                                               8,
                                               0);
                        plcClient->processQueue();
                        return 0;//后面不执行
                        //QLOG_DEBUG()<<" type 1 flag 2";
                        int MatchFlag = 8;//不可到达区域标记
                        //转换偏移
                        std::vector<double> posdata(8);
                        posdata.clear();

                        posdata.push_back(stringList.at(0).toDouble());
                        posdata.push_back(stringList.at(1).toDouble());
                        posdata.push_back(stringList.at(2).toDouble());
                        posdata.push_back(stringList.at(3).toDouble());
                        posdata.push_back(stringList.at(4).toDouble());
                        posdata.push_back(stringList.at(5).toDouble());
                        cp.poseTrans2RT(posdata,resRT);
                        //输出
                        QMatrix4x4 qMatrix(
                                resRT(0,0), resRT(0,1), resRT(0,2), resRT(0,3),
                                resRT(1,0), resRT(1,1), resRT(1,2), resRT(1,3),
                                resRT(2,0), resRT(2,1), resRT(2,2), resRT(2,3),
                                resRT(3,0), resRT(3,1), resRT(3,2), resRT(3,3)
                            );
                        QLOG_DEBUG()<<"3d martrix data:"<<qMatrix;

                        // standard  0,0,1
                        //double cc=-sin(Htheta) * len;
                        //double zz=qAbs(len-len*cos(Htheta));
                            float r33 = qMatrix(2, 2);
                            double cos_theta = r33;

                            // 使用反余弦函数计算Z夹角
                            double theta = std::acos(cos_theta);
                            // 电机
                            double theta_degrees = 180.0 - theta * (180.0 / M_PI);
                            double Htheta=M_PI-theta;

                            float r11 = qMatrix(0, 0);
                            double cos_thetax = r11;

                                // 使用反余弦函数计算X夹角
                            double thetax = std::acos(cos_thetax);

                                // 将角度从弧度转换为度
                            double theta_degreesx = 180.0 -thetax * (180.0 / M_PI);
                            double Mtheta = M_PI - theta;
                            //float r22 = qMatrix(1,1);
                            //double cos_thetay = r22;
                            //double thetay = std::acos(cos_thetay);
                            //double theta_degreesy = 180.0 - thetay * (180.0 / M_PI);

                            //QLOG_DEBUG()<< QStringLiteral("Z轴旋转角度X-axis:   绕Z夹角") << theta_degreesx << " degrees" ;
                            //QLOG_DEBUG()<< QStringLiteral("电机旋转角度Z-axis:  绕X轴夹角: ") << theta_degrees  << " degrees" ;
                            //QLOG_DEBUG()<< QStringLiteral("电机旋转角度Y-axis:  绕Y轴夹角: ") << theta_degreesy  << " degrees" ;


                           QVector3D zAxisDirection(resRT(0,2), resRT(1,2), resRT(2,2));
                            // 输出法向量
                            //QLOG_DEBUG() << QStringLiteral("法向量") << zAxisDirection;

                        //电机夹爪长度
                        int len = cfg.length;
                        //x偏移 绕Z轴的旋转角

                        //double x = numbers.at(0) + cos(numbers.at(5))* sin(theta) * len;

//                        double y = numbers.at(1) + sin(thetax)* sin(theta) * len;


//                        double z = numbers.at(2)  - qAbs(len-len*cos(Htheta));
                        //看一下补偿值
//                        double x_caculate,y_caculate,z_caculate;
//                        x_caculate = cos(numbers.at(5))* sin(theta) * len;
//                        y_caculate = sin(thetax)* sin(theta) * len;
//                        z_caculate = qAbs(len-len*cos(Htheta));
//                        double x_alternative = cos(numbers.at(5))* sin(thetax) * len;

                        float r1,r2;//r1是3D相机发送的角度  R2是根据四个象限位置 计算出的角度
                        float x,y,z;
                        //QLOG_DEBUG()<<"3D add 1 unit";
                        //QLOG_DEBUG()<<QStringLiteral("给角度")<< numbers.at(5)+cfg.offset_c;
                        if(resRT(1,2)<0)
                        {
                            QLOG_DEBUG()<<QStringLiteral("符合电机旋转方向,RT(1,2) < 0");


                            //double y = numbers.at(1) +(sin(Htheta) * len);
//                            x = numbers.at(0) + (sin(thetax) * len);
//                            y = numbers.at(1) +(sin(Htheta) * len);
                            //cos(numbers.at(5))* sin(theta) * len

                            double aa_2 = cos(numbers.at(5))*sin(Htheta) * len;//x补偿2
                            double cc=sin(Htheta) * len;//Y补偿
                            double zz=qAbs(len-len*cos(Htheta));//Z补偿  实际上是Mtheta
                            //QLOG_DEBUG()<<QStringLiteral("初始值：   x:")<<numbers.at(0)<<",y:"<<numbers.at(1)<<",z:"<<numbers.at(2);
//                            QLOG_DEBUG()<<QStringLiteral("X初始值：   ")<<numbers.at(0)<<QStringLiteral( "补偿值：   ")<<aa;
//                            QLOG_DEBUG()<<QStringLiteral("X初始值：   ")<<numbers.at(0)<<QStringLiteral( "补偿值_2：   ")<<aa_2;
//                            QLOG_DEBUG()<<QStringLiteral("Y初始值：   ")<<numbers.at(1)<<QStringLiteral( "补偿值：   ")<<cc;
//                            QLOG_DEBUG()<<QStringLiteral("z初始值：   ")<<numbers.at(2)<<QStringLiteral( "补偿值：   ")<<zz;
                              //QLOG_DEBUG()<<QStringLiteral("X补偿值_1：   ")<<aa;
                             // QLOG_DEBUG()<<QStringLiteral("X补偿值_2：   ")<<aa_2;
                              //QLOG_DEBUG()<<QStringLiteral("Y补偿值 ：   ")<<cc;
                              //QLOG_DEBUG()<<QStringLiteral("Z补偿值 ：   ")<<zz;
                               //x = numbers.at(0) + aa_2;
                               //y = numbers.at(1) + cc;
                               //z = numbers.at(2)  - zz;


                               //重新计算偏移
                               QVector3D normal_world(
                                   qMatrix(0,2), //nx
                                   qMatrix(1,2), //ny
                                   qMatrix(2,2)  //nz
                               );
                               QVector3D normal = normal_world;
                               //取反
                               QVector3D normal_opposite = -normal;
                               //QLOG_DEBUG() << QStringLiteral("法向量") << normal;
                              // QLOG_DEBUG() << QStringLiteral("取反后的法向量") << normal_opposite;
                               QVector3D offset = normal_opposite * len;
                               float x_offset_vec = offset.x();
                               float y_offset_vec = offset.y();
                               //float z_offset_vec = offset.z();
                               float z_offset_vec = qAbs(len-len*cos(Mtheta));//Z补偿
                               QLOG_DEBUG()<<"length:"<<len;
                               QLOG_DEBUG()<<QStringLiteral("初始值：   x:")<<numbers.at(0)<<",y:"<<numbers.at(1)<<",z:"<<numbers.at(2);
                               QLOG_DEBUG()<<QStringLiteral("偏移值：   x:")<<x_offset_vec<<",y_offset:"<<y_offset_vec<<",z_offset:"<<z_offset_vec;
                               //如果resRT(0,2)>0 产品X旋转  偏向X轴负向 说明X需要向X负向进行补偿
                               if(resRT(0,2) >  0){
                                  x_offset_vec = 0 - x_offset_vec;
                               }
                               x = numbers.at(0) + x_offset_vec;
                               y = numbers.at(1) + y_offset_vec;
                               z = numbers.at(2) - z_offset_vec;
//                               x = numbers.at(0) ;
//                               y = numbers.at(1) ;
//                               z = numbers.at(2) ;
                               QLOG_DEBUG()<<QStringLiteral("实际值：   x:")<<x<<",y:"<<y<<",z:"<<z;
                            if(150<=numbers.at(5) && numbers.at(5)<=200)
                            {
                                //QLOG_DEBUG()<<"绕Z轴旋转角度:"<<numbers.at(5);
                                 //QLOG_DEBUG()<<"case 1, numbers.at(5)rz =     "<<numbers.at(5);

                                 //r1 r2的处理
                                 if(numbers.at(5)> 180){
                                            // r1  get     r2 . set
                                         //原始rz角度大于180度
                                      r1 = 0 - (numbers.at(5) - 360 )+ cfg.offset_c;
                                      r2 = cfg.offset_c;

                                 }else{
                                         //原始rz小于180  r1取料 r2放料
                                     r1 = 0 - (numbers.at(5)) + cfg.offset_c;
                                     r2 =  cfg.offset_c;
                                 }






                                 if((cfg.x_range_min_3D < x && cfg.x_range_max_3D) && (cfg.y_range_min_3D < y && y < cfg.y_range_max_3D)){
                                     //在可到达区域内
                                     //并且电机在可旋转范围内 leisai 雷赛 编码器   发送脉冲信号，
                                     if(theta_degrees *  cfg.posZoom < cfg.posRange){
                                         //满足条件 电机可以旋转
                                            QLOG_DEBUG()<<"led pos fit,match flag 2";
                                            MatchFlag = 2;
                                            setLedShinePos(theta_degrees*cfg.posZoom,4);
                                            QLOG_DEBUG()<<"theta_degrees:"<<theta_degrees<<",posZoom:"<<cfg.posZoom;
                                     }else{
                                         QLOG_DEBUG()<<"LED posZoom:"<<cfg.posZoom<<",X axis degree:"<<theta_degrees;
                                         QLOG_DEBUG()<<"LED pos out of range:"<<cfg.posRange;
                                         MatchFlag = 8;
                                     }
                                     plcClient->addDataUnit(x+x_offset,
                                                            y+y_offset,
                                                            z+z_offset,
                                                            r1,
                                                            r2,
                                                            MatchFlag,
                                                            0);

                                     plcClient->processQueue();
                                 }else{
                                     //不可到达
                                     QLOG_DEBUG()<<"rz > 150 ,X,Y out of range";
                                     plcClient->addDataUnit(x+x_offset,
                                                            y+y_offset,
                                                            z+z_offset,
                                                            r1,
                                                            r2,
                                                            8,
                                                            theta_degrees);
                                     //setLedShinePos(theta_degrees*900,8);
                                     plcClient->processQueue();
                                 }


                            }

                           else
                                {
                                    //QLOG_DEBUG()<<"绕Z轴旋转角度:"<<numbers.at(5);
                                    QLOG_DEBUG()<<"1 numbers.at(5) :=     "<<numbers.at(5);

                                 //r1 r2的处理
                                 if(numbers.at(5)> 180){

                                         //原始rz角度大于180度
                                      r1 = 0 - (numbers.at(5) - 360 )+ cfg.offset_c;
                                      r2 = cfg.offset_c;

                                 }else{
                                         //原始rz小于180  r1取料 r2放料
                                     r1 = 0 - (numbers.at(5)) + cfg.offset_c;
                                     r2 = cfg.offset_c;
                                 }



                                 if((cfg.x_range_min_3D < x && cfg.x_range_max_3D) && (cfg.y_range_min_3D < y && y < cfg.y_range_max_3D)){
                                     //在可到达区域内
                                     //并且电机在可旋转范围内 0-  -14000
                                     if(theta_degrees * cfg.posZoom < cfg.posRange){
                                         //满足条件 电机可以旋转
                                            MatchFlag = 2;
                                            setLedShinePos(theta_degrees*cfg.posZoom,4);
                                            //QLOG_DEBUG()<<"led pos zoom:"<<cfg.posZoom;
                                            QLOG_DEBUG()<<"theta_degrees:"<<theta_degrees<<",posZoom:"<<cfg.posZoom;
                                     }else{
                                         QLOG_DEBUG()<<"LED posZoom:"<<cfg.posZoom<<",X axis degree:"<<theta_degrees;
                                         QLOG_DEBUG()<<"LED pos out of range:"<<cfg.posRange;
                                         MatchFlag = 8;
                                     }
                                     plcClient->addDataUnit(x+x_offset,
                                                            y+y_offset,
                                                            z+z_offset,
                                                            r1,
                                                            r2,
                                                            MatchFlag,
                                                            theta_degrees);

                                     plcClient->processQueue();
                                 }else{
                                     //不可到达
                                     QLOG_DEBUG()<<"rz < 150,X,Y out of range";
                                     plcClient->addDataUnit(x+x_offset,
                                                            y+y_offset,
                                                            z+z_offset,
                                                            r1,
                                                            r2,
                                                            8,
                                                            theta_degrees);
                                     //setLedShinePos(theta_degrees*900,8);
                                     plcClient->processQueue();
                                 }

                                }
                        }


                        else if(resRT(1,2)>0)
                        {
                              QLOG_DEBUG()<<QStringLiteral("符合电机旋转方向,RT(1,2) > 0");
//                            double aa_2 = -cos(numbers.at(5))*sin(Htheta) * len;//x补偿2
//                            double cc=-sin(Htheta) * len;//Y补偿
//                            double zz=qAbs(len-len*cos(Htheta));//Z补偿



                            //重新计算偏移
                            QVector3D normal_world(
                                qMatrix(0,2), //nx
                                qMatrix(1,2), //ny
                                qMatrix(2,2)  //nz
                            );
                            QVector3D normal = normal_world;
                            //取反
                            QVector3D normal_opposite = -normal;
                            //QLOG_DEBUG() << QStringLiteral("法向量") << normal;
                            //QLOG_DEBUG() << QStringLiteral("取反后的法向量") << normal_opposite;
                            QVector3D offset = normal_opposite * len;
                            float x_offset_vec = offset.x();
                            float y_offset_vec = offset.y();
                            //float z_offset_vec = offset.z();
                            float z_offset_vec = qAbs(len-len*cos(Mtheta));//Z补偿
                            QLOG_DEBUG()<<"length:"<<len;
                            QLOG_DEBUG()<<"GET x_offset:"<<x_offset_vec<<",y_offset:"<<y_offset_vec<<",z_offset:"<<z_offset_vec;
                            //如果resRT(0,2)>0 产品X旋转  偏向X轴负向 说明X需要向X负向进行补偿
                            if(resRT(0,2) >  0){
                               x_offset_vec = 0 - x_offset_vec;
                            }
                            x = numbers.at(0) + x_offset_vec;
                            y = numbers.at(1) + y_offset_vec;//y朝向Y负向  Y补偿朝向Y负
                            z = numbers.at(2) - z_offset_vec;

//                            x = numbers.at(0) ;
//                            y = numbers.at(1) ;
//                            z = numbers.at(2) ;
                            QLOG_DEBUG()<<"TAKE offset,x:"<<x<<",y:"<<y<<",z:"<<z;
                            if(150<=numbers.at(5)&& numbers.at(5)<=200)
                            {
                                 QLOG_DEBUG()<<"RT(1,2) >0 and  150 < rz < 200, rz: "<<numbers.at(5);
                                 //r1 r2的处理
                                 if(numbers.at(5)> 180){

                                         //原始rz角度大于180度
                                      r1 = 0 - (numbers.at(5) - 360 )+ cfg.offset_c;
                                      r2 =  cfg.offset_c;

                                 }else{
                                         //原始rz小于180  r1取料 r2放料
                                     r1 = 0 - (numbers.at(5)) + cfg.offset_c;
                                     r2 =  cfg.offset_c;
                                 }


                                 if((cfg.x_range_min_3D < x && cfg.x_range_max_3D) && (cfg.y_range_min_3D < y && y < cfg.y_range_max_3D)){
                                     //在可到达区域内
                                     //并且电机在可旋转范围内
                                     if(theta_degrees *  cfg.posZoom < cfg.posRange){
                                         //满足条件 电机可以旋转
                                            QLOG_DEBUG()<<"led pos fit,match flag 2";
                                            MatchFlag = 2;
                                            setLedShinePos(theta_degrees*cfg.posZoom,4);
                                            QLOG_DEBUG()<<"theta_degrees:"<<theta_degrees<<",posZoom:"<<cfg.posZoom;
                                     }else{
                                         QLOG_DEBUG()<<"LED posZoom:"<<cfg.posZoom<<",X axis degree:"<<theta_degrees;
                                         QLOG_DEBUG()<<"LED pos out of range:"<<cfg.posRange;
                                         MatchFlag = 8;
                                     }
                                     plcClient->addDataUnit(x+x_offset,
                                                            y+y_offset,
                                                            z+z_offset,
                                                            r1,
                                                            r2,
                                                            MatchFlag,
                                                            theta_degrees);

                                     plcClient->processQueue();
                                 }else{
                                     //不可到达
                                     QLOG_DEBUG()<<"rz > 150 ,X,Y out of range";
                                     plcClient->addDataUnit(x+x_offset,
                                                            y+y_offset,
                                                            z+z_offset,
                                                            r1,
                                                            r2,
                                                            8,
                                                            theta_degrees);
                                     //setLedShinePos(theta_degrees*900,8);
                                     plcClient->processQueue();
                                 }

                            }

                           else
                                {
                                 QLOG_DEBUG()<<"RT(1,2) >0 and  rz < 150 or rz > 200,rz:"<<numbers.at(5);
                                 //r1 r2的处理
                                 if(numbers.at(5)> 180){

                                         //原始rz角度大于180度
                                      r1 = 0 - (numbers.at(5) - 360 )+ cfg.offset_c;
                                      r2 = cfg.offset_c;

                                 }else{
                                         //原始rz小于180  r1取料 r2放料
                                     r1 = 0 - (numbers.at(5)) + cfg.offset_c;
                                     r2 = cfg.offset_c;
                                 }




                                 if((cfg.x_range_min_3D < x && cfg.x_range_max_3D) && (cfg.y_range_min_3D < y && y < cfg.y_range_max_3D)){
                                     //在可到达区域内
                                     //并且电机在可旋转范围内 0-  -14000
                                     if(theta_degrees * cfg.posZoom < cfg.posRange){
                                         //满足条件 电机可以旋转
                                            MatchFlag = 2;
                                            setLedShinePos(theta_degrees*cfg.posZoom,4);
                                            QLOG_DEBUG()<<"theta_degrees:"<<theta_degrees<<",posZoom:"<<cfg.posZoom;
                                     }else{
                                         QLOG_DEBUG()<<"LED posZoom:"<<cfg.posZoom<<",X axis degree:"<<theta_degrees;
                                         QLOG_DEBUG()<<"LED pos out of range:"<<cfg.posRange;
                                         MatchFlag = 8;
                                     }
                                     plcClient->addDataUnit(x+x_offset,
                                                            y+y_offset,
                                                            z+z_offset,
                                                            r1,
                                                            r2,
                                                            MatchFlag,
                                                            theta_degrees);

                                     plcClient->processQueue();
                                 }else{
                                     //不可到达
                                     QLOG_DEBUG()<<"rz < 150,X,Y out of range";
                                     plcClient->addDataUnit(x+x_offset,
                                                            y+y_offset,
                                                            z+z_offset,
                                                            r1,
                                                            r2,
                                                            8,
                                                            theta_degrees);
                                     //setLedShinePos(theta_degrees*900,8);
                                     plcClient->processQueue();
                                 }

                                }
                        }


                    }
                    if(cfg.detecType == 3 || cfg.detecType == 4){

                        //QLOG_DEBUG()<<"flag 2";


                        //转换偏移
                        //
                        int len = cfg.length;

                        double x = numbers.at(0) + cos(numbers.at(5))* sin(numbers.at(4)) * len;
                        double y = numbers.at(1) + sin(numbers.at(5))* sin(numbers.at(4)) * len;
                        double z = numbers.at(2) - len + cos(numbers.at(4)) * len;
                        QLOG_DEBUG()<<QStringLiteral("计算补偿后,x:")<<x<<",y:"<<y<<",z:"<<z;
                        float r1,r2;//r1是3D相机发送的角度  R2是根据四个象限位置 计算出的角度
                        //float x_pos = x;
                        //float y_pos = y;
                        //3D放弃使用象限计算C角度
                        //angelProcess(x_pos,y_pos,int(numbers.at(5)),r1,r2,1);//3D



                        //QLOG_DEBUG()<<"3D add 1 unit";
                        if(numbers.at(5)> 180){

                                //原始rz角度大于180度
                             r1 = 0 - (numbers.at(5) - 360 )+ cfg.offset_c;
                             r2 = 90 + cfg.offset_c;

                        }else{
                                //原始rz小于180  r1取料 r2放料
                            r1 = 0 - (numbers.at(5)) + cfg.offset_c;
                            r2 = 90 + cfg.offset_c;
                        }



                        //QLOG_DEBUG()<<"rz:"<<numbers.at(5)<<",offset_c:"<<cfg.offset_c<<",r1:"<<r1<<",r2:"<<r2;
//                        if((-360 > r1) || (r1 > 360)){
//                            r1 = fmod(r1 + 360.0, 720.0);
//                            if(r1 > 360){
//                                r1 -= 720;
//                            }
//                            QLOG_DEBUG()<<"r1 not fit,convert result:"<<r1;
//                        }
//                        if((-360 > r2) || (r2 > 360)){
//                            r2 = fmod(r2 + 360.0, 720.0);
//                            if(r2 > 360){
//                                r2 -= 720;
//                            }
//                            QLOG_DEBUG()<<"r2 not fit,convert result:"<<r2;
//                        }


                        //判断结果是否可执行
                        //x与中心点差值  y与中心点差值
                        //QLOG_DEBUG()<<"xpos:"<<cfg.Xpos3D<<",ypos:"<<cfg.Ypos3D;
                        //QLOG_DEBUG()<<"x_between_min:"<<cfg.x_between_point_min<<",x_between_max:"<<cfg.x_between_point_max;
                        //QLOG_DEBUG()<<"y_between_min:"<<cfg.y_between_point_min<<",y_between_max:"<<cfg.y_between_point_max;
                        //QLOG_DEBUG()<<"x:"<<x<<"y:"<<y;
                        int matchFlag = 2;

//
                        //首先看x
                        if(cfg.Xpos3D - cfg.x_between_point_min > x) {
                               //x太小
                                //看角度
                                if(cfg.x_min_C_min < r1 && r1 < cfg.x_min_C_max){
                                       //不报警
                                 }else{
                                        //报警
                                        matchFlag = 8;
                                        QLOG_DEBUG()<<"case 1.alarm";
                                    }
                        }else if(cfg.Xpos3D + cfg.x_between_point_min < x) {
                            //x太大
                             //看角度
                             if(cfg.x_max_C_min < r1 && r1 < cfg.x_max_C_max){
                                    //不报警
                              }else{
                                     //报警
                                    matchFlag = 8;
                                    QLOG_DEBUG()<<"case 2.alarm";
                                 }
                         }else if(cfg.Ypos3D - cfg.y_between_point_min > y) {
                                   //x太小
                                    //看角度
                                    if(cfg.y_min_C_min < r1 && r1 < cfg.y_min_C_max){
                                           //不报警
                                     }else{
                                            //报警
                                            matchFlag = 8;
                                            QLOG_DEBUG()<<"case 3.alarm";
                                        }
                          }else if(cfg.Ypos3D +  cfg.y_between_point_min <  y) {
                                      //x太小
                                       //看角度
                                       if(cfg.y_max_C_min < r1 && r1 < cfg.y_max_C_max){
                                              //不报警
                                        }else{
                                               //报警
                                                matchFlag = 8;
                                                QLOG_DEBUG()<<"case 4.alarm";
                                        }
                          }else {
                            //不报警
                          }
                        plcClient->addDataUnit(x+x_offset,y+y_offset,z+z_offset,r1,r2,matchFlag);
                        plcClient->processQueue();
//


                    }else{
                        //铜件直接发结果
                        //Controller::getInstance().plcSend(4520,1);
                        //基座3D全部报警
                        plcClient->addDataUnit(stringList.at(0).toDouble(),
                                               stringList.at(1).toDouble(),
                                               stringList.at(2).toDouble(),
                                               0,
                                               0,
                                               8,
                                               0);
                        plcClient->processQueue();
                        return 0;//后面不执行

                    }


                }
                if(stringList[6].toInt() == 5){
                    //PLC报警
                    Controller::getInstance().plcSend(4520,89);
                }

        }
    return 0;
}

void Controller::onHalconDetected(float x, float y,float z, float c,int flag)
{
    //拿到坐标信息 放入缓存模块
    //转换偏移
    QLOG_DEBUG()<<"onHalconDetected,x:"<<x<<", y:"<<y<<",z :"<<z<<",c :"<<c<<",flag :"<<flag;
    float r1,r2;//r1是3D相机发送的角度  R2是根据四个象限位置 计算出的角度
    angelProcess(x,y,c,r1,r2,2);//type1  3D角度计算  type2 2D角度计算
    if((-360 > r2) || (r2 > 360)){
        r2 = fmod(r2 + 360.0, 720.0);
        if(r2 > 360){
           r2 -= 720;
            }
          QLOG_DEBUG()<<"2D r2 not fit,convert result:"<<r2;
         }
    //如果旋转角度在180-360之间  负向运动一圈  变为-180 到 0 度
    if(180 < r2 && r2  < 360){
        r2 -= 360;
    }

    plcClient->addDataUnit(x,y,z,r1,r2,flag);



}

void Controller::clearHalconQueue()
{
    plcClient->clearData();
}

void Controller::newImgResultPop1()
{
    QLOG_DEBUG()<<"new img result array,pop 1";
    plcClient->processQueue();
}

void Controller::handPopResult()
{
    QLOG_DEBUG()<<"trans auto,pop 1";
    plcClient->processQueue();
}

QVector<uint8_t> Controller::convertToHex(int32_t number, size_t byteSize)
{
    //一个4字节整数 转为4个 uint8 十六进制
    QVector<uint8_t> result(byteSize,0);
    uint32_t unsignedNumber = static_cast<uint32_t>(number);
    if(number < 0){
        unsignedNumber = static_cast<uint32_t>(number + (1LL << (byteSize * 8)));
    }

    for(size_t i = 0; i < byteSize; ++i){
        result[byteSize - 1 - i] = static_cast<uint8_t>((unsignedNumber >> (8 * i)) & 0xFF);
    }
    return result;
}

void Controller::setLedShinePos(int32_t val, size_t byteSize)
{

    if(val > 14000){
        QLOG_DEBUG()<<"set pos out of range 14000,val:"<<val;
        return;
    }
    //与电机零点做偏移运算
    int32_t reverstVal = 0 - val;
    //int32_t zeroPoint = cfg.LeadShineZeroPoint;
    int32_t retStep;
    //retStep = reverstVal + zeroPoint;


    retStep = reverstVal;
    QLOG_DEBUG()<<"set led step pos:"<<retStep;
    QVector<uint8_t> res(byteSize,0);
    res = convertToHex(retStep,byteSize);
        //高位2个数 设置到6209寄存器
    QLOG_DEBUG()<<"HIGH val in 6209:"<<res.at(0);
    QLOG_DEBUG()<<"LOW val in 620A:"<<res.at(2);
    serialportDevice->writeStepPosData(0x01,0x6209,res.at(0),res.at(1));
        //低位2个数 放到620A
    Sleep(100);
    serialportDevice->writeStepPosData(0x01,0x620A,res.at(2),res.at(3));

    Sleep(100);
        //触发pr1运行
    serialportDevice->writeModbusData(0x01,0x6002,0x11);

}



void Controller::onPlcRead(int addr, int value)
{
    //QLOG_DEBUG()<<"read addr:"<<addr<<",value:"<<value;
    if(value != 0 && !lastTriggerState){

         plcTimer->stop();//停止轮询 先处理当前流程
         plcClient->setUpdateStatus(false);

         //QLOG_DEBUG()<<"processing,stop reading...";
         plcClient->writeRegister(addr,0);
         lastTriggerState = true;
         QLOG_DEBUG()<<"plc read value ,lastTriggerState: "<<lastTriggerState;


             switch(addr)
             {

             case 4526:

                 if(value == 1){
                     QLOG_DEBUG()<<"4526 read 1,capture 3D image";
                     int type = Controller::getInstance().cfg.detecType;
                     QLOG_DEBUG()<<" 3D detect type:"<<type;
//                     if(type ==1 || type == 2){
//                         //基座 铜件直接发1000

//                         tcpclient->sendData("1000");//叠拍照
//                     }else{
//                         //面板 安装架
//                         tcpclient->sendData("1300");//叠拍照
//                     }
                      if(type == 2){
                          tcpclient->sendData("1000");
                      }else{
                          tcpclient->sendData("1300");//叠拍照
                      }



                    

                 }else if(value == 2){
                     QLOG_DEBUG()<<"4526 read 2,capture 2D image";

                     //2D需要反料数据  看一下队列容量
                     if(plcClient->QueueNotEmpty()){

                         QLOG_DEBUG()<<"2D capture queue remains...pop 1";
                         plcClient->processQueue();
                     }
                     else{
                         emit detectRemains(0);
                         emit start2DCapture();

                     }
                 }else if(value == 3){
                     //残余料

                     if(plcClient->QueueNotEmpty()){

                         QLOG_DEBUG()<<"2D capture queue remains...pop 1";
                         plcClient->processQueue();
                     }
                     else{
                         emit detectRemains(1);
                         emit start2DCapture();

                     }

                 }else if(value == 10){
                     //第五轴回零
                     QLOG_DEBUG()<<"4526 read 10,serialport zero";
                     onPosZero();
                     plcClient->writeRegister(4522,2);
                 }else if(value == 13){
                     //第五轴回零
                     QLOG_DEBUG()<<"4526 read 13,test reading";

                 }
                 break;
             case 4530://铜件信号
                 if(cfg.detecType == 2){

                     emit start2DCapture();
                 }

                 break;



             }





    }else if(value == 0){
        lastTriggerState = false;
        QLOG_DEBUG()<<"plc read value  = 0,lastTriggerState: "<<lastTriggerState;

    }
    lastTriggerState = false;

    int timer = cfg.modbusTimeout;
    plcTimer->start(timer);

}

void Controller::onPlcTimerOut()
{
    //QLOG_DEBUG()<<"reading plc data";

    //
    if(DetectionSystem::getInstance()->getCurrentState() == DetectionSystem::State::Detecting){
        //QLOG_DEBUG()<<"reading plc data";

        plcClient->readRegisters(4526,5);


    }

}

void Controller::onPlcProcesing()
{
    QLOG_DEBUG()<<"procesing plc data";
    Sleep(3000);
    //plcTimer->start();
}

void Controller::onTimerStart()
{
    int timer = cfg.modbusTimeout;
    plcTimer->start(timer);

}

void Controller::onTimerStop()
{
    plcTimer->stop();

}

//脉冲=角度*1000
void Controller::onPosSet(int step)
{
    QLOG_DEBUG()<<"Controller::onPosSet:"<<step;
    QString hex_str = QString::number(step,16);
    bool ok;
    int ret = hex_str.toInt(&ok,16);
    if(ok){
        serialportDevice->writeModbusData(1,0x620A,ret);//设置pr1 位置
        Sleep(50);
        serialportDevice->writeModbusData(1,0x6002,0x11);//触发pr1
    }

}

void Controller::onPosZero()
{
    QLOG_DEBUG()<<"Controller::on pos zero:";
    serialportDevice->writeModbusData(1,0x6002,0x10);//触发pr0
}

void Controller::onPosJOG()
{
    QLOG_DEBUG()<<"Controller::on pos zero:";
    serialportDevice->writeModbusData(1,0x1801,0x4002);
}



