#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "cameracontroller.h"
#include <QThread>
#include <QMutex>
#include <QTimer>
#include "visionalgorithm.h"
#include "modbusclient.h"
#include "tcpserver.h"
#include "serialport.h"
#include "tcpclient.h"
#include "CloudProcess.h"
#include <QCoreApplication>
#include <QSettings>
#include <QMatrix4x4>

struct Config {
    QString modbusIp;
    int port;
    int detecType;
    int modbusTimeout;
    int com;
    float Xpos3D;
    float Ypos3D;
    float Xpos2D;
    float Ypos2D;
    float segment1_R1;
    float segment2_R1;
    float segment3_R1;
    float segment4_R1;
    float segment1_Offset;
    float segment2_Offset;
    float segment3_Offset;
    float segment4_Offset;
    float segment1_R1_2D;
    float segment2_R1_2D;
    float segment3_R1_2D;
    float segment4_R1_2D;
    float segment1_Offset_2D;
    float segment2_Offset_2D;
    float segment3_Offset_2D;
    float segment4_Offset_2D;
    float segmentR;
    int LeadShineZeroPoint;
    float offset_x;
    float offset_y;
    float offset_z;
    float offset_c;
    int cameraIndex2D;
    float offsetX_2d;
    float offsetY_2d;
    float offsetZ_2d;
    float offsetC_2d;
    float seg1xoffset;
    float seg1yoffset;
    float seg2xoffset;
    float seg2yoffset;
    float seg3xoffset;
    float seg3yoffset;
    float seg4xoffset;
    float seg4yoffset;
    float segmentR_2d;
    float originR;
    float x_range_min;
    float x_range_max;
    float y_range_min;
    float y_range_max;
    float x_range_min_3D;
    float x_range_max_3D;
    float y_range_min_3D;
    float y_range_max_3D;
    float x_between_point_min;//x与料盘中心点差值
    float y_between_point_min;//y与料盘中线点差值
    float x_between_point_max;//x与料盘中心点差值
    float y_between_point_max;//y与料盘中线点差值
    float x_min_C_min;
    float x_min_C_max;
    float x_max_C_min;
    float x_max_C_max;
    float y_min_C_min;
    float y_min_C_max;
    float y_max_C_min;
    float y_max_C_max;
    int matchSegment;
    int grey_minimum;
    int grey_lightErea;
    int length;
    int grayMin;
    int grayMax;
    int posZoom;
    int posRange;
};
class Controller : public QObject
{
    Q_OBJECT
public:
    static Controller& getInstance(){
        static Controller instance;//unique instance
        return instance;
    }

    Controller(const Controller&) = delete;
    Controller& operator=(const Controller&) = delete;
    void getSettings();
    int trans3DtoPlc(const QString &data);

    CameraController* cameracontroller;
    void onHalconDetected(float,float,float,float,int);
    void clearHalconQueue();
    void newImgResultPop1();
    void handPopResult();
    QVector<uint8_t> convertToHex(int32_t number, size_t byteSize);

