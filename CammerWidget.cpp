#include "CammerWidget.h"
#include "ui_cammerwidget.h"
#include <QsLog/QsLog.h>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include "controller.h"
#define DEFAULT_SHOW_RATE (30) // 默认显示帧率 | defult display frequency
#define DEFAULT_ERROR_STRING ("N/A")
#define MAX_FRAME_STAT_NUM (50)
#define MIN_LEFT_LIST_NUM (2)
#define MAX_STATISTIC_INTERVAL (5000000000) // List的更新时与最新一帧的时间最大间隔 |  The maximum time interval between the update of list and the latest frame
using namespace HalconCpp;
// 取流回调函数
// get frame callback function
static void FrameCallback(IMV_Frame* pFrame, void* pUser)
{
    CammerWidget* pCammerWidget = (CammerWidget*)pUser;
    if (!pCammerWidget)
    {
        printf("pCammerWidget is NULL!\n");
        return;
    }

    CFrameInfo frameInfo;
    frameInfo.m_nWidth = (int)pFrame->frameInfo.width;
    frameInfo.m_nHeight = (int)pFrame->frameInfo.height;
    frameInfo.m_nBufferSize = (int)pFrame->frameInfo.size;
    frameInfo.m_nPaddingX = (int)pFrame->frameInfo.paddingX;
    frameInfo.m_nPaddingY = (int)pFrame->frameInfo.paddingY;
    frameInfo.m_ePixelType = pFrame->frameInfo.pixelFormat;
    frameInfo.m_pImageBuf = (unsigned char *)malloc(sizeof(unsigned char) * frameInfo.m_nBufferSize);
    frameInfo.m_nTimeStamp = pFrame->frameInfo.timeStamp;

    // 内存申请失败，直接返回
    // memory application failed, return directly
    if (frameInfo.m_pImageBuf != NULL)
    {
        memcpy(frameInfo.m_pImageBuf, pFrame->pData, frameInfo.m_nBufferSize);

        if (pCammerWidget->m_qDisplayFrameQueue.size() > 16)
        {
            CFrameInfo frameOld;
            if (pCammerWidget->m_qDisplayFrameQueue.get(frameOld))
            {
                free(frameOld.m_pImageBuf);
                frameOld.m_pImageBuf = NULL;
            }
        }

        pCammerWidget->m_qDisplayFrameQueue.push_back(frameInfo);
    }

    pCammerWidget->recvNewFrame(pFrame->frameInfo.size);
}

// 显示线程
// display thread
static unsigned int __stdcall displayThread(void* pUser)
{
    CammerWidget* pCammerWidget = (CammerWidget*)pUser;
    if (!pCammerWidget)
    {
        printf("pCammerWidget is NULL!\n");
        return -1;
    }

    pCammerWidget->display();

    return 0;
}

CammerWidget::CammerWidget(QWidget *parent) :
    QWidget(parent)
    ,ui(new Ui::CammerWidget)
    , m_currentCameraKey("")
    , m_devHandle(NULL)
    , m_nDisplayInterval(0)
    , m_nFirstFrameTime(0)
    , m_nLastFrameTime(0)
    , m_nTotalFrameCount(0)
    , m_bNeedUpdate(true)
    , m_isExitDisplayThread(false)
    , m_threadHandle(NULL)
{
    ui->setupUi(this);


    qthalconwin=new QtHalconWin(ui->widget);
    qRegisterMetaType<uint64_t>("uint64_t");
    connect(this, SIGNAL(signalShowImage(unsigned char*, int, int, uint64_t)), this, SLOT(ShowImage(unsigned char*, int, int, uint64_t)));
    //connect(this, SIGNAL(signalShowImage(unsigned char*, int, int, uint64_t)), this, SLOT(HalConProcess(unsigned char*, int, int, uint64_t)));

    ui->label_Pixmap->setVisible(false);
    // 默认显示30帧
    // defult display 30 frames
    setDisplayFPS(DEFAULT_SHOW_RATE);

    m_elapsedTimer.start();

    // 启动显示线程
    // start display thread
    m_threadHandle = (HANDLE)_beginthreadex(NULL,
        0,
        displayThread,
        this,
        CREATE_SUSPENDED,
        NULL);

    if (!m_threadHandle)
    {
        printf("Failed to create display thread!\n");
    }
    else
    {
        ResumeThread(m_threadHandle);

        m_isExitDisplayThread = false;
    }

    //如果配置了index 直接搜索相机 按照配置的index 自动打开相机 设置触发模式 并开始捕获等待触发信号
    int index = Controller::getInstance().cfg.cameraIndex2D;
    if(index > 0){
        qDebug()<<"2d Camera Index:"<<index;
        //自动过程
        initCameraList();
        SetCamera(m_deviceInfoList.pDevInfo[index-1].cameraKey);
        if(!CameraOpen()){
            QLOG_DEBUG()<<"open camera failed!";
            //return;
        }
        CameraChangeTrig(CammerWidget::ETrigType::trigSoftware);
        CameraStart();
        ui->btn_startGrab->setEnabled(false);
        ui->btn_stopGrab->setEnabled(true);
        ui->comboBox_camerlist->setEnabled(true);
    }else{
        qDebug()<<"2d Camera Index not found:"<<index;
    }

    //signal
    //emit displayData(rowNum.I(),colNum.I(),rowGap.I(),colGap.I(),garyMin.I(),grayMax.I());
    grayMin = Controller::getInstance().cfg.grayMin;
    grayMax = Controller::getInstance().cfg.grayMax;
}

CammerWidget::~CammerWidget()
{
    CameraStop();
    // 关闭显示线程
    // close display thread
    m_isExitDisplayThread = true;
    WaitForSingleObject(m_threadHandle, INFINITE);
    CloseHandle(m_threadHandle);

    delete ui;
}

void CammerWidget::initCameraList()
{
    int ret = IMV_OK;
    ret = IMV_EnumDevices(&m_deviceInfoList, interfaceTypeAll);
    if (IMV_OK != ret)
    {
        QLOG_DEBUG()<<"Enumeration devices failed! ErrorCode:"<< ret;
        return;
    }
    if (m_deviceInfoList.nDevNum < 1)
    {
        QLOG_DEBUG()<<"2D camera not found";
        ui->btn_openCamera->setEnabled(false);
        ui->comboBox_camerlist->setEnabled(false);

        ui->btn_openCamera->setEnabled(false);

    }else{
        ui->comboBox_camerlist->setEnabled(true);
        ui->btn_openCamera->setEnabled(true);


        for(unsigned int i = 0; i < m_deviceInfoList.nDevNum; i++){
            ui->comboBox_camerlist->addItem(m_deviceInfoList.pDevInfo[i].cameraKey);
        }
        SetCamera(m_deviceInfoList.pDevInfo[0].cameraKey);
    }
    ui->btn_closeCamera->setEnabled(false);
    ui->btn_startGrab->setEnabled(false);
    ui->btn_stopGrab->setEnabled(false);
}

// 设置曝光
// set exposeTime
bool CammerWidget::SetExposeTime(double dExposureTime)
{
    int ret = IMV_OK;

    ret = IMV_SetDoubleFeatureValue(m_devHandle, "ExposureTime", dExposureTime);
    if (IMV_OK != ret)
    {
        printf("set ExposureTime value = %0.2f fail, ErrorCode[%d]\n", dExposureTime, ret);
        return false;
    }

    return true;
}

// 设置增益
// set gain
bool CammerWidget::SetAdjustPlus(double dGainRaw)
{
    int ret = IMV_OK;

    ret = IMV_SetDoubleFeatureValue(m_devHandle, "GainRaw", dGainRaw);
    if (IMV_OK != ret)
    {
        printf("set GainRaw value = %0.2f fail, ErrorCode[%d]\n", dGainRaw, ret);
        return false;
    }

    return true;
}

// 打开相机
// open camera
bool CammerWidget::CameraOpen(void)
{
    int ret = IMV_OK;

    if (m_currentCameraKey.length() == 0)
    {
        printf("open camera fail. No camera.\n");
        return false;
    }

    if (m_devHandle)
    {
        printf("m_devHandle is already been create!\n");
        return false;
    }
    QByteArray cameraKeyArray = m_currentCameraKey.toLocal8Bit();
    char* cameraKey = cameraKeyArray.data();

    ret = IMV_CreateHandle(&m_devHandle, modeByCameraKey, (void*)cameraKey);
    if (IMV_OK != ret)
    {
        printf("create devHandle failed! cameraKey[%s], ErrorCode[%d]\n", cameraKey, ret);
        return false;
    }

    // 打开相机
    // Open camera
    ret = IMV_Open(m_devHandle);
    if (IMV_OK != ret)
    {
        printf("open camera failed! ErrorCode[%d]\n", ret);
        return false;
    }

    return true;
}

// 关闭相机
// close camera
bool CammerWidget::CameraClose(void)
{
    int ret = IMV_OK;

    if (!m_devHandle)
    {
        printf("close camera fail. No camera.\n");
        return false;
    }

    if (false == IMV_IsOpen(m_devHandle))
    {
        printf("camera is already close.\n");
        return false;
    }

    ret = IMV_Close(m_devHandle);
    if (IMV_OK != ret)
    {
        printf("close camera failed! ErrorCode[%d]\n", ret);
        return false;
    }

    ret = IMV_DestroyHandle(m_devHandle);
    if (IMV_OK != ret)
    {
        printf("destroy devHandle failed! ErrorCode[%d]\n", ret);
        return false;
    }

    m_devHandle = NULL;

    return true;
}

// 开始采集
// start grabbing
bool CammerWidget::CameraStart()
{
    int ret = IMV_OK;

    if (IMV_IsGrabbing(m_devHandle))
    {
        printf("camera is already grebbing.\n");
        return false;
    }


    ret = IMV_AttachGrabbing(m_devHandle, FrameCallback, this);
    if (IMV_OK != ret)
    {
        printf("Attach grabbing failed! ErrorCode[%d]\n", ret);
        return false;
    }

    ret = IMV_StartGrabbing(m_devHandle);
    if (IMV_OK != ret)
    {
        printf("start grabbing failed! ErrorCode[%d]\n", ret);
        return false;
    }

    return true;
}

// 停止采集
// stop grabbing
bool CammerWidget::CameraStop()
{
    int ret = IMV_OK;
    if (!IMV_IsGrabbing(m_devHandle))
    {
        printf("camera is already stop grebbing.\n");
        return false;
    }

    ret = IMV_StopGrabbing(m_devHandle);
    if (IMV_OK != ret)
    {
        printf("Stop grabbing failed! ErrorCode[%d]\n", ret);
        return false;
    }

    // 清空显示队列
    // clear display queue
    CFrameInfo frameOld;
    while (m_qDisplayFrameQueue.get(frameOld))
    {
        free(frameOld.m_pImageBuf);
        frameOld.m_pImageBuf = NULL;
    }

    m_qDisplayFrameQueue.clear();

    return true;
}

