#ifndef QTHALCONARITHMETIC_H
#define QTHALCONARITHMETIC_H

/*********************************************************************************
  *Version:      1.0
  *Date:	     2024.4.25
  *Description:  Halcon封装C++的算法类
  *note:         类模板分文件编写要将声明和实现写到同一个文件中,
  *              并更改后缀名为.hpp,hpp是约定的名称，并不是强制
**********************************************************************************/

#include "HalconCpp.h"
using namespace HalconCpp;
#include <QString>
#include <vector>
using namespace std;

//-------根据检测需求定义的检测结果数据结构------
enum ReState
{
    NO = -1,    //无产品
    NG,         //不良品
    OK          //良品
};

template <typename T>
class QtHalconArithmetic
{
public:

    //要纯虚函数,就必须要虚析构函数。
    virtual ~QtHalconArithmetic(){};

    /**
     * @breif 函数简要说明- 纯虚函数,用户必须继承该类并重写该函数,返回值用户自定义
     * @param srcImg: 待处理图像
     * @return T : 数据结构体
     */
    virtual T action(HObject srcImg) = 0;

    /**
     * @breif 函数简要说明- QString与HTuple互转
     * @param qString: qString
     */
    HTuple QString2HTuple(QString qString);
    QString HTuple2CString(HTuple hv_str);

    /**
     * @breif 函数简要说明- 读取文件夹图片,将图片路径放入容器中
     * @param filedir: 文件夹图片
     * @param vec: 容器
     */
    void ListImages(QString filedir, vector<QString>* vec);

    /**
     * @breif 函数简要说明- 读取图片
     * @param imageFile: 图片路径
     */
    HObject readimage(const QString imageFile);

    /**
     * @breif 函数简要说明-           相机坐标转机械手坐标
     * @param HandEyeFilePath:      手眼标定tup文件
     * @param cameraRow,cameraCol:  相机r,c坐标
     * @param RobotArmX,RobotArmY:  机械手x,y坐标
     */
    BOOL Camera2RobotArm(const QString HandEyeFilePath,
                             HTuple cameraRow, HTuple cameraCol,
                             HTuple& RobotArmX, HTuple& RobotArmY);
};

//类外实现
template<typename T>
HTuple QtHalconArithmetic<T>::QString2HTuple(QString qString)
{
    QByteArray ba = qString.toLocal8Bit();
    HalconCpp::HTuple hv_string(ba.data());

    //或者
    //std::string str = string.toStdString();
    //HTuple hv_string;
    //hv_string = str.c_str();
    return hv_string;
}

template<typename T>
QString QtHalconArithmetic<T>::HTuple2CString(HTuple hv_str)
{
    //0表示HTuple中第一个变量
    QString qstr;
    qstr = hv_str[0].S();
    return qstr;
}

template<typename T>
void QtHalconArithmetic<T>::ListImages(QString filedir, vector<QString> *vec)
{
    vec->clear();
    HTuple hv_filedir, hv_ImageFiles, hv_Index;
    hv_filedir = QString2HTuple(filedir);
    ListFiles(hv_filedir, ((HTuple("files").Append("follow_links")).Append("recursive")),
        &hv_ImageFiles);
    TupleRegexpSelect(hv_ImageFiles, (HTuple("\\.(tif|tiff|gif|bmp|jpg|jpeg|jp2|png|pcx|pgm|ppm|pbm|xwd|ima|hobj)$").Append("ignore_case")),
        &hv_ImageFiles);
    HTuple end_val3 = (hv_ImageFiles.TupleLength()) - 1;
    HTuple step_val3 = 1;
    for (hv_Index = 0; hv_Index.Continue(end_val3, step_val3); hv_Index += step_val3)
    {
        QString str;
        str = hv_ImageFiles[hv_Index].S();
        vec->push_back(str);
    }
}

template<typename T>
HObject QtHalconArithmetic<T>::readimage(const QString imageFile)
{
    HObject image;
    HTuple imagepath = QString2Htuple(imageFile);
    ReadImage(&image, imagepath);
    return image;
}

template<typename T>
BOOL QtHalconArithmetic<T>::Camera2RobotArm(const QString HandEyeFilePath, HTuple cameraRow, HTuple cameraCol, HTuple &RobotArmX, HTuple &RobotArmY)
{
    HTuple EyeHandMatrix;
    HTuple Path;
    Path =  CString2HTuple(HandEyeFilePath);
    ReadTuple(Path, &EyeHandMatrix);
    try
    {
        AffineTransPoint2d(EyeHandMatrix, cameraRow, cameraCol, &RobotArmX, &RobotArmY);
        return TRUE;
    }
    catch (...)
    {
        return FALSE;
    }
    return FALSE;
}

#endif // QTHALCONARITHMETIC_H
