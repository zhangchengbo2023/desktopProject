#ifndef QTHALCONWIN_H
#define QTHALCONWIN_H

/*********************************************************************************
  *Version:      1.0
  *Date:	     2024.4.25
  *Description:  Halcon封装C++的可视化类
**********************************************************************************/


#include <QWidget>

#include "HalconCpp.h"
#include <vector>
#include<QString>
#include<QDebug>

using namespace HalconCpp;
using namespace std;

class QtHalconWin
{
public:
    QtHalconWin();
    QtHalconWin(QWidget * widget);

    void InitDisplay();

public:

    HTuple QString2Htuple(QString string);

    /**
     * @breif 函数简要说明-打开Halcon窗口并绑定widget
     * @param widget: halcon窗口打开在Qt中哪个Widget上
     */
    void openwindow(QWidget * widget);

    /**
     * @breif 打开一张图片并显示到窗口，输入图片路径
     * @param imageFile: 图片路径
     */
    HObject readimage(const QString imageFile);

    /**
     * @breif 在Halcon窗口上显示信息
     * @param hv_String:显示的信息
     * @param hv_Color:显示信息的颜色
     * @param hv_Row,hv_Column:显示信息的横纵坐标
     * @param hv_Box:显示信息有无底框
     * @param hv_CoordSystem:显示信息相对于image还是window
     */
    void dispMessage(HTuple hv_String,
                     HTuple hv_Color = "green",
                     HTuple hv_Row = 10, HTuple hv_Column = 10,
                     HTuple hv_Box = "false",
                     HTuple hv_CoordSystem = "image");

    /**
     * @breif 函数简要说明-  显示区域，图片，XLD
     * 对display做了改进,如果显示的是Image,则清空m_objs(当前窗口所显示的内容)
     * @param img: 区域，图片，XLD
     * @param objs:  对区域，图片，XLD集合objs 一一显示
     */
    void devDisplay(HObject& img);
    void devDisplay(vector<HObject>& objs);

    /**
     * @breif 函数简要说明-  显示十字
     */
    void dispCross(HTuple row,HTuple col,HTuple size,HTuple angle);

    /**
     * @breif 函数简要说明-  清空窗口
     */
    void clearWindow();

    /**
     * @breif 函数简要说明-  设置线宽
     * @param hv_Width: 线宽
     */
    void setLineWidth(HTuple hv_Width);

    /**
     * @breif 函数简要说明-  设置颜色
     * @param color: "green", "red", "blue"
     */
    void setColor(QString color);

    /**
     * @breif 函数简要说明-  设置颜色种类
     * @param color: 3 6 9
     */
    void setColored(int colorNum);

    /**
     * @breif 函数简要说明-  设置显示模式
     * @param drawType: (边框："margin", 填充："fill")
     */
    void setDraw(QString drawType);

    /**
     * @breif 函数简要说明-  设置字体
     * @param size: 字体大小
     * @param fontName: 字体名称
     * @param bold: 是否加粗
     * @param slant: 是否倾斜
     */
    void SetDisplayFont(HTuple size = 18,
                        HTuple fontName = "mono",
                        bool bold = true,
                        bool slant = false);

    /**
     * @breif 函数简要说明-  获得窗口句柄
     */
    HTuple getWindow();

private:
    /**
     * @breif 函数简要说明-显示HObject对象
     * @param obj: 显示的对象  区域，图片，XLD
     */
    void display(HObject& obj);
public:

    //窗口句柄
    HTuple hv_WindowHandle;
    //当前窗口所显示的内容(push_back对象是image,之前的会clear)
    vector<HObject> m_objs;

};

#endif // QTHALCONWIN_H