// 切换采集方式、触发方式 （连续采集、外部触发、软件触发）
// Switch acquisition mode and triggering mode (continuous acquisition, external triggering and software triggering)
bool CammerWidget::CameraChangeTrig(ETrigType trigType)
{
    int ret = IMV_OK;

    if (trigContinous == trigType)
    {
        // 设置触发模式
        // set trigger mode
        ret = IMV_SetEnumFeatureSymbol(m_devHandle, "TriggerMode", "Off");
        if (IMV_OK != ret)
        {
            printf("set TriggerMode value = Off fail, ErrorCode[%d]\n", ret);
            return false;
        }
    }
    else if (trigSoftware == trigType)
    {
        // 设置触发器
        // set trigger
        ret = IMV_SetEnumFeatureSymbol(m_devHandle, "TriggerSelector", "FrameStart");
        if (IMV_OK != ret)
        {
            printf("set TriggerSelector value = FrameStart fail, ErrorCode[%d]\n", ret);
            return false;
        }

        // 设置触发模式
        // set trigger mode
        ret = IMV_SetEnumFeatureSymbol(m_devHandle, "TriggerMode", "On");
        if (IMV_OK != ret)
        {
            printf("set TriggerMode value = On fail, ErrorCode[%d]\n", ret);
            return false;
        }

        // 设置触发源为软触发
        // set triggerSource as software trigger
        ret = IMV_SetEnumFeatureSymbol(m_devHandle, "TriggerSource", "Software");
        if (IMV_OK != ret)
        {
            printf("set TriggerSource value = Software fail, ErrorCode[%d]\n", ret);
            return false;
        }
    }
    else if (trigLine == trigType)
    {
        // 设置触发器
        // set trigger
        ret = IMV_SetEnumFeatureSymbol(m_devHandle, "TriggerSelector", "FrameStart");
        if (IMV_OK != ret)
        {
            printf("set TriggerSelector value = FrameStart fail, ErrorCode[%d]\n", ret);
            return false;
        }

        // 设置触发模式
        // set trigger mode
        ret = IMV_SetEnumFeatureSymbol(m_devHandle, "TriggerMode", "On");
        if (IMV_OK != ret)
        {
            printf("set TriggerMode value = On fail, ErrorCode[%d]\n", ret);
            return false;
        }

        // 设置触发源为Line1触发
        // set trigggerSource as Line1 trigger
        ret = IMV_SetEnumFeatureSymbol(m_devHandle, "TriggerSource", "Line1");
        if (IMV_OK != ret)
        {
            printf("set TriggerSource value = Line1 fail, ErrorCode[%d]\n", ret);
            return false;
        }
    }

    return true;
}

// 执行一次软触发
// execute one software trigger
bool CammerWidget::ExecuteSoftTrig(void)
{
    QLOG_DEBUG()<<"ExecuteSoftTrig...";
    int ret = IMV_OK;

    ret = IMV_ExecuteCommandFeature(m_devHandle, "TriggerSoftware");
    if (IMV_OK != ret)
    {
        QLOG_DEBUG()<<"ExecuteSoftTrig fail, ErrorCode:";
        QLOG_DEBUG()<<ret;
        return false;
    }
    QLOG_DEBUG()<<"ExecuteSoftTrig success.";
    //printf("ExecuteSoftTrig success.\n");
    return true;
}

// 设置当前相机
// set current camera
void CammerWidget::SetCamera(const QString& strKey)
{
    m_currentCameraKey = strKey;
}