    void setLedShinePos(int32_t val, size_t byteSize);
    void testMultiPleSend();
    void onPosSet(int step);
    void onPosZero();
    void onPosJOG();
    void  angelProcess(float x ,float y,float r,float& r1,float& r2,int type){
        //R 3D结果中的第六轴 R1 机器人固定数值 计算得到R2
        //R1写入 3110
        //R2写入 3106
        //Type1 3D  type2  2D
        QLOG_DEBUG()<<"angelProcess:,X:"<<x<<", y:"<<y<<", r:"<<r<<", type:"<<type;
        float cfg_x;
        float cfg_y;
        float cfg_c;
        float segmentR;
        float origin_r;
        float r1offset;
        float r2offset;
        float r3offset;
        float r4offset;
        float segment_r1;
        float segment_r2;
        float segment_r3;
        float segment_r4;
        if(type == 1){
            QLOG_DEBUG()<<"angelProcess,type 3D";
            cfg_x = cfg.Xpos3D;
            cfg_y = cfg.Ypos3D;
            cfg_c = cfg.offset_c;
            segmentR = cfg.segmentR;
            r1offset = cfg.segment1_Offset;
            r2offset = cfg.segment2_Offset;
            r3offset = cfg.segment3_Offset;
            r4offset = cfg.segment4_Offset;
            segment_r1 = cfg.segment1_R1;
            segment_r2 = cfg.segment2_R1;
            segment_r3 = cfg.segment3_R1;
            segment_r4 = cfg.segment4_R1;
        }else{
            //2D偏移
            QLOG_DEBUG()<<"angelProcess,type 2D";


            cfg_x = cfg.Xpos2D;
            cfg_y = cfg.Ypos2D;
            cfg_c = cfg.offsetC_2d;
            segmentR = cfg.segmentR_2d;
            origin_r = cfg.originR;
            //QLOG_DEBUG()<<"origin_r:"<<r;

            //r = origin_r;


            segment_r1 = cfg.segment1_R1_2D;
            segment_r2 = cfg.segment2_R1_2D;
            segment_r3 = cfg.segment3_R1_2D;
            segment_r4 = cfg.segment4_R1_2D;
            r1offset = cfg.segment1_Offset_2D;
            r2offset = cfg.segment2_Offset_2D;
            r3offset = cfg.segment3_Offset_2D;
            r4offset = cfg.segment4_Offset_2D;
        }
        int seg;//象限

        //如果有是目标区域配置 改为指定象限的偏移计算
//        if(cfg.matchSegment != 0){
//            if((cfg.x_range_min < x && x < cfg.x_range_max) && (cfg.y_range_min < y && y < cfg.y_range_max)){
//                //目标区域
//                switch (cfg.matchSegment) {
//                    case 1:
//                        r1 = segment_r1;
//                        r2 = (r - r1 + r1offset + segmentR + cfg_c);
//                        seg = 1;
//                        x += cfg.seg1xoffset;
//                        y += cfg.seg1yoffset;
//                        QLOG_DEBUG()<<"match segment 1";
//                    break;
//                    case 2:
//                    r1 = segment_r2;
//                    r2 = (r - r1 + r2offset + segmentR + cfg_c);
//                    seg = 2;
//                    x += cfg.seg2xoffset;
//                    y += cfg.seg2yoffset;
//                    QLOG_DEBUG()<<"match segment 2";
//                    break;
//                case 3:
//                    r1 = segment_r3;
//                    r2 = (r - r1 + r3offset + segmentR + cfg_c);
//                    seg = 3;
//                    x += cfg.seg3xoffset;
//                    y += cfg.seg3yoffset;
//                    QLOG_DEBUG()<<"match segment 3";
//                    break;
//                case 4:
//                    r1 = segment_r4;
//                    r2 = (r - r1 + r4offset + segmentR + cfg_c);
//                    seg = 4;
//                    x += cfg.seg4xoffset;
//                    y += cfg.seg4yoffset;
//                    QLOG_DEBUG()<<"match segment 4";
//                break;
//                }
//            }
//            return;
//        }
        //QLOG_DEBUG()<<"cfg val:3D_xpos="<<cfg_x<<",3D_pos="<<cfg_y;
        QLOG_DEBUG()<<"before offset,x:"<<x<<",y:"<<y;
        if(x >cfg_x  && y > cfg_y){

            r1= segment_r1;//seg 1
            //r1offset = cfg.segment1_Offset;
            r2 = (r - r1 + r1offset + segmentR + cfg_c);
            seg = 1;
            //x += cfg.seg1xoffset;
            //y += cfg.seg1yoffset;
            //QLOG_DEBUG()<<"3D seg:1"<<"r:"<<r<<",r1:"<<r1<<",r2:"<<r2;
        }else if( x < cfg_x && y > cfg_y){

            r1 = segment_r2;
            //r1offset = cfg.segment2_Offset;
            r2 = (r - r1 + r2offset + segmentR + cfg_c);
            seg = 2;
            //x += cfg.seg2xoffset;
            //y += cfg.seg2yoffset;

            //QLOG_DEBUG()<<"3D seg:2"<<"r:"<<r<<",r1:"<<r1<<",r2:"<<r2;

        }else if(x < cfg_x && y < cfg_y){

            r1 = segment_r3;
            //r1offset = cfg.segment3_Offset;
            r2 = (r - r1 + r3offset + segmentR + cfg_c);
            seg = 3;
            //x += cfg.seg3xoffset;
            //y += cfg.seg3yoffset;
            //QLOG_DEBUG()<<"3D seg:3"<<"r:"<<r<<",r1:"<<r1<<",r2:"<<r2;
        }else if(x > cfg_x && y < cfg_y)
        {

            r1 = segment_r4;
            //r1offset = cfg.segment4_Offset;
            r2 = (r - r1 + r4offset + segmentR + cfg_c);
            seg = 4;
            //x += cfg.seg4xoffset;
            //y += cfg.seg4yoffset;
            //QLOG_DEBUG()<<"3D seg:4"<<"r:"<<r<<",r1:"<<r1<<",r2:"<<r2;
        }
        QLOG_DEBUG()<<"take offset,x:"<<x<<",y:"<<y;
        //如果是90度或者270度的反料  再偏移180
        if((85 < r && r < 95) || (265 < r && r < 275)){
            r2 += 180;
            QLOG_DEBUG()<<"90 or 270,r2+180 ="<<"r"<<r2;
        }
        QLOG_DEBUG()<<"seg:"<<seg<<"<r:"<<r<<",r1:"<<r1<<",r1offset:"<<r1offset<<",segR:"<<segmentR<<"c_offset:"<<cfg_c<<"r2:"<<r2;
    }
public slots:
//    void onNewConnection();
    void onImageCaptured(const QImage& image);
    void onCloudDetected(QVector<QString> vec);
    void newCaculate(){
    //trans3D: "105.381,-529.052,-114.45,216.571,357.058,178.699,1"
        std::vector<double> posdata(8);
        posdata.clear();

        posdata.push_back(105.381);
        posdata.push_back(-529.052);
        posdata.push_back(-114.45);
        posdata.push_back(216.571);
        posdata.push_back(357.058);
        posdata.push_back(178.699);
        cp.poseTrans2RT(posdata,resRT);
        QMatrix4x4 qMatrix(
                                        resRT(0,0), resRT(0,1), resRT(0,2), resRT(0,3),
                                        resRT(1,0), resRT(1,1), resRT(1,2), resRT(1,3),
                                        resRT(2,0), resRT(2,1), resRT(2,2), resRT(2,3),
                                        resRT(3,0), resRT(3,1), resRT(3,2), resRT(3,3)
                                    );



        //第一步 提取旋转后的Z轴(上表面的法向量方向）

        QVector3D normal_world(
            qMatrix( 0,2), //nx
            qMatrix(1,2), //ny
            qMatrix(2,2)  //nz
        );
        QVector3D normal = normal_world;
        QLOG_DEBUG() << QStringLiteral("法向量") << normal;
        double theta_x = std::atan2(normal.y(), normal.z());
        double theta_y = -std::atan2(normal.x(), std::sqrt(normal.y()*normal.y() + normal.z()*normal.z()));
        //旋转角度 绕x轴旋转  绕y轴旋转 只控制绕x轴
        QLOG_DEBUG()<<"get rx:"<<theta_x<<",ry:"<<theta_y;
        //第二部 计算偏移向量   len表示 物体中心到物体上表面的距离
        int len = 200;
        QVector3D offset = normal * len;

        //第三步 添加立方体中心坐标

        float x_offset = offset.x();
        float y_offset = offset.y();
        float z_offset = offset.z();
        QLOG_DEBUG()<<"GET x_offset:"<<x_offset<<",y_offset:"<<y_offset<<",z_offset:"<<z_offset;
        //float x0,y0,z0;
        //新坐标
        //float x = x0 + x_offset;
        //float y = y0 + y_offset;
        //float z = z0 + z_offset;
    }
    //void onMessageRead(QString message,int clientID);
    void onZeroSet();
    void onPlcRead(int addr,int value);
    void onPlcTimerOut();
    void onPlcProcesing();
    void onTimerStart();
    void onTimerStop();
    void plcSend(int addr,int val){
        plcClient->writeRegister(addr,val);
    }

signals:
    void startCapture();

    void imgCaptured(const QImage& image);
    void start2DCapture();
    void detectRemains(int);
private:

    QMutex mutex;
    Controller(QObject *parent = nullptr);
    ~Controller();

    QThread* cameraThread;

    //3D
    TcpClient* tcpclient;
    //PLC
    Modbusclient* plcClient;
    QTimer* plcTimer;

    OD::CloudProcess cp;
    Eigen::Matrix4f resRT;
public:
    Serialport* serialportDevice;
    QSettings* settings;
    Config cfg;
    float x_seg = -36.619;
    float y_seg = -389.86;
    bool needRequest = true;
    bool lastTriggerState = false; // 保存上一次读取值是否为1
};

#endif // CONTROLLER_H
