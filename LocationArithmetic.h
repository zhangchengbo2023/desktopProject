#ifndef LOCATIONARITHMETIC_H
#define LOCATIONARITHMETIC_H

#include "QtHalconArithmetic.hpp"
/*********************************************************************************
该文件演示了使用者如何去使用QtHalconArithmetic抽象类
    1.定义一个结构体VisionCheckResult
    2.继承QtHalconArithmetic
    class LocationArithmetic : public QtHalconArithmetic<VisionCheckResult>{...};
    3.重写QtHalconArithmetic类的action成员函数
**********************************************************************************/

//1.使用者自己定义图像处理返回数据包

struct VisionCheckResult
{
    HObject  ho_NOregion;
    HObject ho_NGregion;
    HObject ho_NGcross;
    HObject  ho_Mixregion;
    HTuple hv_TupleNX, hv_TupleNY;
    HTuple hv_TupleMX, hv_TupleMY;
    HTuple hv_TupleM2Y,hv_TupleM2X,hv_TupleN2X, hv_TupleN2Y;
};
struct VisionCheckResult1
{

    HObject  ho_NOregion;
    HObject  ho_NGregion;
    HObject  ho_NGcross;
    HObject  ho_Mixregion;
    HTuple hv_TupleNX, hv_TupleNY,hv_NGAngle;
    HTuple hv_TupleMX, hv_TupleMY;
    HTuple hv_TupleM2Y,hv_TupleM2X,hv_TupleN2X, hv_TupleN2Y;
    HTuple hv_gray;
};
struct VisionCheckResult2
{

};

struct VisionCheckResult3
{

};
class JZLocationArithmetic : public QtHalconArithmetic<VisionCheckResult>
{
public:


    JZLocationArithmetic();
    void createserchmodel(HTuple  hv_WindowHandle);
    bool TJTMaction(HObject ho_Image);
    VisionCheckResult CYaction(HObject ho_Image);
    //2.使用者自己定义图像处理算法
    VisionCheckResult action(HObject ho_Image) override;
    VisionCheckResult JZsearchmodaction(HObject ho_Image,int min,int max);
};

class AZJLocationArithmetic2: public QtHalconArithmetic<VisionCheckResult1>
{
public:

    VisionCheckResult1 action(HObject ho_Image) override;
    AZJLocationArithmetic2();
    VisionCheckResult1 MBCYaction(HObject ho_Image);
    VisionCheckResult1 AZJaction(HObject ho_Image,int min,int max);
    VisionCheckResult1 MBaction(HObject ho_Image,int min,int max);
    VisionCheckResult1 detecType(HObject ho_Image);
    VisionCheckResult1 cyAZJaction(HObject ho_Image);
};

class LocationArithmetic3: public QtHalconArithmetic<VisionCheckResult>
{
public:
    LocationArithmetic3();

    VisionCheckResult action(HObject ho_Image) override;
};

class LocationArithmetic4: public QtHalconArithmetic<VisionCheckResult1>
{

public:
    LocationArithmetic4();
    HObject  ho_Image, ho_Circle, ho_ImageModel, ho_GrayImage;
    HObject  ho_Cross, ho_ContCircle,ho_Crosss,ho_Cires;

    // Local control variables
    HTuple  hv_Channels, hv_WindowHandle, hv_Row;
    HTuple  hv_Column, hv_Radius, hv_PI, hv_ImageFiles;
    HTuple  hv_Index, hv_start, hv_ResRow, hv_ResColumn, hv_Res_rotAngle;
    HTuple  hv_ResScore, hv_end, hv_time, hv_i;
    HTuple  hv_ModelID111;

    //2.使用者自己定义图像处理算法
    VisionCheckResult1 action(HObject ho_Image) override;

    //创建模板
    void setModel(HTuple modelID);

};

class LocationArithmetic5: public QtHalconArithmetic<VisionCheckResult3>
{

public:
    LocationArithmetic5();
    HObject  ho_ROI_0, ho_ImageReduced1;
    HObject  ho_GrayImage1, ho_Regions1, ho_RegionOpening1, ho_Rectangle1;
    HObject  ho_ImageAffinTrans, ho_wuo, ho_Rectangle2, ho_EmptyObject,ho_rere;
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
    HTuple  hv_Row, hv_Column, hv_RowOriginal, hv_ColumnOriginal;
    HTuple  hv_mixRow, hv_mixColumn, hv_T2, hv_RunTime;
    HTuple  hv_ModelID1;
    HTuple  hv_mRow1_1, hv_mColumn1_1, hv_mRow1_2,hv_mColumn1_2;
    HTuple  hv_mrCol2, hv_mlRow1, hv_mlCol1, hv_mrRow2;
    HTuple  hv_Row2_1, hv_Column2_1, hv_Row2_2, hv_Column2_2;
    //2.使用者自己定义图像处理算法
    VisionCheckResult3 action(HObject ho_Image) override;
    VisionCheckResult3 model(HObject ho_Image,HTuple hv_win) ;

};



#endif // LOCATIONARITHMETIC_H