void CammerWidget::generateTemplate()
{


    // Local iconic variables
      HObject  ho_Image, ho_ROI_0, ho_ImageReduced1;
      HObject  ho_GrayImage1, ho_Regions1, ho_RegionOpening1, ho_Rectangle1;
      HObject  ho_ImageAffinTrans, ho_wuo, ho_Rectangle2, ho_EmptyObject;
      HObject  ho_ObjectSelected, ho_ImageReduced, ho_GrayImage;
      HObject  ho_Regions, ho_RegionClosing, ho_ConnectedRegions;
      HObject  ho_RegionOpening, ho_ConnectedRegions1, ho_SelectedRegions;
      HObject  ho_Rectangle;

      // Local control variables
      HTuple  hv_T1, hv_ImageFiles, hv_Index, hv_Width;
      HTuple  hv_Height, hv_Row3, hv_Column3, hv_Phi, hv_Length1;
      HTuple  hv_Length2, hv_HomMat2D, hv_HomMat2DInverse, hv_slRow;
      HTuple  hv_slCol, hv_srRow, hv_srCol, hv_disRow, hv_disCol;
      HTuple  hv_dd, hv_row1, hv_startgray, hv_endgray, hv_row;
      HTuple  hv_col, hv_rec_l_row, hv_rec_l_col, hv_rec_r_row;
      HTuple  hv_rec_r_col, hv_Number, hv_Index2, hv_Area1, hv_aRow;
      HTuple  hv_aColumn, hv_Mean, hv_Deviation, hv_NumRegions;
      HTuple  hv_Row1, hv_Column1, hv_Row2, hv_Column2, hv_Area;
      HTuple  hv_Row2_1, hv_Column2_1, hv_Row2_2, hv_Column2_2;
      HTuple  hv_Row, hv_Column, hv_RowOriginal, hv_ColumnOriginal;
      HTuple  hv_mixRow, hv_mixColumn, hv_T2, hv_RunTime, hv_WindowHandle;
    int ret = LoadImageToHObject(ho_Image);
    //GenImage1(&ho_Image,"byte",nWidth,nHeight,reinterpret_cast<Hlong>(pRgbFrameBuf));

    if(ret){
        //成功获取到本地文件
        GetImageSize(ho_Image,&hv_Width,&hv_Height);

        WId winId = ui->label_Pixmap->winId();//ui handle
        // ui handle -> HLong
        Hlong halconWinId = static_cast<Hlong>(winId);
        int width, height;
        width = ui->label_Pixmap->width();
        height = ui->label_Pixmap->height();
        OpenWindow(0,0,hv_Width/3,hv_Height/3,halconWinId,"visible","",&hv_WindowHandle);

        SetWindowAttr("background_color","black");

        HDevWindowStack::Push(hv_WindowHandle);
          if (HDevWindowStack::IsOpen())
            DispObj(ho_Image, HDevWindowStack::GetActive());



        //Rgb1ToGray(ho_Image, &ho_GrayImage);
        //鼠标画矩形
        DrawRectangle1(hv_WindowHandle, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
        GenRectangle1(&ho_Rectangle, hv_Row1, hv_Column1, hv_Row2, hv_Column2);
        //存hv_Row1, hv_Column1, hv_Row2, hv_Column2
        QVector<HTuple> pos{hv_Row1, hv_Column1, hv_Row2, hv_Column2};

//        //切割制作模板的region
//        ReduceDomain(ho_GrayImage, ho_Rectangle, &ho_ImageReduced);
//        //生成模板和保存，-4，4表示此模板可以搜索到-180°到180°之间的所有目标
//        CreateShapeModel(ho_ImageReduced, "auto", -4, 4, "auto", "auto", "use_polarity",
//               "auto", "auto", &hv_ModelID);
//        WriteShapeModel(hv_ModelID, "./tofind.shm");
//        QMessageBox::information(nullptr, "Success", "模型制作成功");

        //GenRectangle1(&ho_ROI_0, 176.46, 203.383, 2134.08, 3185.17);
            ReduceDomain(ho_Image, ho_Rectangle, &ho_ImageReduced1);
            Rgb1ToGray(ho_ImageReduced1, &ho_GrayImage1);
            GetImageSize(ho_GrayImage1, &hv_Width, &hv_Height);
            Threshold(ho_GrayImage1, &ho_Regions1, 0, 5);
            OpeningRectangle1(ho_Regions1, &ho_RegionOpening1, 5, 5);
            SmallestRectangle2(ho_RegionOpening1, &hv_Row3, &hv_Column3, &hv_Phi, &hv_Length1,
                &hv_Length2);
            GenRectangle2(&ho_Rectangle1, hv_Row3, hv_Column3, hv_Phi, hv_Length1, hv_Length2);
            VectorAngleToRigid(hv_Height/2, hv_Width/2, hv_Phi, hv_Row3, hv_Column3, 0, &hv_HomMat2D);
            HomMat2dInvert(hv_HomMat2D, &hv_HomMat2DInverse);
            AffineTransImage(ho_ImageReduced1, &ho_ImageAffinTrans, hv_HomMat2D, "constant",
                "false");
            GenEmptyObj(&ho_wuo);

            //显示ho_ImageAffinTrans 覆盖相机图片


            //模板2
            DrawRectangle1(hv_WindowHandle, &hv_Row2_1, &hv_Column2_1, &hv_Row2_2, &hv_Column2_2);
            //hv_Row2_1, hv_Column2_1, hv_Row2_2, hv_Column2_2
            hv_slRow = hv_Row2_1;
               hv_slCol = hv_Column2_1;
               hv_srRow = hv_Row2_2;
               hv_srCol = hv_Column2_2;
               hv_disRow = 510;
               hv_disCol = 510;
               hv_dd = 530;
               hv_row1 = 390;

               //***********************灰度阈值****************************
                 hv_startgray = 0;
                 hv_endgray = 8;
                 //**********************************************************
                 for (hv_row=0; hv_row<=3; hv_row+=1)
                 {
                   for (hv_col=0; hv_col<=5; hv_col+=1)
                   {
                     hv_rec_l_row = hv_slRow+(hv_row*hv_disRow);
                     hv_rec_l_col = hv_slCol+(hv_col*hv_disCol);
                     hv_rec_r_row = hv_srRow+(hv_row*hv_disRow);
                     hv_rec_r_col = hv_srCol+(hv_col*hv_disCol);
                     GenRectangle1(&ho_Rectangle2, hv_rec_l_row, hv_rec_l_col, hv_rec_r_row, hv_rec_r_col);
                     ConcatObj(ho_wuo, ho_Rectangle2, &ho_wuo);
                   }
                 }
                 CountObj(ho_wuo, &hv_Number);

                 GenEmptyObj(&ho_EmptyObject);


                 HTuple end_val52 = hv_Number;
                 HTuple step_val52 = 1;
                 for (hv_Index2=1; hv_Index2.Continue(end_val52, step_val52); hv_Index2 += step_val52)
                 {

                   SelectObj(ho_wuo, &ho_ObjectSelected, hv_Index2);
                   AreaCenter(ho_ObjectSelected, &hv_Area1, &hv_aRow, &hv_aColumn);
                   ReduceDomain(ho_ImageAffinTrans, ho_ObjectSelected, &ho_ImageReduced);
                   Rgb1ToGray(ho_ImageReduced, &ho_GrayImage);
                   Intensity(ho_ObjectSelected, ho_ImageReduced, &hv_Mean, &hv_Deviation);

                   Threshold(ho_GrayImage, &ho_Regions, 0, 8);

                   ClosingRectangle1(ho_Regions, &ho_RegionClosing, 10, 60);
                   Connection(ho_RegionClosing, &ho_ConnectedRegions);
                   OpeningRectangle1(ho_ConnectedRegions, &ho_RegionOpening, 20, 1);
                   Connection(ho_RegionOpening, &ho_ConnectedRegions1);
                   SelectShape(ho_ConnectedRegions1, &ho_SelectedRegions, ((HTuple("area").Append("height")).Append("width")),
                       "and", ((HTuple(1000).Append(65)).Append(10)), ((HTuple(6000).Append(190)).Append(40)));
                   //计算筛选后的区域数量，确认是否有物料
                   CountObj(ho_SelectedRegions, &hv_NumRegions);
                   //360 890 1420 1950
                   if (0 != (HTuple(hv_NumRegions!=0).TupleAnd(hv_Mean<100)))
                   {
                     SmallestRectangle1(ho_SelectedRegions, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
                     GenRectangle1(&ho_Rectangle, hv_Row1, hv_Column1, hv_Row2, hv_Column2);
                     AreaCenter(ho_Rectangle, &hv_Area, &hv_Row, &hv_Column);

                     ConcatObj(ho_EmptyObject, ho_Rectangle, &ho_EmptyObject);
                     //判断正反
                     if (0 != (hv_Index2<=6))
                     {
                       if (0 != (hv_Row>=hv_row1))
                       {
                         //disp_message(3600, hv_Row, "image", 520, hv_Column-100, "blue", "true");
                         qDebug()<<QString::fromStdString(hv_Row.S().Text());
                         qDebug()<<QString::fromStdString(hv_Index2.S().Text());
                         AffineTransPoint2d(hv_HomMat2DInverse, hv_Row, hv_Column, &hv_RowOriginal,
                             &hv_ColumnOriginal);
                         //RowOriginal, ColumnOriginal就是原图像的坐标

                       }
                       else if (0 != (hv_Row<hv_row1))
                       {
                         //disp_message(3600, hv_Row, "image", 520, hv_Column, "red", "true");
                        qDebug()<<QString::fromStdString(hv_Row.S().Text());
                        qDebug()<<QString::fromStdString(hv_Index2.S().Text());
                       }
                     }
                     else if (0 != (HTuple(hv_Index2>6).TupleAnd(hv_Index2<=12)))
                     {
                       if (0 != (hv_Row>=(hv_row1+hv_dd)))
                       {
                         //disp_message(3600, hv_Row, "image", hv_Row+100, hv_Column-100, "blue",
                             //"true");
                           qDebug()<<QString::fromStdString(hv_Row.S().Text());
                           qDebug()<<QString::fromStdString(hv_Index2.S().Text());
                         AffineTransPoint2d(hv_HomMat2DInverse, hv_Row, hv_Column, &hv_RowOriginal,
                             &hv_ColumnOriginal);
                         //RowOriginal, ColumnOriginal就是原图像的坐标
                       }
                       else if (0 != (hv_Row<(hv_row1+hv_dd)))
                       {
                         //disp_message(3600, hv_Row, "image", hv_Row+100, hv_Column, "red", "true");
                        qDebug()<<QString::fromStdString(hv_Row.S().Text());
                        qDebug()<<QString::fromStdString(hv_Index2.S().Text());
                       }
                     }
                     else if (0 != (HTuple(hv_Index2>12).TupleAnd(hv_Index2<=18)))
                     {
                       if (0 != (hv_Row>=(hv_row1+(hv_dd*2))))
                       {
                         //disp_message(3600, hv_Row, "image", hv_Row+100, hv_Column-100, "blue",
                         //    "true");
                           qDebug()<<QString::fromStdString(hv_Row.S().Text());
                           qDebug()<<QString::fromStdString(hv_Index2.S().Text());
                         AffineTransPoint2d(hv_HomMat2DInverse, hv_Row, hv_Column, &hv_RowOriginal,
                             &hv_ColumnOriginal);
                         //RowOriginal, ColumnOriginal就是原图像的坐标
                       }
                       else if (0 != (hv_Row<(hv_row1+(hv_dd*2))))
                       {
                         //disp_message(3600, hv_Row, "image", hv_Row+100, hv_Column, "red", "true");
                        qDebug()<<QString::fromStdString(hv_Row.S().Text());
                        qDebug()<<QString::fromStdString(hv_Index2.S().Text());
                       }
                     }
                     else if (0 != (HTuple(hv_Index2>18).TupleAnd(hv_Index2<=24)))
                     {
                       if (0 != (hv_Row>=(hv_row1+(hv_dd*3))))
                       {
                         //disp_message(3600, hv_Row, "image", hv_Row+100, hv_Column-100, "blue",
                         //    "true");
                           qDebug()<<QString::fromStdString(hv_Row.S().Text());
                           qDebug()<<QString::fromStdString(hv_Index2.S().Text());
                         AffineTransPoint2d(hv_HomMat2DInverse, hv_Row, hv_Column, &hv_RowOriginal,
                             &hv_ColumnOriginal);
                         //RowOriginal, ColumnOriginal就是原图像的坐标
                       }
                       else if (0 != (hv_Row<(hv_row1+(hv_dd*3))))
                       {
                         //disp_message(3600, hv_Row, "image", hv_Row+100, hv_Column, "red", "true");
                            qDebug()<<QString::fromStdString(hv_Row.S().Text());
                            qDebug()<<QString::fromStdString(hv_Index2.S().Text());
                       }
                     }
                   }
                   else if (0 != (HTuple(hv_NumRegions!=0).TupleAnd(hv_Mean<100)))
                   {
                     SmallestRectangle1(ho_ObjectSelected, &hv_mixRow, &hv_mixColumn, &hv_mixRow,
                         &hv_mixColumn);

                   }
                   else if (0 != (hv_NumRegions==0))
                   {
                     //没有物料/或有缺陷

                   }
                 }

   }

}

bool CammerWidget::LoadImageToHObject(HObject &ho_Image)
{
    // 使用 QFileDialog 选择图像文件
        QString filePath = QFileDialog::getOpenFileName(nullptr, "Select Image", "", "Images (*.jpeg *.jpg *.bmp *.tif)");

        if (!filePath.isEmpty()) {
            // 将 QString 转换为 std::string
            std::string imagePath = filePath.toStdString();

            // 使用 ReadImage 读取图像文件
            ReadImage(&ho_Image, imagePath.c_str());
            return true;
        }else{
            return false;
        }

}



void CammerWidget::testResult(float x, float y, float z,float c,int flag)
{
    QLOG_DEBUG()<<"testResult,x:"<<x<<",y :"<<y<<",z :"<<z<<",c :"<<c<<",flag:"<<flag;

    Controller::getInstance().onHalconDetected(x,y,z,c,flag);
}
void CammerWidget::clearResultQueue(){
    Controller::getInstance().clearHalconQueue();
}

void CammerWidget::popfromqueue()
{
    Controller::getInstance().handPopResult();
}
void CammerWidget::detectFull(bool full)
{
    if(full){
        Controller::getInstance().newImgResultPop1();
    }
}

void CammerWidget::ConvertToHObject(HObject *hImage, unsigned char *pRgbFrameBuf, int nWidth, int nHeight)
{
    HTuple hv_Width_img = nWidth;
    HTuple hv_Height_img = nHeight;
    HTuple hv_Pointer_img = reinterpret_cast<Hlong>(pRgbFrameBuf);
    //RGB888
    if(Controller::getInstance().cfg.detecType != 2){
        GenImageInterleaved(hImage,(Hlong)pRgbFrameBuf, "rgb",nWidth,nHeight, 0,"byte",0,0,0,0,-1,0);
    }else{
        GenImage1(hImage,"byte" , nWidth,nHeight,(Hlong)pRgbFrameBuf);
    }

}

QString CammerWidget::openImageFileDialog(QWidget *parent)
{
    QString filePath = QFileDialog::getOpenFileName(
            parent,
            "选择图片文件",
            "",
            "Image Files (*.png *.jpg *.bmp);;All Files (*)"
        );

    return filePath;
}

bool CammerWidget::loadImageToRgbBuffer()
{

    //这里是把本地图片转为SDK数据流 在进行测试 模拟相机图片数据
    QString filePath = QFileDialog::getOpenFileName(nullptr, "Select Image", "", "Images (*.jpeg *.jpg *.bmp *.tif)");
    QImage image(filePath);
        if (image.isNull()) {

            //判断一下导入的图片是否为空
            HTuple numCount;
            HalconCpp::CountObj(CurrentImage, &numCount);
            if(numCount==0)
            {
                QMessageBox::information(nullptr, "failed", "没有检测到图片，请导入图片后再模拟测试");
                return false;  // 加载失败
            }

        }
        qDebug()<<filePath;
        // 确保图片格式为 RGB888
        if (image.format() != QImage::Format_RGB888) {
            //qDebug()<<"pixel format";
            image = image.convertToFormat(QImage::Format_RGB888);
        }else{
            qDebug()<<"pixel format Format_RGB888";
        }

        // 获取图片宽度和高度
        int nWidth = image.width();
        int nHeight = image.height();
        qDebug()<<"nwidth"<<nWidth;
        qDebug()<<"nHeight"<<nHeight;
        // 计算所需的缓冲区大小（宽度 * 高度 * 3）
        int bufferSize = nWidth * nHeight * 3;

        // 为 pRgbFrameBuf 分配内存
        unsigned char *pRgbFrameBuf = new unsigned char[bufferSize];

        // 复制图像数据到 pRgbFrameBuf
        memcpy(pRgbFrameBuf, image.bits(), bufferSize);
        int type = Controller::getInstance().cfg.detecType;
        HalConProcess(pRgbFrameBuf, nWidth, nHeight,type);
        free(pRgbFrameBuf);

        return true;  // 成功加载并转换图片
}

// 显示
// diaplay
bool CammerWidget::ShowImage(unsigned char* pRgbFrameBuf, int nWidth, int nHeight, uint64_t nPixelFormat)
{

    QImage image;
    if (NULL == pRgbFrameBuf ||
        nWidth == 0 ||
            nHeight == 0)
        {
            QLOG_DEBUG()<<"image is invalid";
            printf("%s image is invalid.\n", __FUNCTION__);
            return false;
        }
        if (gvspPixelMono8 == nPixelFormat)
        {
            QLOG_DEBUG()<<"ShowImage get SDK image pixelFormat :Format_Grayscale8";
            image = QImage(pRgbFrameBuf, nWidth, nHeight, QImage::Format_Grayscale8);
        }
        else
        {
            QLOG_DEBUG()<<"ShowImage get SDK image pixelFormat :Format_RGB888";
            image = QImage(pRgbFrameBuf,nWidth, nHeight, QImage::Format_RGB888);
        }

        // 将QImage的大小收缩或拉伸，与label的大小保持一致。这样label中能显示完整的图片
        // Shrink or stretch the size of Qimage to match the size of the label. In this way, the complete image can be displayed in the label
        //QImage imageScale = image.scaled(QSize(ui->label_Pixmap->width(), ui->label_Pixmap->height()));
        //QPixmap pixmap = QPixmap::fromImage(imageScale);
        //ui->label_Pixmap->setPixmap(pixmap);

        //检测类型  1基座 2铜件套模 3安装架 4面板
        int type = Controller::getInstance().cfg.detecType;
        if(type==2){
            HalConProcess_Gray(pRgbFrameBuf, nWidth, nHeight);

        }else{
            HalConProcess(pRgbFrameBuf, nWidth, nHeight,type);
        }

        free(pRgbFrameBuf);

        return true;

    }

void CammerWidget::HalConProcess(unsigned char *pRgbFrameBuf, int nWidth, int nHeight,int type)
{

    //看一下检测正常料 还是残余料
       int offsetx,offsety,offsetz;
       offsetx = Controller::getInstance().cfg.offsetX_2d;
       offsety = Controller::getInstance().cfg.offsetY_2d;
       offsetz = Controller::getInstance().cfg.offsetZ_2d;
       float originR = Controller::getInstance().cfg.originR;
       QLOG_DEBUG()<<"detect origin c:"<<originR;
       ConvertToHObject(&CurrentImage,pRgbFrameBuf,nWidth,nHeight);
       qthalconwin->devDisplay(CurrentImage);
       //HTuple  hv_Number1, hv_Number2,hv_Number3;
       if(detecType != 0){
           QLOG_DEBUG()<<"detect remians,type:"<<type;

           if (type == 1){
               //******************************基座残余料*********************//
               HTuple hv_Numberc,hv_leng1,hv_leng2;
                VisionCheckResult rjzc=jz.CYaction(CurrentImage);

                   //显示
                 TupleLength(rjzc.hv_TupleNX,&hv_leng1);
                 TupleLength(rjzc.hv_TupleN2X,&hv_leng2);
                 CountObj(rjzc.ho_NGregion, &hv_Numberc);
                    if (0 != (int(hv_Numberc!=0)))
                    {
                        //re.hv_TupleNX反坐标;
                        qthalconwin->setColor("red");
                        qthalconwin->devDisplay(rjzc.ho_NGregion);
                       qthalconwin->devDisplay(rjzc. ho_NGcross);
                        qthalconwin->setColor("green");

                        QString qxString;
                        QString qyString;

                        for(int i = 0; i < hv_leng1.I(); i++){
                            qxString = QString::number(rjzc.hv_TupleNX[i].D());
                            qyString = QString::number(rjzc.hv_TupleNY[i].D());
                            //前两行 标志1   后两行标志2   5是残余料
                            //testResult(qxString.toFloat()+offsetx, qyString.toFloat()+offsety,offsetz,180,5);
                            testResult(qxString.toFloat()+offsetx, qyString.toFloat()+offsety,offsetz,180,8);
                            //qDebug()<<"NX XPOS:"<<qxString;
                            //qDebug()<<"NX YPOS:"<<qyString;
                        }
                        //line 3 4     value:72.637
                        //QLOG_DEBUG()<<"fan 2 size:"<<hv_leng2.I();
                        for(int i = 0; i < hv_leng2.I(); i++){
                            qxString = QString::number(rjzc.hv_TupleN2X[i].D());
                            qyString = QString::number(rjzc.hv_TupleN2Y[i].D());
                            //QLOG_DEBUG()<<"fan 72 ";
                            testResult(qxString.toFloat()+offsetx, qyString.toFloat()+offsety,offsetz,180,5);
                            //qDebug()<<"MX XPOS:"<<qxString;
                            //qDebug()<<"MX YPOS:"<<qyString;
                            //qDebug()<<"s:"<<hv_Number1.I();
                        }

                    }else{
                        testResult(0,0,0,0,6);//空盘
                    }

           }
           else if(type == 3){
               //******************************安装架残余料*********************//
           HTuple  hv_Number1, hv_Number2,hv_Number3,hv_leng1,hv_leng2;
           VisionCheckResult1 cyazj=AZJ.cyAZJaction(CurrentImage);

         CountObj(cyazj.ho_NGregion, &hv_Number1);

         TupleLength(cyazj.hv_TupleNX,&hv_leng1);

          if (0 != (int(hv_Number1!=0)))
         {
             //re.hv_TupleNX反坐标;
             qthalconwin->setColor("red");
             qthalconwin->devDisplay(cyazj.ho_NGregion);
             qthalconwin->setColor("green");

             QString qxString;
             QString qyString;

             for(int i = 0; i < hv_leng1.I(); i++){
                 qxString = QString::number(cyazj.hv_TupleNX[i].D());
                 qyString = QString::number(cyazj.hv_TupleNY[i].D());
                 //testResult(qxString.toFloat(), qyString.toFloat(),-125.0,1,3);
                 testResult(qxString.toFloat()+offsetx, qyString.toFloat()+offsety,offsetz,0,5);
                 //qDebug()<<"NX XPOS:"<<qxString;
                 //qDebug()<<"NX YPOS:"<<qyString;
             }

         }

         if(hv_Number1.I() == 0){
             QLOG_DEBUG()<<"empty flag 6";
           testResult(0,0,0,0,6);
         }
           }
           else if(type == 4){

               //*************************面板残余料*******************

               HTuple  hv_Number1, hv_Number2,hv_Number3,hv_leng1,hv_leng2;
               VisionCheckResult1 cyazj=AZJ.cyAZJaction(CurrentImage);

             CountObj(cyazj.ho_NGregion, &hv_Number1);

             TupleLength(cyazj.hv_TupleNX,&hv_leng1);

              if (0 != (int(hv_Number1!=0)))
             {
                 //re.hv_TupleNX反坐标;
                 qthalconwin->setColor("red");
                 qthalconwin->devDisplay(cyazj.ho_NGregion);
                 qthalconwin->setColor("green");

                 QString qxString;
                 QString qyString;
                   QString qcString;
                 for(int i = 0; i < hv_leng1.I(); i++){
                     qxString = QString::number(cyazj.hv_TupleNX[i].D());
                     qyString = QString::number(cyazj.hv_TupleNY[i].D());
                     qcString = QString::number(cyazj.hv_NGAngle[i].D());
                     //testResult(qxString.toFloat(), qyString.toFloat(),-125.0,1,3);
                     testResult(qxString.toFloat()+offsetx, qyString.toFloat()+offsety,offsetz,qcString.toFloat(),5);

                 }

             }

             if(hv_Number1.I() == 0){
                 QLOG_DEBUG()<<"empty flag 6";
               testResult(0,0,0,0,6);
             }
           }

       }else{
           QLOG_DEBUG()<<"detect normal,type:"<<type;

           if(type == 1){
               //******************************基座混料 反料*********************//
               HTuple  hv_Number1, hv_Number2,hv_Number3;
               //判断一下导入的图片是否为空
               HTuple numCount,hv_leng1,hv_leng2;
               HalconCpp::CountObj(CurrentImage, &numCount);
               if(numCount==0)
               {
                   //QMessageBox::information(nullptr, "failed", "没有检测到模型，请制作模型再检测");
                   return;
               }
                  VisionCheckResult rjz=jz.JZsearchmodaction(CurrentImage,grayMin,grayMax);
              // VisionCheckResult rjz=jz.action(CurrentImage);
               CountObj(rjz.ho_Mixregion, &hv_Number1);
               CountObj(rjz.ho_NGregion, &hv_Number2);
               CountObj(rjz.ho_NOregion, &hv_Number3);

               TupleLength(rjz.hv_TupleMX,&hv_leng1);
               TupleLength(rjz.hv_TupleM2X,&hv_leng2);

               if (0 != (int(hv_Number1!=0)))
                   {
                   //re.hv_TupleMX混料坐标;

                   qthalconwin->setColor("blue");
                   qthalconwin->devDisplay(rjz.ho_Mixregion);
                   qthalconwin->setColor("green");

                   QString qxString;
                   QString qyString;
                   QLOG_DEBUG()<<"hun 1 size:"<<hv_leng1.I();
                   for(int i = 0; i < hv_leng1.I(); i++){
                       qxString = QString::number(rjz.hv_TupleMX[i].D());
                       qyString = QString::number(rjz.hv_TupleMY[i].D());
                       //testResult(qxString.toFloat(), qyString.toFloat(),-125.0,1,4);
                       //testResult(qxString.toFloat()+offsetx, qyString.toFloat()+offsety,offsetz,originR,4);
                       testResult(qxString.toFloat()+offsetx, qyString.toFloat()+offsety,offsetz,originR,8);
                       qDebug()<<"MX XPOS:"<<qxString;
                       qDebug()<<"MX YPOS:"<<qyString;
                         qDebug()<<"s:"<<hv_Number1.I();
                   }
                   QLOG_DEBUG()<<"hun 2 size:"<<hv_leng2.I();
                   for(int i = 0; i < hv_leng2.I(); i++){
                       qxString = QString::number(rjz.hv_TupleM2X[i].D());
                       qyString = QString::number(rjz.hv_TupleM2Y[i].D());
                       //QLOG_DEBUG()<<"fan 72 ";
                       //testResult(qxString.toFloat(), qyString.toFloat(),-125.0,2,4);
                       //testResult(qxString.toFloat()+offsetx, qyString.toFloat()+offsety,offsetz,originR,4);
                       testResult(qxString.toFloat()+offsetx, qyString.toFloat()+offsety,offsetz,originR,8);
                       //qDebug()<<"MX XPOS:"<<qxString;
                       //qDebug()<<"MX YPOS:"<<qyString;
                       //qDebug()<<"s:"<<hv_Number1.I();
                   }


                   }
               TupleLength(rjz.hv_TupleNX,&hv_leng1);
               TupleLength(rjz.hv_TupleN2X,&hv_leng2);
               if (0 != (int(hv_Number2!=0)))
               {
                       //re.hv_TupleNX反坐标;
                       qthalconwin->setColor("red");
                       qthalconwin->devDisplay(rjz.ho_NGregion);
                       qthalconwin->devDisplay(rjz.ho_NGcross);
                       qthalconwin->setColor("green");

                       QString qxString;
                       QString qyString;

                       //line 1 2   value:251
                       QLOG_DEBUG()<<"fan 1 size:"<<hv_leng1.I();
                       for(int i = 0; i < hv_leng1.I(); i++){
                           qxString = QString::number(rjz.hv_TupleNX[i].D());
                           qyString = QString::number(rjz.hv_TupleNY[i].D());
                           //QLOG_DEBUG()<<"fan 251.532 ";
                           //testResult(qxString.toFloat(), qyString.toFloat(),-125.0,1,3);
                            //testResult(qxString.toFloat()+offsetx, qyString.toFloat()+offsety,offsetz,originR,3);
                            testResult(qxString.toFloat()+offsetx, qyString.toFloat()+offsety,offsetz,originR,8);
                           //qDebug()<<"NX XPOS:"<<qxString;
                           //qDebug()<<"NX YPOS:"<<qyString;
                       }
                       //line 3 4     value:72.637
                       QLOG_DEBUG()<<"fan 2 size:"<<hv_leng2.I();
                       for(int i = 0; i < hv_leng2.I(); i++){
                           qxString = QString::number(rjz.hv_TupleN2X[i].D());
                           qyString = QString::number(rjz.hv_TupleN2Y[i].D());
                           //QLOG_DEBUG()<<"fan 72 ";
                           //testResult(qxString.toFloat(), qyString.toFloat(),-125.0,2,3);
                            //testResult(qxString.toFloat()+offsetx, qyString.toFloat()+offsety,offsetz,originR,3);
                            testResult(qxString.toFloat()+offsetx, qyString.toFloat()+offsety,offsetz,originR,8);
                           //qDebug()<<"MX XPOS:"<<qxString;
                           //qDebug()<<"MX YPOS:"<<qyString;
                           //qDebug()<<"s:"<<hv_Number1.I();
                       }

                }
               if (0 != (int(hv_Number3!=0)))
               {

                   qthalconwin->setColor("yellow");
                   qthalconwin->devDisplay(rjz.ho_NOregion);
                   qthalconwin->setColor("green");

               }
               if(hv_Number1 == 0 && hv_Number2 == 0){
                   //合格

                   QLOG_DEBUG()<<"no flaws. flag 1";
                   testResult(0,0,offsetz,0,1);
              }
           }


           else if(type == 3){
               //******************************安装架 反料 混料*********************//
               HTuple  hv_Number1, hv_Number2,hv_Number3,hv_leng1,hv_leng2;
               VisionCheckResult1 az=AZJ.AZJaction(CurrentImage,grayMin,grayMax);
               CountObj(az.ho_Mixregion, &hv_Number1);
               CountObj(az.ho_NGregion, &hv_Number2);
               CountObj(az.ho_NOregion, &hv_Number3);

               TupleLength(az.hv_TupleMX,&hv_leng1);

        //混料屏蔽
//               if (0 != (int(hv_Number1!=0)))
//                   {
//                   //re.hv_TupleMX混料坐标;

//                   qthalconwin->setColor("blue");
//                   qthalconwin->devDisplay(az.ho_Mixregion);

//                   qthalconwin->setColor("green");

//                   QString qxString;
//                   QString qyString;

//                   for(int i = 0; i < hv_leng1.I(); i++){
//                       qxString = QString::number(az.hv_TupleMX[i].D());
//                       qyString = QString::number(az.hv_TupleMY[i].D());
//                       //testResult(qxString.toFloat(), qyString.toFloat(),-125.0,1.5,4);
//                       testResult(qxString.toFloat()+offsetx, qyString.toFloat()+offsety,offsetz,0,4);
//                   }



//                   }
               TupleLength(az.hv_TupleNX,&hv_leng1);
               TupleLength(az.hv_TupleN2X,&hv_leng2);

                    if (0 != (int(hv_Number2!=0)))
                   {
                       //re.hv_TupleNX反坐标;
                       qthalconwin->setColor("red");
                       qthalconwin->devDisplay(az.ho_NGregion);
                       qthalconwin->devDisplay(az.ho_NGcross);

                       qthalconwin->setColor("green");

                       QString qxString;
                       QString qyString;

                       for(int i = 0; i < hv_leng1.I(); i++){
                           qxString = QString::number(az.hv_TupleNX[i].D());
                           qyString = QString::number(az.hv_TupleNY[i].D());

                           //testResult(qxString.toFloat(), qyString.toFloat(),-125.0,1.5,3);
                           testResult(qxString.toFloat()+offsetx, qyString.toFloat()+offsety,offsetz,180,3);

                       }
                   }
                   if (0 != (int(hv_Number3!=0)))
                   {

                         qthalconwin->setColor("yellow");
                         qthalconwin->devDisplay(az.ho_NOregion);
                         qthalconwin->setColor("green");

                   }
                   if(hv_Number1.I() == 0 && hv_Number2.I() == 0){
                       //合格
                       QLOG_DEBUG()<<"qualified flag 1";
                     testResult(0,0,offsetz,0,1);
                   }
           }

           else if(type == 4){
               //**************************面板混料  反料*********************
               HTuple  hv_Number1, hv_Number2,hv_Number3,hv_leng1,hv_leng2;
               VisionCheckResult1 mb=MB.MBaction(CurrentImage,grayMin,grayMax);

               CountObj(mb.ho_Mixregion, &hv_Number1);
               CountObj(mb.ho_NGregion, &hv_Number2);
               CountObj(mb.ho_NOregion, &hv_Number3);
               QString NString;
                NString = QString::number(hv_Number2.D());
                QLOG_DEBUG()<<"NG:"<<NString;
               TupleLength(mb.hv_TupleMX,&hv_leng1);


               if (0 != (int(hv_Number1!=0)))
                   {
                   //re.hv_TupleMX混料坐标;

                   qthalconwin->setColor("blue");
                   qthalconwin->devDisplay(mb.ho_Mixregion);
                   qthalconwin->setColor("green");

                   QString qxString;
                   QString qyString;

                   for(int i = 0; i < hv_leng1.I(); i++){
                       qxString = QString::number(mb.hv_TupleMX[i].D());
                       qyString = QString::number(mb.hv_TupleMY[i].D());
                       //testResult(qxString.toFloat(), qyString.toFloat(),-125.0,1,4);
                       testResult(qxString.toFloat()+offsetx, qyString.toFloat()+offsety,offsetz,180,4);

                       //qDebug()<<"s:"<<hv_Number1.I();
                   }


                   }
               TupleLength(mb.hv_TupleNX,&hv_leng1);

                    if (0 != (int(hv_Number2!=0)))
                   {
                       //re.hv_TupleNX反坐标;
                       qthalconwin->setColor("red");
                       qthalconwin->devDisplay(mb.ho_NGregion);
                       qthalconwin->devDisplay(mb.ho_NGcross);
                       qthalconwin->setColor("green");

                       QString qxString;
                       QString qyString;
                       QString qcString;

                       for(int i = 0; i < hv_leng1.I(); i++){
                           qxString = QString::number(mb.hv_TupleNX[i].D());
                           qyString = QString::number(mb.hv_TupleNY[i].D());
                           qcString = QString::number(mb.hv_NGAngle[i].D());
                           //testResult(qxString.toFloat(), qyString.toFloat(),-125.0,1,3);
                           testResult(qxString.toFloat()+offsetx, qyString.toFloat()+offsety,offsetz,qcString.toFloat(),3);
                           //qDebug()<<"NX XPOS:"<<qxString;
                           //qDebug()<<"NX YPOS:"<<qyString;
                           QLOG_DEBUG()<<"MX XPOS:"<<qxString<<"   "<<i;
                           QLOG_DEBUG()<<"MX YPOS:"<<qyString;
                           QLOG_DEBUG()<<"MC XPOS:"<<qcString;
                       }


                   }
                   if (0 != (int(hv_Number3!=0)))
                   {

                         qthalconwin->setColor("yellow");
                         qthalconwin->devDisplay(mb.ho_NOregion);
                         qthalconwin->setColor("green");

                   }
                   if(hv_Number1.I() == 0 && hv_Number2.I() == 0){
                       //合格
                     QLOG_DEBUG()<<"qualified flag 1";
                     testResult(0,0,offsetz,0,1);
                   }
           }


     }


       detectFull(true);//本次检测完 自动弹出一个坐标

}

void CammerWidget::HalConProcess_Gray(unsigned char *pRgbFrameBuf, int nWidth, int nHeight)
{
    ConvertToHObject(&CurrentImage,pRgbFrameBuf,nWidth,nHeight);
    qthalconwin->devDisplay(CurrentImage);

    bool ret = TJT.TJTMaction(CurrentImage);

    if(ret){
        QLOG_DEBUG()<<"TJT OK";
        //空模OK  4524 写入2

        Controller::getInstance().plcSend(4524,2);

    }else{
        QLOG_DEBUG()<<"TJT NG";
        //空模 NG 4524 写1

        Controller::getInstance().plcSend(4524,1);
    }

}

void CammerWidget::onStart2DCapture()
{
    qDebug()<<"CammerWidget::onStart2DCapture";
    ExecuteSoftTrig();
}

void CammerWidget::onDetectRemains(int val)
{
    QLOG_DEBUG()<<"onDetectRemains:value"<<val;
    detecType = val;
}

void CammerWidget::onRowDataChanged(int num, int gap)
{
    rowNum = num;
    rowGap = gap;
    qDebug()<<"data changed,row num:"<<rowNum.I()<<",row gap:"<<gap;

}

void CammerWidget::onColDataChanged(int num, int gap)
{
    colNum = num;
    colGap = gap;
    qDebug()<<"data changed,col num:"<<num<<",col gap:"<<gap;
}

void CammerWidget::onGrayDataChanged(int min, int max)
{
    grayMin = min;
    grayMax = max;
    qDebug()<<"gray data changed,col num:"<<min<<",col gap:"<<max;
}

void CammerWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    QTimer::singleShot(0,[this](){
        SetWindowExtents(qthalconwin->hv_WindowHandle,
                         0,0,
                         ui->widget->width()-1,
                         ui->widget->height()-1);

    });
}

// 显示线程  // 253.4  74.24
// display thread
void CammerWidget::display()
{
    while (!m_isExitDisplayThread)
    {
        CFrameInfo frameInfo;

        if (false == m_qDisplayFrameQueue.get(frameInfo))
        {
            Sleep(1);
            continue;
        }

        // 判断是否要显示。超过显示上限（30帧），就不做转码、显示处理
        // Judge whether to display. If the upper display limit (30 frames) is exceeded, transcoding and display processing will not be performed
        if (!isTimeToDisplay())
        {
            // 释放内存
            // release memory
            free(frameInfo.m_pImageBuf);
            continue;
        }

        // mono8格式可不做转码，直接显示，其他格式需要经过转码才能显示
        // mono8 format can be displayed directly without transcoding. Other formats can be displayed only after transcoding
        if (gvspPixelMono8 == frameInfo.m_ePixelType)
        {
            // 显示线程中发送显示信号，在主线程中显示图像
            // Send display signal in display thread and display image in main thread
            emit signalShowImage(frameInfo.m_pImageBuf, (int)frameInfo.m_nWidth, (int)frameInfo.m_nHeight, (uint64_t)frameInfo.m_ePixelType);
        }
        else
        {
            // 转码
            unsigned char* pRGBbuffer = NULL;
            int nRgbBufferSize = 0;
            nRgbBufferSize = frameInfo.m_nWidth * frameInfo.m_nHeight * 3;
            pRGBbuffer = (unsigned char*)malloc(nRgbBufferSize);
            if (pRGBbuffer == NULL)
            {
                // 释放内存
                // release memory
                free(frameInfo.m_pImageBuf);
                printf("RGBbuffer malloc failed.\n");
                continue;
            }

            IMV_PixelConvertParam stPixelConvertParam;
            stPixelConvertParam.nWidth = frameInfo.m_nWidth;
            stPixelConvertParam.nHeight = frameInfo.m_nHeight;
            stPixelConvertParam.ePixelFormat = frameInfo.m_ePixelType;
            stPixelConvertParam.pSrcData = frameInfo.m_pImageBuf;
            stPixelConvertParam.nSrcDataLen = frameInfo.m_nBufferSize;
            stPixelConvertParam.nPaddingX = frameInfo.m_nPaddingX;
            stPixelConvertParam.nPaddingY = frameInfo.m_nPaddingY;
            stPixelConvertParam.eBayerDemosaic = demosaicNearestNeighbor;
            stPixelConvertParam.eDstPixelFormat = gvspPixelRGB8;
            stPixelConvertParam.pDstBuf = pRGBbuffer;
            stPixelConvertParam.nDstBufSize = nRgbBufferSize;

            int ret = IMV_PixelConvert(m_devHandle, &stPixelConvertParam);
            if (IMV_OK != ret)
            {
                // 释放内存
                // release memory
                printf("image convert to RGB failed! ErrorCode[%d]\n", ret);
                free(frameInfo.m_pImageBuf);
                free(pRGBbuffer);
                continue;
            }

            // 释放内存
            // release memory
            free(frameInfo.m_pImageBuf);

            // 显示线程中发送显示信号，在主线程中显示图像
            // Send display signal in display thread and display image in main thread
            emit signalShowImage(pRGBbuffer, (int)stPixelConvertParam.nWidth, (int)stPixelConvertParam.nHeight, (uint64_t)stPixelConvertParam.eDstPixelFormat);
        }
    }
}


bool CammerWidget::isTimeToDisplay()
{
    m_mxTime.lock();

    // 不显示
    // don't display
    if (m_nDisplayInterval <= 0)
    {
        m_mxTime.unlock();
        return false;
    }

    // 第一帧必须显示
    // the frist frame must be displayed
    if (m_nFirstFrameTime == 0 || m_nLastFrameTime == 0)
    {
        m_nFirstFrameTime = m_elapsedTimer.nsecsElapsed();
        m_nLastFrameTime = m_nFirstFrameTime;

        m_mxTime.unlock();
        return true;
    }

    // 当前帧和上一帧的间隔如果大于显示间隔就显示
    // display if the interval between the current frame and the previous frame is greater than the display interval
    uint64_t nCurTimeTmp = m_elapsedTimer.nsecsElapsed();
    uint64_t nAcquisitionInterval = nCurTimeTmp - m_nLastFrameTime;
    if (nAcquisitionInterval > m_nDisplayInterval)
    {
        m_nLastFrameTime = nCurTimeTmp;
        m_mxTime.unlock();
        return true;
    }

    // 当前帧相对于第一帧的时间间隔
    // Time interval between the current frame and the first frame
    uint64_t nPre = (m_nLastFrameTime - m_nFirstFrameTime) % m_nDisplayInterval;
    if (nPre + nAcquisitionInterval > m_nDisplayInterval)
    {
        m_nLastFrameTime = nCurTimeTmp;
        m_mxTime.unlock();
        return true;
    }

    m_mxTime.unlock();
    return false;
}

// 设置显示频率
// set display frequency
void CammerWidget::setDisplayFPS(int nFPS)
{
    m_mxTime.lock();
    if (nFPS > 0)
    {
        m_nDisplayInterval = 1000 * 1000 * 1000.0 / nFPS;
    }
    else
    {
        m_nDisplayInterval = 0;
    }
    m_mxTime.unlock();
}

// 窗口关闭响应函数
// window close response function
void CammerWidget::closeEvent(QCloseEvent * event)
{
    Q_UNUSED(event);
    IMV_DestroyHandle(m_devHandle);
    m_devHandle = NULL;
}

// 状态栏统计信息 开始
// Status bar statistics begin
void CammerWidget::resetStatistic()
{
    QMutexLocker locker(&m_mxStatistic);
    m_nTotalFrameCount = 0;
    m_listFrameStatInfo.clear();
    m_bNeedUpdate = true;
}
QString CammerWidget::getStatistic()
{
    if (m_mxStatistic.tryLock(30))
    {
        if (m_bNeedUpdate)
        {
            updateStatistic();
        }

        m_mxStatistic.unlock();
        return m_strStatistic;
    }
    return "";
}
void CammerWidget::updateStatistic()
{
    size_t nFrameCount = m_listFrameStatInfo.size();
    QString strFPS = DEFAULT_ERROR_STRING;
    QString strSpeed = DEFAULT_ERROR_STRING;

    if (nFrameCount > 1)
    {
        quint64 nTotalSize = 0;
        FrameList::const_iterator it = m_listFrameStatInfo.begin();

        if (m_listFrameStatInfo.size() == 2)
        {
            nTotalSize = m_listFrameStatInfo.back().m_nFrameSize;
        }
        else
        {
            for (++it; it != m_listFrameStatInfo.end(); ++it)
            {
                nTotalSize += it->m_nFrameSize;
            }
        }

        const FrameStatInfo& first = m_listFrameStatInfo.front();
        const FrameStatInfo& last = m_listFrameStatInfo.back();

        qint64 nsecs = last.m_nPassTime - first.m_nPassTime;

        if (nsecs > 0)
        {
            double dFPS = (nFrameCount - 1) * ((double)1000000000.0 / nsecs);
            double dSpeed = nTotalSize * ((double)1000000000.0 / nsecs) / (1000.0) / (1000.0) * (8.0);
            strFPS = QString::number(dFPS, 'f', 2);
            strSpeed = QString::number(dSpeed, 'f', 2);
        }
    }

    m_strStatistic = QString("Stream: %1 images   %2 FPS   %3 Mbps")
        .arg(m_nTotalFrameCount)
        .arg(strFPS)
        .arg(strSpeed);
    m_bNeedUpdate = false;
}

void CammerWidget::recvNewFrame(quint32 frameSize)
{
    QMutexLocker locker(&m_mxStatistic);
    if (m_listFrameStatInfo.size() >= MAX_FRAME_STAT_NUM)
    {
        m_listFrameStatInfo.pop_front();
    }
    m_listFrameStatInfo.push_back(FrameStatInfo(frameSize, m_elapsedTimer.nsecsElapsed()));
    ++m_nTotalFrameCount;

    if (m_listFrameStatInfo.size() > MIN_LEFT_LIST_NUM)
    {
        FrameStatInfo infoFirst = m_listFrameStatInfo.front();
        FrameStatInfo infoLast = m_listFrameStatInfo.back();
        while (m_listFrameStatInfo.size() > MIN_LEFT_LIST_NUM && infoLast.m_nPassTime - infoFirst.m_nPassTime > MAX_STATISTIC_INTERVAL)
        {
            m_listFrameStatInfo.pop_front();
            infoFirst = m_listFrameStatInfo.front();
        }
    }

    m_bNeedUpdate = true;
}
// 状态栏统计信息 end
// Status bar statistics ending

void CammerWidget::on_pushButton_selectImage_clicked()
{
   QString filePath = QFileDialog::getOpenFileName(nullptr, "Select Image", "", "Images (*.jpeg *.jpg *.bmp *.tif)");
   if(filePath.isEmpty()){
      QMessageBox::information(nullptr, "failed", "没有检测到图片文件，请导入图片后再制作模型");
      return;
   }
   CurrentImage = qthalconwin->readimage(filePath);

}

void CammerWidget::on_pushButton_gnerate_clicked()
{
    //判断一下导入的图片是否为空
    HTuple numCount;
    HalconCpp::CountObj(CurrentImage, &numCount);
    if(numCount==0)
    {
        QMessageBox::information(nullptr, "failed", "检测到图片，请导入图片后再制作模型");
        return;
    }
    HObject   ho_Rectangle3;
    HObject  ho_wuo, ho_Rectangle2;

    // Local control variables
    HTuple  hv_Row2_1, hv_Column2_1;
    HTuple  hv_Row2_2, hv_Column2_2, hv_disRow, hv_disCol, hv_startgray;
    HTuple  hv_endgray, hv_row, hv_col, hv_rec_l_row, hv_rec_l_col;
    HTuple  hv_rec_r_row, hv_rec_r_col, hv_Number;
    //***************画出搜索框架****************

    qthalconwin->devDisplay(CurrentImage);
    DrawRectangle1(qthalconwin->hv_WindowHandle, &hv_Row2_1, &hv_Column2_1, &hv_Row2_2, &hv_Column2_2);
    GenRectangle1(&ho_Rectangle3, hv_Row2_1, hv_Column2_1, hv_Row2_2, hv_Column2_2);
    hv_disRow = 510;
    hv_disCol = 510;
    //**********************灰度阈值****************************
    hv_startgray = 0;
    hv_endgray = 8;
    //**********************************************************
    GenEmptyObj(&ho_wuo);

    for (hv_row=0; hv_row<=rowNum-1; hv_row+=1)
    {
      for (hv_col=0; hv_col<=colNum-1; hv_col+=1)
      {
        hv_rec_l_row = hv_Row2_1+(hv_row*rowGap);
        hv_rec_l_col = hv_Column2_1+(hv_col*colGap);
        hv_rec_r_row = hv_Row2_2+(hv_row*rowGap);
        hv_rec_r_col = hv_Column2_2+(hv_col*colGap);
        GenRectangle1(&ho_Rectangle2, hv_rec_l_row, hv_rec_l_col, hv_rec_r_row, hv_rec_r_col);
        ConcatObj(ho_wuo, ho_Rectangle2, &ho_wuo);
      }

    }
   WriteObject(ho_wuo, "Serchmodel");
   qthalconwin->devDisplay(ho_wuo);
}

void CammerWidget::on_pushButton_detect_clicked()
{

    //看一下检测正常料 还是残余料
        if(detecType != 0){
            qDebug()<<"detect remains";
            HObject  ho_wuo, ho_EmptyObject, ho_Mixregion;
            HObject  ho_ObjectSelected, ho_ImageReduced, ho_GrayImage;
            HObject  ho_Rectangle5, ho_ImageEquHisto, ho_ImageMedian;
            HObject  ho_Regions, ho_ConnectedRegions, ho_SelectedRegions;
            HObject  ho_Rectangle, ho_CYRectangle, ho_Cross;

            // Local control variables
            HTuple  hv_ImageFiles, hv_Index, hv_Number, hv_HomMat2Dto3D;
            HTuple  hv_WindowHandle, hv_TupleX, hv_TupleY, hv_Index2;
            HTuple  hv_Area1, hv_aRow, hv_aColumn, hv_NGR1, hv_NGC1;
            HTuple  hv_NGR2, hv_NGC2, hv_Mean, hv_Deviation, hv_NumRegions;
            HTuple  hv_Row1, hv_Column1, hv_Row2, hv_Column2, hv_Area;
            HTuple  hv_Row, hv_Column, hv_Qx, hv_Qy, hv_Length, hv_Index1;
            HTuple  hv_Area2, hv_Row4, hv_Column4, hv_Numberc;
             VisionCheckResult rjzc;
            QString executableDir = QCoreApplication::applicationDirPath();

            QDir dir(executableDir);
            dir.cdUp();
            QString proDir = dir.absolutePath();
            QString path = proDir + "/files/JZSerch.hobj";
            qDebug()<<"file path:"<<path;

            try {
                   HTuple hfilePath = path.toStdString().c_str();
                   ReadObject(&ho_wuo,hfilePath);

            } catch (HException &ex) {
                   qDebug()<<"hobject read failed";
            }

                CountObj(ho_wuo, &hv_Number);

                try {
                        path = proDir + "/files/HandEyeCalibrationMatrix.tup";
                       HTuple hfilePath = path.toStdString().c_str();
                        ReadTuple(hfilePath, &hv_HomMat2Dto3D);

                } catch (HException &ex) {
                       qDebug()<<"htuple read failed";
                }

                GenEmptyObj(&ho_EmptyObject);
                GenEmptyObj(&ho_Mixregion);

                hv_TupleX = HTuple();
                hv_TupleY = HTuple();
                {
                HTuple end_val14 = hv_Number;
                HTuple step_val14 = 1;
                for (hv_Index2=1; hv_Index2.Continue(end_val14, step_val14); hv_Index2 += step_val14)
                {
                  SelectObj(ho_wuo, &ho_ObjectSelected, hv_Index2);
                  AreaCenter(ho_ObjectSelected, &hv_Area1, &hv_aRow, &hv_aColumn);
                  ReduceDomain(CurrentImage, ho_ObjectSelected, &ho_ImageReduced);
                  SmallestRectangle1(ho_ObjectSelected, &hv_NGR1, &hv_NGC1, &hv_NGR2, &hv_NGC2);
                  Rgb1ToGray(ho_ImageReduced, &ho_GrayImage);
                  Intensity(ho_ObjectSelected, ho_ImageReduced, &hv_Mean, &hv_Deviation);
                  if (0 != (int(hv_Mean>150)))
                  {
                    GenRectangle1(&ho_Rectangle5, hv_NGR1, hv_NGC1, hv_NGR2, hv_NGC2);
                    ConcatObj(ho_Mixregion, ho_Rectangle5, &ho_Mixregion);
                  }
                  EquHistoImage(ho_GrayImage, &ho_ImageEquHisto);
                  MedianImage(ho_ImageEquHisto, &ho_ImageMedian, "circle", 10, "mirrored");

                  Threshold(ho_ImageMedian, &ho_Regions, 0, 15);
                  Connection(ho_Regions, &ho_ConnectedRegions);
                  SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, (HTuple("circularity").Append("area")),
                      "and", (HTuple(0.75).Append(600)), (HTuple(1).Append(1000)));
                  CountObj(ho_SelectedRegions, &hv_NumRegions);


                  if (0 != (int(hv_NumRegions!=0)))
                  {
                    SmallestRectangle1(ho_SelectedRegions, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
                    GenRectangle1(&ho_Rectangle, hv_Row1, hv_Column1, hv_Row2, hv_Column2);
                    AreaCenter(ho_Rectangle, &hv_Area, &hv_Row, &hv_Column);
                    GenRectangle1(&ho_CYRectangle, hv_NGR1, hv_NGC1, hv_NGR2, hv_NGC2);
                    ConcatObj(ho_EmptyObject, ho_CYRectangle, &ho_EmptyObject);

                    AffineTransPoint2d(hv_HomMat2Dto3D, hv_Row, hv_Column, &hv_Qx, &hv_Qy);
                    TupleConcat(hv_TupleX, hv_Qx, &hv_TupleX);
                    TupleConcat(hv_TupleY, hv_Qy, &hv_TupleY);
                  }
                  else if (0 != (int(hv_NumRegions==0)))
                  {
                    //没有物料
                  }
                }
                }

                TupleLength(hv_TupleY, &hv_Length);
                if (0 != (int(hv_Length!=0)))
                {
                  {
                  HTuple end_val51 = hv_Length-1;
                  HTuple step_val51 = 1;
                  for (hv_Index1=0; hv_Index1.Continue(end_val51, step_val51); hv_Index1 += step_val51)
                  {
                    GenCrossContourXld(&ho_Cross, HTuple(hv_TupleX[hv_Index1]), HTuple(hv_TupleY[hv_Index1]),
                        15, 3);
                    AreaCenterPointsXld(ho_Cross, &hv_Area2, &hv_Row4, &hv_Column4);
                  }
                  }
                }
                rjzc.ho_NGregion=ho_EmptyObject;
                rjzc.hv_TupleNX=hv_TupleX;
                rjzc.hv_TupleNY=hv_TupleY;
                //显示
                 CountObj(rjzc.ho_NGregion, &hv_Numberc);
                 if (0 != (int(hv_Numberc!=0)))
                 {
                     //re.hv_TupleNX反坐标;
                     qthalconwin->setColor("red");
                     qthalconwin->devDisplay(rjzc.ho_NGregion);
                     qthalconwin->setColor("green");

                     QString qxString;
                     QString qyString;

                     for(int i = 0; i < hv_Numberc.I(); i++){
                         qxString = QString::number(rjzc.hv_TupleNX[i].D());
                         qyString = QString::number(rjzc.hv_TupleNY[i].D());
                         qDebug()<<"NX XPOS:"<<qxString;
                         qDebug()<<"NX YPOS:"<<qyString;
                     }

                 }

        }else{
            qDebug()<<"normal detect";

            HTuple  hv_Number1, hv_Number2,hv_Number3;
            //判断一下导入的图片是否为空
            HTuple numCount;
            HalconCpp::CountObj(CurrentImage, &numCount);
            if(numCount==0)
            {
                //QMessageBox::information(nullptr, "failed", "没有检测到图片，请制作模型再检测");
                return;
            }
            VisionCheckResult rjz=jz.JZsearchmodaction(CurrentImage,grayMin,grayMax);
           // VisionCheckResult rjz=jz.action(CurrentImage);
            CountObj(rjz.ho_Mixregion, &hv_Number1);
            CountObj(rjz.ho_NGregion, &hv_Number2);
            CountObj(rjz.ho_NOregion, &hv_Number3);

            if (0 != (int(hv_Number1!=0)))
                {
                //re.hv_TupleMX混料坐标;

                qthalconwin->setColor("blue");
                qthalconwin->devDisplay(rjz.ho_Mixregion);
                qthalconwin->setColor("green");

                QString qxString;
                QString qyString;

                for(int i = 0; i < hv_Number1.I(); i++){
                    qxString = QString::number(rjz.hv_TupleMX[i].D());
                    qyString = QString::number(rjz.hv_TupleMY[i].D());

                    qDebug()<<"MX XPOS:"<<qxString;
                    qDebug()<<"MX YPOS:"<<qyString;
                      qDebug()<<"s:"<<hv_Number1.I();
                 }
                }

                if (0 != (int(hv_Number2!=0)))
                {
                    //re.hv_TupleNX反坐标;
                    qthalconwin->setColor("red");
                    qthalconwin->devDisplay(rjz.ho_NGregion);
                    qthalconwin->setColor("green");

                    QString qxString;
                    QString qyString;

                    for(int i = 0; i < hv_Number2.I(); i++){
                        qxString = QString::number(rjz.hv_TupleNX[i].D());
                        qyString = QString::number(rjz.hv_TupleNY[i].D());
                        qDebug()<<"NX XPOS:"<<qxString;
                        qDebug()<<"NX YPOS:"<<qyString;
                    }



                }
                if (0 != (int(hv_Number3!=0)))
                {

                      qthalconwin->setColor("yellow");
                      qthalconwin->devDisplay(rjz.ho_NOregion);
                      qthalconwin->setColor("green");

                }

        }
        detecType = 0;


}



void CammerWidget::on_pushButton_clicked()
{
    //QLOG_DEBUG()<<"VALUE:"<<ui->lineEdit->text();
    //hv_row1 = HTuple(ui->lineEdit->text().toInt());
}

void CammerWidget::on_pushButton_inportImgTest_clicked()
{
    loadImageToRgbBuffer();
}

void CammerWidget::on_btn_getCameraList_clicked()
{
     initCameraList();

}

void CammerWidget::on_btn_openCamera_clicked()
{

    if(!CameraOpen()){
        QLOG_DEBUG()<<"open camera failed!";
        return;
    }
    ui->comboBox_camerlist->setEnabled(false);
    ui->btn_openCamera->setEnabled(false);
    ui->btn_closeCamera->setEnabled(true);
    ui->btn_startGrab->setEnabled(true);
    ui->btn_stopGrab->setEnabled(false);
}

void CammerWidget::on_btn_softTrigger_clicked()
{
    CameraChangeTrig(CammerWidget::ETrigType::trigSoftware);
}

void CammerWidget::on_btn_startGrab_clicked()
{
    CameraStart();
    ui->btn_startGrab->setEnabled(false);
    ui->btn_stopGrab->setEnabled(true);
    ui->comboBox_camerlist->setEnabled(true);
}

void CammerWidget::on_btn_stopGrab_clicked()
{
    CameraStop();
    ui->btn_startGrab->setEnabled(true);
    ui->btn_stopGrab->setEnabled(false);
}

void CammerWidget::on_btn_closeCamera_clicked()
{
    on_btn_stopGrab_clicked();
    CameraClose();
    ui->comboBox_camerlist->setEnabled(true);
    ui->btn_openCamera->setEnabled(true);
    ui->btn_closeCamera->setEnabled(false);
    ui->btn_startGrab->setEnabled(false);
    ui->btn_stopGrab->setEnabled(false);
}

void CammerWidget::on_btn_softTrigger_excute_clicked()
{
    ExecuteSoftTrig();
}

void CammerWidget::on_comboBox_camerlist_currentIndexChanged(int index)
{
    SetCamera(m_deviceInfoList.pDevInfo[index].cameraKey);
}

void CammerWidget::on_pushButton_clearQueue_clicked()
{
    clearResultQueue();
}

void CammerWidget::on_pushButton_2_clicked()
{
    popfromqueue();
}

void CammerWidget::on_pushButton_3_clicked()
{
    HObject  ho_c1, ho_GrayImage, ho_ImageMedian;
    HObject  ho_Circle, ho_RegionDifference, ho_ImageSurface;
    HObject  ho_ImageSub, ho_Rectangle1, ho_ImageReduced, ho_Rectangle3;
    HObject  ho_medicine_regions, ho_Rectangle2;

    // Local control variables
    HTuple  hv_WindowHandle, hv_Width, hv_Height;
    HTuple  hv_Alpha, hv_Beta, hv_Gamma, hv_Delta, hv_Epsilon;
    HTuple  hv_Zeta, hv_Area, hv_Row, hv_Column, hv_Row11, hv_Column11;
    HTuple  hv_Row2, hv_Column2, hv_ModelID, hv_Row2_1, hv_Column2_1;
    HTuple  hv_Row2_2, hv_Column2_2, hv_disRow, hv_disCol, hv_dd;
    HTuple  hv_Area2, hv_Row1, hv_Column1, hv_col, hv_row, hv_rec_l_row;
    HTuple  hv_rec_l_col, hv_rec_r_row, hv_rec_r_col;

       qthalconwin->setLineWidth(1);
        Rgb1ToGray(CurrentImage, &ho_GrayImage);
         MedianImage(ho_GrayImage, &ho_ImageMedian, "circle", 5, "mirrored");

         GetImageSize(ho_ImageMedian, &hv_Width, &hv_Height);
         GenCircle(&ho_Circle, 495, 630, 350.5);
         Difference(ho_ImageMedian, ho_Circle, &ho_RegionDifference);
         FitSurfaceSecondOrder(ho_RegionDifference, ho_ImageMedian, "regression", 5, 2,
             &hv_Alpha, &hv_Beta, &hv_Gamma, &hv_Delta, &hv_Epsilon, &hv_Zeta);
         AreaCenter(ho_RegionDifference, &hv_Area, &hv_Row, &hv_Column);
         GenImageSurfaceSecondOrder(&ho_ImageSurface, "byte", hv_Alpha, hv_Beta, hv_Gamma,
             hv_Delta, hv_Epsilon, hv_Zeta, hv_Row, hv_Column, hv_Width, hv_Height);
         SubImage(ho_ImageMedian, ho_ImageSurface, &ho_ImageSub, 1, 128);
         qthalconwin->devDisplay(ho_ImageSub);

         DrawRectangle1(qthalconwin->hv_WindowHandle, &hv_Row11, &hv_Column11, &hv_Row2, &hv_Column2);
         GenRectangle1(&ho_Rectangle1, hv_Row11, hv_Column11, hv_Row2, hv_Column2);

         ReduceDomain(ho_ImageSub, ho_Rectangle1, &ho_ImageReduced);
         CreateShapeModel(ho_ImageReduced, 4, (HTuple(0).TupleRad()), HTuple(360).TupleRad(),
             "auto", "auto", "ignore_global_polarity", "auto", "auto", &hv_ModelID);
        qthalconwin->devDisplay(ho_ImageReduced);
         WriteShapeModel(hv_ModelID, "Itemodel");
  qDebug()<<"zzz";


}

void CammerWidget::on_pushButton_4_clicked()
{
    HTuple  hv_Number1, hv_Number2,hv_Number3,hv_leng1,hv_leng2;
    VisionCheckResult1 rjz=AZJ.AZJaction(CurrentImage,grayMin,grayMax);

    CountObj(rjz.ho_Mixregion, &hv_Number1);
    CountObj(rjz.ho_NGregion, &hv_Number2);
    CountObj(rjz.ho_NOregion, &hv_Number3);

    TupleLength(rjz.hv_TupleMX,&hv_leng1);
    TupleLength(rjz.hv_TupleM2X,&hv_leng2);

    if (0 != (int(hv_Number1!=0)))
        {
        //re.hv_TupleMX混料坐标;

        qthalconwin->setColor("blue");
        qthalconwin->devDisplay(rjz.ho_Mixregion);
        qthalconwin->setColor("green");

        QString qxString;
        QString qyString;
        QLOG_DEBUG()<<"hun 1 size:"<<hv_leng1.I();
        for(int i = 0; i < hv_leng1.I(); i++){
            qxString = QString::number(rjz.hv_TupleMX[i].D());
            qyString = QString::number(rjz.hv_TupleMY[i].D());
            //testResult(qxString.toFloat(), qyString.toFloat(),-125.0,1,4);

            qDebug()<<"MX XPOS:"<<qxString;
            qDebug()<<"MX YPOS:"<<qyString;
              qDebug()<<"s:"<<hv_Number1.I();
        }
        QLOG_DEBUG()<<"hun 2 size:"<<hv_leng2.I();
        for(int i = 0; i < hv_leng2.I(); i++){
            qxString = QString::number(rjz.hv_TupleM2X[i].D());
            qyString = QString::number(rjz.hv_TupleM2Y[i].D());

        }


        }
    TupleLength(rjz.hv_TupleNX,&hv_leng1);
    TupleLength(rjz.hv_TupleN2X,&hv_leng2);
    if (0 != (int(hv_Number2!=0)))
    {
            //re.hv_TupleNX反坐标;
            qthalconwin->setColor("red");
            qthalconwin->devDisplay(rjz.ho_NGregion);
            qthalconwin->setColor("green");

            QString qxString;
            QString qyString;

            //line 1 2   value:251
            QLOG_DEBUG()<<"fan 1 size:"<<hv_leng1.I();
            for(int i = 0; i < hv_leng1.I(); i++){
                qxString = QString::number(rjz.hv_TupleNX[i].D());
                qyString = QString::number(rjz.hv_TupleNY[i].D());

            }
            //line 3 4     value:72.637
            QLOG_DEBUG()<<"fan 2 size:"<<hv_leng2.I();
            for(int i = 0; i < hv_leng2.I(); i++){
                qxString = QString::number(rjz.hv_TupleN2X[i].D());
                qyString = QString::number(rjz.hv_TupleN2Y[i].D());

            }

     }
    if (0 != (int(hv_Number3!=0)))
    {

        qthalconwin->setColor("yellow");
        qthalconwin->devDisplay(rjz.ho_NOregion);
        qthalconwin->setColor("green");

    }

}

void CammerWidget::on_pushButton_5_clicked()
{
    HTuple  hv_Number1, hv_Number2,hv_Number3,hv_leng1,hv_leng2;
    VisionCheckResult1 az=MB.MBaction(CurrentImage,grayMin,grayMax);
    CountObj(az.ho_Mixregion, &hv_Number1);
    CountObj(az.ho_NGregion, &hv_Number2);
    CountObj(az.ho_NOregion, &hv_Number3);

    TupleLength(az.hv_TupleMX,&hv_leng1);
   // qthalconwin->dispMessage(az.hv_gray);

    if (0 != (int(hv_Number1!=0)))
        {
        //re.hv_TupleMX混料坐标;

        qthalconwin->setColor("blue");
        qthalconwin->devDisplay(az.ho_Mixregion);
        qthalconwin->setColor("green");

        QString qxString;
        QString qyString;
        QString qcString;
        for(int i = 0; i < hv_leng1.I()-1; i++){

            qxString = QString::number(az.hv_TupleMX[i].D());
           qyString = QString::number(az.hv_TupleMY[i].D());
            qDebug()<<"112233";
          // qcString = QString::number(az.hv_NGAngle[i].D());
          //  testResult(qxString.toFloat(), qyString.toFloat(),-125.0,251.582,4);
            qDebug()<<"MX XPOS:"<<qxString;
            qDebug()<<"MY YPOS:"<<qyString;
           // qDebug()<<"MC YPOS:"<<qcString;
            qDebug()<<"s:"<<hv_Number1.I();
        }
//        for(int i = 0; i < hv_leng2.I(); i++){
//            qxString = QString::number(az.hv_TupleM2X[i].D());
//            qyString = QString::number(az.hv_TupleM2Y[i].D());
//            testResult(qxString.toFloat(), qyString.toFloat(),-125.0,251.582,4);
//            qDebug()<<"MX XPOS:"<<qxString;
//            qDebug()<<"MX YPOS:"<<qyString;
//            qDebug()<<"s:"<<hv_Number1.I();
//        }


        }
    TupleLength(az.hv_TupleNX,&hv_leng1);
    TupleLength(az.hv_TupleNX,&hv_leng2);
  qDebug()<<"hv_leng2:"<<hv_leng2.I();
   // TupleLength(az.hv_TupleN2X,&hv_leng2);

         if (0 != (int(hv_Number2!=0)))
        {
            //re.hv_TupleNX反坐标;
            qthalconwin->setColor("red");
            qthalconwin->devDisplay(az.ho_NGregion);
            qthalconwin->setColor("green");

            QString qxString;
            QString qyString;
            QString qcString;
            qDebug()<<"size:"<<hv_leng1.I();
            for(int i = 0; i <=hv_leng1.I()-1; i++){
                qxString = QString::number(az.hv_TupleNX[i].D());
                qyString = QString::number(az.hv_TupleNY[i].D());
                qcString = QString::number(az.hv_NGAngle[i].D());
                qDebug()<<"NX XPOS:"<<qxString;
                qDebug()<<"NY YPOS:"<<qyString;
                qDebug()<<"NC YPOS:"<<qcString;
                qDebug()<<"size:  "<<i;

            }
//            for(int i = 0; i < hv_leng2.I(); i++){
//                qxString = QString::number(az.hv_TupleN2X[i].D());
//                qyString = QString::number(az.hv_TupleN2Y[i].D());

//                qDebug()<<"NX XPOS:"<<qxString;
//                qDebug()<<"NX YPOS:"<<qyString;

//            }

        }
        if (0 != (int(hv_Number3!=0)))
        {

              qthalconwin->setColor("yellow");
              qthalconwin->devDisplay(az.ho_NOregion);
              qthalconwin->setColor("green");

        }

}

void CammerWidget::on_pushButton_6_clicked()
{
 //   Controller::getInstance().onPlcRead(4526,3);
    //emit start2DCapture();
    HTuple  hv_Number1, hv_Number2,hv_Number3,hv_leng1,hv_leng2;
    VisionCheckResult1 cyazj=AZJ.cyAZJaction(CurrentImage);

  CountObj(cyazj.ho_NGregion, &hv_Number1);

  TupleLength(cyazj.hv_TupleNX,&hv_leng1);

   if (0 != (int(hv_Number1!=0)))
  {

      qthalconwin->setColor("red");
      qthalconwin->devDisplay(cyazj.ho_NGregion);
      qthalconwin->setColor("green");

      QString qxString;
      QString qyString;

      for(int i = 0; i < hv_leng1.I(); i++){
          qxString = QString::number(cyazj.hv_TupleNX[i].D());
          qyString = QString::number(cyazj.hv_TupleNY[i].D());
          //testResult(qxString.toFloat(), qyString.toFloat(),-125.0,1,3);
          //testResult(qxString.toFloat()+offsetx, qyString.toFloat()+offsety,offsetz,0,5);
          qDebug()<<"NX XPOS:"<<qxString;
          qDebug()<<"NX YPOS:"<<qyString;
      }

  }
   qDebug()<<"333";

  if(hv_Number1.I() == 0){
     // QLOG_DEBUG()<<"empty flag 6";
  //  testResult(0,0,0,0,6);
  }





}

void CammerWidget::on_pushButton_7_clicked()
{
    VisionCheckResult1 az=MB.MBaction(CurrentImage,grayMin,grayMax);
    qthalconwin->dispMessage(az.hv_gray);
}
