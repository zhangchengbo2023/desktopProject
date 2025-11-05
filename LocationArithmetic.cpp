#include "LocationArithmetic.h"
#include <QDebug>
#include <QFile>
#include <QCoreApplication>
#include <QDir>
#include "controller.h"
JZLocationArithmetic::JZLocationArithmetic()
{

}

void JZLocationArithmetic::createserchmodel(HTuple  hv_WindowHandle)
{

}

bool JZLocationArithmetic::TJTMaction(HObject ho_Image)
{
   int grey = Controller::getInstance().cfg.grey_minimum;
   int area = Controller::getInstance().cfg.grey_lightErea;
   qDebug()<<"action:grey val="<<grey<<",area val="<<area;
   VisionCheckResult re;
   HObject  ho_ImageMedian, ho_Regions,ho_CloRegions;
   HObject  ho_ConnectedRegions, ho_SelectedRegions;

   // Local control variables
   HTuple  hv_ImageFiles, hv_Index, hv_Number;

     MedianImage(ho_Image, &ho_ImageMedian, "circle", 3, "mirrored");

       Threshold(ho_ImageMedian, &ho_Regions, 115, 255);

       ClosingCircle(ho_Regions,&ho_CloRegions,3);
       Connection(ho_Regions, &ho_ConnectedRegions);
       SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and", 400, 99999);
       CountObj(ho_SelectedRegions, &hv_Number);
       if (0 != (int(hv_Number==0)))
       {
           return true;
       }
       else if (0 != (int(hv_Number!=0)))
       {
           return false;
       }


}

VisionCheckResult JZLocationArithmetic::CYaction(HObject ho_Image)
{


   VisionCheckResult re;
   HObject  ho_medicine_regions, ho_ImageA;
   HObject  ho_ImageB, ho_ImageC, ho_ImageResult1, ho_ImageResult2;
   HObject  ho_ImageResult3, ho_GrayImage1, ho_ImageMedian1;
   HObject  ho_Circle, ho_RegionDifference, ho_ImageSurface;
   HObject  ho_ImageSub, ho_NGregion, ho_Mixregion, ho_NOregion;
   HObject  ho_ObjectSelected, ho_Rectangle1, ho_ImageReduced2;
   HObject  ho_ImageHun, ho_Cross,ho_NGcross;

   // Local control variables
   HTuple  hv_ImageFiles, hv_Index, hv_WindowHandle1;
   HTuple  hv_HomMat2Dto3D, hv_ZENID, hv_Number, hv_Width;
   HTuple  hv_Height, hv_Alpha, hv_Beta, hv_Gamma, hv_Delta;
   HTuple  hv_Epsilon, hv_Zeta, hv_Area, hv_Row, hv_Column;
   HTuple  hv_Mix1Row, hv_Mix1Col, hv_NG1Row, hv_NG1Col, hv_NGAngle;
   HTuple  hv_NGscore, hv_Index2, hv_Row3, hv_Column3, hv_Row11;
   HTuple  hv_Column11, hv_Row2, hv_Column2, hv_Mean, hv_Deviation;
   HTuple  hv_Qx, hv_Qy, hv_FRow, hv_FColumn, hv_Angle, hv_Score;
   HTuple  hv_Deg, hv_Number1, hv_Number2, hv_Number3;



   ReadTuple("MB2Dto3D.tup", &hv_HomMat2Dto3D);
   ReadShapeModel("Itemodel", &hv_ZENID);
   ReadObject(&ho_medicine_regions, "Serchmodel");
   Decompose3(ho_Image, &ho_ImageA, &ho_ImageB, &ho_ImageC);
   TransFromRgb(ho_ImageA, ho_ImageB, ho_ImageC, &ho_ImageResult1, &ho_ImageResult2,
       &ho_ImageResult3, "hsv");
   Rgb1ToGray(ho_Image, &ho_GrayImage1);
   MedianImage(ho_GrayImage1, &ho_ImageMedian1, "circle", 5, "mirrored");
   CountObj(ho_medicine_regions, &hv_Number);


   GetImageSize(ho_ImageMedian1, &hv_Width, &hv_Height);
   GenCircle(&ho_Circle, 495, 630, 350.5);
   Difference(ho_ImageMedian1, ho_Circle, &ho_RegionDifference);
   FitSurfaceSecondOrder(ho_RegionDifference, ho_ImageMedian1, "regression", 5,
       2, &hv_Alpha, &hv_Beta, &hv_Gamma, &hv_Delta, &hv_Epsilon, &hv_Zeta);
   AreaCenter(ho_RegionDifference, &hv_Area, &hv_Row, &hv_Column);
   GenImageSurfaceSecondOrder(&ho_ImageSurface, "byte", hv_Alpha, hv_Beta, hv_Gamma,
       hv_Delta, hv_Epsilon, hv_Zeta, hv_Row, hv_Column, hv_Width, hv_Height);
   SubImage(ho_ImageMedian1, ho_ImageSurface, &ho_ImageSub, 1, 128);

   hv_Mix1Row = HTuple();
   hv_Mix1Col = HTuple();
   hv_NG1Row = HTuple();
   hv_NG1Col = HTuple();
   hv_NGAngle = HTuple();
   hv_NGscore = HTuple();
   //反料
   GenEmptyObj(&ho_NGregion);
   //混料
   GenEmptyObj(&ho_Mixregion);
   //空料
   GenEmptyObj(&ho_NOregion);

    GenEmptyObj(&ho_NGcross);

   {
   HTuple end_val40 = hv_Number;
   HTuple step_val40 = 1;
   for (hv_Index2=1; hv_Index2.Continue(end_val40, step_val40); hv_Index2 += step_val40)
   {
     SelectObj(ho_medicine_regions, &ho_ObjectSelected, hv_Index2);
     AreaCenter(ho_ObjectSelected, &hv_Area, &hv_Row3, &hv_Column3);
     SmallestRectangle1(ho_ObjectSelected, &hv_Row11, &hv_Column11, &hv_Row2,
         &hv_Column2);
     GenRectangle1(&ho_Rectangle1, hv_Row11, hv_Column11, hv_Row2, hv_Column2);
     ReduceDomain(ho_ImageSub, ho_ObjectSelected, &ho_ImageReduced2);
     ReduceDomain(ho_ImageResult1, ho_ObjectSelected, &ho_ImageHun);
     Intensity(ho_ObjectSelected, ho_ImageReduced2, &hv_Mean, &hv_Deviation);

     FindShapeModel(ho_ImageReduced2, hv_ZENID, -HTuple(10).TupleRad(), HTuple(720).TupleRad(),
           0.5, 1, 0.5, "least_squares", 0, 0.9, &hv_FRow, &hv_FColumn, &hv_Angle,
           &hv_Score);
       if (0 != (hv_FRow.TupleLength()))
       {
         TupleDeg(hv_Angle, &hv_Deg);
        AffineTransPoint2d(hv_HomMat2Dto3D, hv_FRow, hv_FColumn, &hv_Qx,
                 &hv_Qy);
        TupleConcat(hv_NG1Row, hv_Qx, &hv_NG1Row);
        TupleConcat(hv_NG1Col, hv_Qy, &hv_NG1Col);
        TupleConcat(hv_NGAngle, hv_Deg, &hv_NGAngle);
        TupleConcat(hv_NGscore, hv_Score, &hv_NGscore);
        GenRectangle1(&ho_Rectangle1, hv_Row11, hv_Column11, hv_Row2, hv_Column2);
        GenCrossContourXld(&ho_Cross, hv_FRow, hv_FColumn, 50, hv_Angle);
        ConcatObj(ho_NGregion, ho_Rectangle1, &ho_NGregion);
        ConcatObj(ho_NGcross, ho_Cross, &ho_NGcross);

       }
       else if (0 != (int((hv_FRow.TupleLength())==0)))
       {
         ConcatObj(ho_NOregion, ho_Rectangle1, &ho_NOregion);

       }
     }


   }

   CountObj(ho_NGregion, &hv_Number2);
   CountObj(ho_NOregion, &hv_Number3);

   re.ho_NGregion=ho_NGregion;
   re.ho_NGcross=ho_NGcross;
   re.ho_NOregion=ho_NOregion;
   re.ho_Mixregion=ho_Mixregion;

   re.hv_TupleMX=hv_Mix1Row;
   re.hv_TupleMY=hv_Mix1Col;
   re.hv_TupleNX=hv_NG1Row;
   re.hv_TupleNY=hv_NG1Col;
   return re;
}

VisionCheckResult JZLocationArithmetic::action(HObject ho_Image)
{
   //声明处理结果
   HObject  ho_wuo, ho_NGregion, ho_Mixregion;
   HObject  ho_NOregion, ho_ObjectSelected, ho_ImageReduced;
   HObject  ho_GrayImage, ho_Rectangle5, ho_ImageEquHisto, ho_ImageMedian;
   HObject  ho_Regions, ho_ConnectedRegions, ho_SelectedRegions;
   HObject  ho_RegionUnion, ho_RegionDilation, ho_ConnectedRegions1;
   HObject  ho_SortedRegions, ho_up, ho_down;

   // Local control variables
   HTuple  hv_TupleMX, hv_TupleMY,hv_TupleM2Y,hv_TupleM2X,hv_TupleN2X, hv_TupleN2Y;
   HTuple  hv_ImageFiles, hv_Index, hv_WindowHandle;
   HTuple  hv_HomMat2Dto3D, hv_Number, hv_TupleNX, hv_TupleNY;
   HTuple  hv_Index2, hv_Area1, hv_aRow, hv_aColumn, hv_NGR1;
   HTuple  hv_NGC1, hv_NGR2, hv_NGC2, hv_Mean, hv_Deviation;
   HTuple  hv_NumRegions, hv_upArea, hv_upRow, hv_upColumn;
   HTuple  hv_downArea, hv_downRow, hv_downColumn, hv_RowOriginal;
   HTuple  hv_ColumnOriginal, hv_Qx, hv_Qy, hv_Number1, hv_Number2;
   HTuple  hv_Number3, hv_Length;
   VisionCheckResult re;

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
       //ReadObject(&ho_wuo, filePath);


       try {
               path = proDir + "/files/HandEyeCalibrationMatrix.tup";
              HTuple hfilePath = path.toStdString().c_str();
               ReadTuple(hfilePath, &hv_HomMat2Dto3D);

       } catch (HException &ex) {
              qDebug()<<"htuple read failed";
       }

       qDebug()<<"nnn2";
       CountObj(ho_wuo, &hv_Number);
       //反料
       GenEmptyObj(&ho_NGregion);
       //混料
       GenEmptyObj(&ho_Mixregion);
       //空料
       GenEmptyObj(&ho_NOregion);
       hv_TupleNX = HTuple();
       hv_TupleNY = HTuple();
        hv_TupleMX= HTuple();
        hv_TupleMY= HTuple();

       HTuple end_val17 = hv_Number;
       HTuple step_val17 = 1;
       for (hv_Index2=1; hv_Index2.Continue(end_val17, step_val17); hv_Index2 += step_val17)
       {

         SelectObj(ho_wuo, &ho_ObjectSelected, hv_Index2);
         AreaCenter(ho_ObjectSelected, &hv_Area1, &hv_aRow, &hv_aColumn);
         ReduceDomain(ho_Image, ho_ObjectSelected, &ho_ImageReduced);

         SmallestRectangle1(ho_ObjectSelected, &hv_NGR1, &hv_NGC1, &hv_NGR2, &hv_NGC2);
         Rgb1ToGray(ho_ImageReduced, &ho_GrayImage);
         Intensity(ho_ObjectSelected, ho_ImageReduced, &hv_Mean, &hv_Deviation);
         if (0 != (int(hv_Mean>150)))
         {
           GenRectangle1(&ho_Rectangle5, hv_NGR1, hv_NGC1, hv_NGR2, hv_NGC2);
           AffineTransPoint2d(hv_HomMat2Dto3D, hv_aRow, hv_aColumn,
               &hv_Qx, &hv_Qy);
           ConcatObj(ho_Mixregion, ho_Rectangle5, &ho_Mixregion);
           if(0 != (int(hv_Index2<=12)))
           {
               TupleConcat(hv_TupleMX, hv_Qx, &hv_TupleMX);
               TupleConcat(hv_TupleMY, hv_Qy, &hv_TupleMY);
           }
           else if(0 != (int(hv_Index2>12)))
           {
               TupleConcat(hv_TupleM2X, hv_Qx, &hv_TupleM2X);
               TupleConcat(hv_TupleM2Y, hv_Qy, &hv_TupleM2Y);
           }

         }
         EquHistoImage(ho_GrayImage, &ho_ImageEquHisto);
         MedianImage(ho_ImageEquHisto, &ho_ImageMedian, "circle", 10, "mirrored");

         Threshold(ho_ImageMedian, &ho_Regions, 0, 40);
         Connection(ho_Regions, &ho_ConnectedRegions);
         SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, (HTuple("circularity").Append("area")),
             "and", (HTuple(0.78).Append(600)), (HTuple(1).Append(1000)));
         CountObj(ho_SelectedRegions, &hv_NumRegions);

         //360 890 1420 1950
         if (0 != (HTuple(int(hv_NumRegions==3)).TupleAnd(int(hv_Mean<150))))
         {
           Union1(ho_SelectedRegions, &ho_RegionUnion);
           DilationRectangle1(ho_RegionUnion, &ho_RegionDilation, 1, 40);
           Connection(ho_RegionDilation, &ho_ConnectedRegions1);
           SortRegion(ho_ConnectedRegions1, &ho_SortedRegions, "first_point", "true",
               "row");

           SelectObj(ho_SortedRegions, &ho_up, 1);
           AreaCenter(ho_up, &hv_upArea, &hv_upRow, &hv_upColumn);
           SelectObj(ho_SortedRegions, &ho_down, 2);
           AreaCenter(ho_down, &hv_downArea, &hv_downRow, &hv_downColumn);


           if (0 != (int(hv_upArea>hv_downArea)))
           {
             hv_RowOriginal = hv_upRow;
             hv_ColumnOriginal = hv_upColumn;
           }
           else if (0 != (int(hv_downArea>hv_upArea)))
           {
             hv_RowOriginal = hv_downRow;
             hv_ColumnOriginal = hv_downColumn;
           }
           //判断正反

             if (0 != (int(hv_upArea<hv_downArea)))
             {
               GenRectangle1(&ho_Rectangle5, hv_NGR1, hv_NGC1, hv_NGR2, hv_NGC2);
               ConcatObj(ho_NGregion, ho_Rectangle5, &ho_NGregion);

               AffineTransPoint2d(hv_HomMat2Dto3D, hv_RowOriginal, hv_ColumnOriginal,
                   &hv_Qx, &hv_Qy);

               //
               if(0 != (int(hv_Index2<=12)))
               {
                   TupleConcat(hv_TupleNX, hv_Qx, &hv_TupleNX);
                   TupleConcat(hv_TupleNY, hv_Qy, &hv_TupleNY);
               }
               else if(0 != (int(hv_Index2>12)))
               {
                   TupleConcat(hv_TupleN2X, hv_Qx, &hv_TupleN2X);
                   TupleConcat(hv_TupleN2Y, hv_Qy, &hv_TupleN2Y);
               }
             }
         else if (0 != (HTuple(int(hv_NumRegions!=3)).TupleAnd(int(hv_Mean<150))))
         {
           //没有物料/或有缺陷
           GenRectangle1(&ho_Rectangle5, hv_NGR1, hv_NGC1, hv_NGR2, hv_NGC2);
           ConcatObj(ho_NOregion, ho_Rectangle5, &ho_NOregion);
         }

       }

            }
       //71

       CountObj(ho_Mixregion, &hv_Number1);
       CountObj(ho_NGregion, &hv_Number2);
       CountObj(ho_NOregion, &hv_Number3);

       re.ho_NGregion=ho_NGregion;
       re.ho_NOregion=ho_NOregion;
       re.ho_Mixregion=ho_Mixregion;
       re.hv_TupleMX=hv_TupleMX;
       re.hv_TupleMY=hv_TupleMY;
       re.hv_TupleNX=hv_TupleNX;
       re.hv_TupleNY=hv_TupleNY;

       re.hv_TupleM2X=hv_TupleM2X;
       re.hv_TupleM2Y=hv_TupleM2Y;
       re.hv_TupleN2X=hv_TupleN2X;
       re.hv_TupleN2Y=hv_TupleN2Y;

       TupleLength(hv_TupleNY, &hv_Length);


       return re;
}

VisionCheckResult JZLocationArithmetic::JZsearchmodaction(HObject ho_Image,int min,int max)
{
   VisionCheckResult re;

   HObject  ho_medicine_regions, ho_ImageA;
   HObject  ho_ImageB, ho_ImageC, ho_ImageResult1, ho_ImageResult2;
   HObject  ho_ImageResult3, ho_GrayImage1, ho_ImageMedian1;
   HObject  ho_Circle, ho_RegionDifference, ho_ImageSurface;
   HObject  ho_ImageSub, ho_NGregion, ho_Mixregion, ho_NOregion;
   HObject  ho_ObjectSelected, ho_Rectangle1, ho_ImageReduced2;
   HObject  ho_ImageHun, ho_Cross,ho_NGcross;

   // Local control variables
   HTuple  hv_ImageFiles, hv_Index, hv_WindowHandle1;
   HTuple  hv_HomMat2Dto3D, hv_ZENID, hv_Number, hv_Width;
   HTuple  hv_Height, hv_Alpha, hv_Beta, hv_Gamma, hv_Delta;
   HTuple  hv_Epsilon, hv_Zeta, hv_Area, hv_Row, hv_Column;
   HTuple  hv_Mix1Row, hv_Mix1Col, hv_NG1Row, hv_NG1Col, hv_NGAngle;
   HTuple  hv_NGscore, hv_Index2, hv_Row3, hv_Column3, hv_Row11;
   HTuple  hv_Column11, hv_Row2, hv_Column2, hv_Mean, hv_Deviation;
   HTuple  hv_Qx, hv_Qy, hv_FRow, hv_FColumn, hv_Angle, hv_Score;
   HTuple  hv_Deg, hv_Number1, hv_Number2, hv_Number3;


   ReadTuple("MB2Dto3D.tup", &hv_HomMat2Dto3D);
   ReadShapeModel("Itemodel", &hv_ZENID);
   ReadObject(&ho_medicine_regions, "Serchmodel");
   Decompose3(ho_Image, &ho_ImageA, &ho_ImageB, &ho_ImageC);
   TransFromRgb(ho_ImageA, ho_ImageB, ho_ImageC, &ho_ImageResult1, &ho_ImageResult2,
       &ho_ImageResult3, "hsv");
   Rgb1ToGray(ho_Image, &ho_GrayImage1);
   MedianImage(ho_GrayImage1, &ho_ImageMedian1, "circle", 5, "mirrored");
   CountObj(ho_medicine_regions, &hv_Number);


   GetImageSize(ho_ImageMedian1, &hv_Width, &hv_Height);
   GenCircle(&ho_Circle, 495, 630, 350.5);
   Difference(ho_ImageMedian1, ho_Circle, &ho_RegionDifference);
   FitSurfaceSecondOrder(ho_RegionDifference, ho_ImageMedian1, "regression", 5,
       2, &hv_Alpha, &hv_Beta, &hv_Gamma, &hv_Delta, &hv_Epsilon, &hv_Zeta);
   AreaCenter(ho_RegionDifference, &hv_Area, &hv_Row, &hv_Column);
   GenImageSurfaceSecondOrder(&ho_ImageSurface, "byte", hv_Alpha, hv_Beta, hv_Gamma,
       hv_Delta, hv_Epsilon, hv_Zeta, hv_Row, hv_Column, hv_Width, hv_Height);
   SubImage(ho_ImageMedian1, ho_ImageSurface, &ho_ImageSub, 1, 128);

   hv_Mix1Row = HTuple();
   hv_Mix1Col = HTuple();
   hv_NG1Row = HTuple();
   hv_NG1Col = HTuple();
   hv_NGAngle = HTuple();
   hv_NGscore = HTuple();
   //反料
   GenEmptyObj(&ho_NGregion);
   GenEmptyObj(&ho_NGcross);
   //混料
   GenEmptyObj(&ho_Mixregion);
   //空料
   GenEmptyObj(&ho_NOregion);
   {
   HTuple end_val40 = hv_Number;
   HTuple step_val40 = 1;
   for (hv_Index2=1; hv_Index2.Continue(end_val40, step_val40); hv_Index2 += step_val40)
   {
     SelectObj(ho_medicine_regions, &ho_ObjectSelected, hv_Index2);
     AreaCenter(ho_ObjectSelected, &hv_Area, &hv_Row3, &hv_Column3);
     SmallestRectangle1(ho_ObjectSelected, &hv_Row11, &hv_Column11, &hv_Row2,
         &hv_Column2);
     GenRectangle1(&ho_Rectangle1, hv_Row11, hv_Column11, hv_Row2, hv_Column2);
     ReduceDomain(ho_ImageSub, ho_ObjectSelected, &ho_ImageReduced2);
     ReduceDomain(ho_ImageResult1, ho_ObjectSelected, &ho_ImageHun);
     Intensity(ho_ObjectSelected, ho_ImageReduced2, &hv_Mean, &hv_Deviation);
     //混料判断标准


     if (0 != (HTuple(int(hv_Mean<min)).TupleOr(int(hv_Mean>max))))
     {
       GenRectangle1(&ho_Rectangle1, hv_Row11, hv_Column11, hv_Row2, hv_Column2);
       ConcatObj(ho_Mixregion, ho_Rectangle1, &ho_Mixregion);
       AffineTransPoint2d(hv_HomMat2Dto3D, hv_Row3, hv_Column3, &hv_Qx, &hv_Qy);
       TupleConcat(hv_Mix1Row, hv_Qx, &hv_Mix1Row);
       TupleConcat(hv_Mix1Col, hv_Qy, &hv_Mix1Col);
     }
     else if (0 != (HTuple(int(hv_Mean>min)).TupleAnd(int(hv_Mean<max))))
     {

       FindShapeModel(ho_ImageReduced2, hv_ZENID, HTuple(0).TupleRad(), HTuple(360).TupleRad(),
           0.5, 1, 0.5, "least_squares", 0, 0.9, &hv_FRow, &hv_FColumn, &hv_Angle,
           &hv_Score);
       if (0 != (hv_FRow.TupleLength()))
       {
         TupleDeg(hv_Angle, &hv_Deg);

         if (0 != (int(hv_Index2<=(hv_Number/2))))
         {
           if (0 != (HTuple(HTuple(int(0<=(hv_Deg.TupleAbs()))).TupleAnd(int((hv_Deg.TupleAbs())<=160)))
                     .TupleOr(HTuple(int(200<=(hv_Deg.TupleAbs()))).TupleAnd(int((hv_Deg.TupleAbs())<=360)))))
           {
             AffineTransPoint2d(hv_HomMat2Dto3D, hv_FRow, hv_FColumn, &hv_Qx,&hv_Qy);
             TupleConcat(hv_NG1Row, hv_Qx, &hv_NG1Row);
             TupleConcat(hv_NG1Col, hv_Qy, &hv_NG1Col);
             TupleConcat(hv_NGAngle, hv_Deg, &hv_NGAngle);
             TupleConcat(hv_NGscore, hv_Score, &hv_NGscore);
             GenRectangle1(&ho_Rectangle1, hv_Row11, hv_Column11, hv_Row2, hv_Column2);
             GenCrossContourXld(&ho_Cross, hv_FRow, hv_FColumn, 50, hv_Angle);
             ConcatObj(ho_NGcross, ho_Cross, &ho_NGcross);
             ConcatObj(ho_NGregion, ho_Rectangle1, &ho_NGregion);

           }
         }
         else if (0 != (int(hv_Index2>(hv_Number/2))))
         {
             //前两排和后两排方向一致
            // if (0 != (HTuple(HTuple(int(0<=(hv_Deg.TupleAbs()))).TupleAnd(int((hv_Deg.TupleAbs())<=160)))
              //         .TupleOr(HTuple(int(200<=(hv_Deg.TupleAbs()))).TupleAnd(int((hv_Deg.TupleAbs())<=360)))))
             //前两排和后两排方向不一致
            if (0 != (HTuple(int(160<=(hv_Deg.TupleAbs()))).TupleAnd(int((hv_Deg.TupleAbs())<=340))))
             {
               AffineTransPoint2d(hv_HomMat2Dto3D, hv_FRow, hv_FColumn, &hv_Qx,&hv_Qy);
               TupleConcat(hv_NG1Row, hv_Qx, &hv_NG1Row);
               TupleConcat(hv_NG1Col, hv_Qy, &hv_NG1Col);
               TupleConcat(hv_NGAngle, hv_Deg, &hv_NGAngle);
               TupleConcat(hv_NGscore, hv_Score, &hv_NGscore);
               GenRectangle1(&ho_Rectangle1, hv_Row11, hv_Column11, hv_Row2, hv_Column2);
               GenCrossContourXld(&ho_Cross, hv_FRow, hv_FColumn, 50, hv_Angle);
               ConcatObj(ho_NGregion, ho_Rectangle1, &ho_NGregion);
                ConcatObj(ho_NGcross, ho_Cross, &ho_NGcross);
             }
         }
       }
       else if (0 != (int((hv_FRow.TupleLength())==0)))
       {
         ConcatObj(ho_NOregion, ho_Rectangle1, &ho_NOregion);

       }
     }

   }
   }
   CountObj(ho_Mixregion, &hv_Number1);
   CountObj(ho_NGregion, &hv_Number2);
   CountObj(ho_NOregion, &hv_Number3);

   re.ho_NGregion=ho_NGregion;
   re.ho_NGcross=ho_NGcross;
   re.ho_NOregion=ho_NOregion;
   re.ho_Mixregion=ho_Mixregion;

   re.hv_TupleMX=hv_Mix1Row;
   re.hv_TupleMY=hv_Mix1Col;
   re.hv_TupleNX=hv_NG1Row;
   re.hv_TupleNY=hv_NG1Col;


   return re;
}


VisionCheckResult1 AZJLocationArithmetic2::action(HObject ho_Image)
{
VisionCheckResult1 re;
return re;
}

AZJLocationArithmetic2::AZJLocationArithmetic2()
{

}

VisionCheckResult1  AZJLocationArithmetic2::AZJaction(HObject ho_Image,int min,int max)
{
   //声明处理结果
   VisionCheckResult1 re;

   HObject  ho_medicine_regions, ho_ImageA;
   HObject  ho_ImageB, ho_ImageC, ho_ImageResult1, ho_ImageResult2;
   HObject  ho_ImageResult3, ho_GrayImage1, ho_ImageMedian1;
   HObject  ho_Circle, ho_RegionDifference, ho_ImageSurface;
   HObject  ho_ImageSub, ho_NGregion, ho_Mixregion, ho_NOregion;
   HObject  ho_ObjectSelected, ho_Rectangle1, ho_ImageReduced2;
   HObject  ho_ImageHun, ho_Cross,ho_NGcross;

   // Local control variables
   HTuple  hv_ImageFiles, hv_Index, hv_WindowHandle1;
   HTuple  hv_HomMat2Dto3D, hv_ZENID, hv_Number, hv_Width;
   HTuple  hv_Height, hv_Alpha, hv_Beta, hv_Gamma, hv_Delta;
   HTuple  hv_Epsilon, hv_Zeta, hv_Area, hv_Row, hv_Column;
   HTuple  hv_Mix1Row, hv_Mix1Col, hv_NG1Row, hv_NG1Col, hv_NGAngle;
   HTuple  hv_NGscore, hv_Index2, hv_Row3, hv_Column3, hv_Row11;
   HTuple  hv_Column11, hv_Row2, hv_Column2, hv_Mean, hv_Deviation;
   HTuple  hv_Qx, hv_Qy, hv_FRow, hv_FColumn, hv_Angle, hv_Score;
   HTuple  hv_Deg, hv_Number1, hv_Number2, hv_Number3;


   ReadTuple("MB2Dto3D.tup", &hv_HomMat2Dto3D);
   ReadShapeModel("Itemodel", &hv_ZENID);
   ReadObject(&ho_medicine_regions, "Serchmodel");
   Decompose3(ho_Image, &ho_ImageA, &ho_ImageB, &ho_ImageC);
   TransFromRgb(ho_ImageA, ho_ImageB, ho_ImageC, &ho_ImageResult1, &ho_ImageResult2,
       &ho_ImageResult3, "hsv");
   Rgb1ToGray(ho_Image, &ho_GrayImage1);
   MedianImage(ho_GrayImage1, &ho_ImageMedian1, "circle", 5, "mirrored");
   CountObj(ho_medicine_regions, &hv_Number);


   GetImageSize(ho_ImageMedian1, &hv_Width, &hv_Height);
   GenCircle(&ho_Circle, 495, 630, 350.5);
   Difference(ho_ImageMedian1, ho_Circle, &ho_RegionDifference);
   FitSurfaceSecondOrder(ho_RegionDifference, ho_ImageMedian1, "regression", 5,
       2, &hv_Alpha, &hv_Beta, &hv_Gamma, &hv_Delta, &hv_Epsilon, &hv_Zeta);
   AreaCenter(ho_RegionDifference, &hv_Area, &hv_Row, &hv_Column);
   GenImageSurfaceSecondOrder(&ho_ImageSurface, "byte", hv_Alpha, hv_Beta, hv_Gamma,
       hv_Delta, hv_Epsilon, hv_Zeta, hv_Row, hv_Column, hv_Width, hv_Height);
   SubImage(ho_ImageMedian1, ho_ImageSurface, &ho_ImageSub, 1, 128);

   hv_Mix1Row = HTuple();
   hv_Mix1Col = HTuple();
   hv_NG1Row = HTuple();
   hv_NG1Col = HTuple();
   hv_NGAngle = HTuple();
   hv_NGscore = HTuple();
   //反料
   GenEmptyObj(&ho_NGregion);
   GenEmptyObj(&ho_NGcross);
   //混料
   GenEmptyObj(&ho_Mixregion);
   //空料
   GenEmptyObj(&ho_NOregion);
   {
   HTuple end_val40 = hv_Number;
   HTuple step_val40 = 1;
   for (hv_Index2=1; hv_Index2.Continue(end_val40, step_val40); hv_Index2 += step_val40)
   {
     SelectObj(ho_medicine_regions, &ho_ObjectSelected, hv_Index2);
     AreaCenter(ho_ObjectSelected, &hv_Area, &hv_Row3, &hv_Column3);
     SmallestRectangle1(ho_ObjectSelected, &hv_Row11, &hv_Column11, &hv_Row2,
         &hv_Column2);
     GenRectangle1(&ho_Rectangle1, hv_Row11, hv_Column11, hv_Row2, hv_Column2);
     ReduceDomain(ho_ImageSub, ho_ObjectSelected, &ho_ImageReduced2);
     ReduceDomain(ho_ImageResult1, ho_ObjectSelected, &ho_ImageHun);
     Intensity(ho_ObjectSelected, ho_ImageReduced2, &hv_Mean, &hv_Deviation);
     //混料判断标准
     if (0 != (HTuple(int(hv_Mean<min)).TupleOr(int(hv_Mean>max))))
     {
       GenRectangle1(&ho_Rectangle1, hv_Row11, hv_Column11, hv_Row2, hv_Column2);
       ConcatObj(ho_Mixregion, ho_Rectangle1, &ho_Mixregion);
       AffineTransPoint2d(hv_HomMat2Dto3D, hv_Row3, hv_Column3, &hv_Qx, &hv_Qy);
       TupleConcat(hv_Mix1Row, hv_Qx, &hv_Mix1Row);
       TupleConcat(hv_Mix1Col, hv_Qy, &hv_Mix1Col);
     }
     else if (0 != (HTuple(int(hv_Mean>min)).TupleAnd(int(hv_Mean<max))))
     {

       FindShapeModel(ho_ImageReduced2, hv_ZENID, HTuple(0).TupleRad(), HTuple(360).TupleRad(),
           0.5, 1, 0.5, "least_squares", 0, 0.9, &hv_FRow, &hv_FColumn, &hv_Angle,
           &hv_Score);
       if (0 != (hv_FRow.TupleLength()))
       {
         TupleDeg(hv_Angle, &hv_Deg);

         if (0 != (int(hv_Index2<=(hv_Number/2))))
         {
           if (0 != (HTuple(HTuple(int(0<=(hv_Deg.TupleAbs()))).TupleAnd(int((hv_Deg.TupleAbs())<=160)))
                     .TupleOr(HTuple(int(200<=(hv_Deg.TupleAbs()))).TupleAnd(int((hv_Deg.TupleAbs())<=360)))))
           {
             AffineTransPoint2d(hv_HomMat2Dto3D, hv_FRow, hv_FColumn, &hv_Qx,&hv_Qy);
             TupleConcat(hv_NG1Row, hv_Qx, &hv_NG1Row);
             TupleConcat(hv_NG1Col, hv_Qy, &hv_NG1Col);
             TupleConcat(hv_NGAngle, hv_Deg, &hv_NGAngle);
             TupleConcat(hv_NGscore, hv_Score, &hv_NGscore);
             GenRectangle1(&ho_Rectangle1, hv_Row11, hv_Column11, hv_Row2, hv_Column2);
             GenCrossContourXld(&ho_Cross, hv_FRow, hv_FColumn, 50, hv_Angle);
             ConcatObj(ho_NGregion, ho_Rectangle1, &ho_NGregion);
             ConcatObj(ho_NGcross, ho_Cross, &ho_NGcross);

           }
         }
         else if (0 != (int(hv_Index2>(hv_Number/2))))
         {
             //前两排和后两排方向一致
//              if (0 != (HTuple(HTuple(int(0<=(hv_Deg.TupleAbs()))).TupleAnd(int((hv_Deg.TupleAbs())<=160)))
//                        .TupleOr(HTuple(int(200<=(hv_Deg.TupleAbs()))).TupleAnd(int((hv_Deg.TupleAbs())<=360)))))
             //前两排和后两排方向不一致
             if (0 != (HTuple(int(160<=(hv_Deg.TupleAbs()))).TupleAnd(int((hv_Deg.TupleAbs())<=340))))
             {
               AffineTransPoint2d(hv_HomMat2Dto3D, hv_FRow, hv_FColumn, &hv_Qx,&hv_Qy);
               TupleConcat(hv_NG1Row, hv_Qx, &hv_NG1Row);
               TupleConcat(hv_NG1Col, hv_Qy, &hv_NG1Col);
               TupleConcat(hv_NGAngle, hv_Deg, &hv_NGAngle);
               TupleConcat(hv_NGscore, hv_Score, &hv_NGscore);
               GenRectangle1(&ho_Rectangle1, hv_Row11, hv_Column11, hv_Row2, hv_Column2);
               GenCrossContourXld(&ho_Cross, hv_FRow, hv_FColumn, 50, hv_Angle);
               ConcatObj(ho_NGregion, ho_Rectangle1, &ho_NGregion);
               ConcatObj(ho_NGcross, ho_Cross, &ho_NGcross);
             }
         }
       }
       else if (0 != (int((hv_FRow.TupleLength())==0)))
       {
         ConcatObj(ho_NOregion, ho_Rectangle1, &ho_NOregion);

       }
     }

   }
   }
   CountObj(ho_Mixregion, &hv_Number1);
   CountObj(ho_NGregion, &hv_Number2);
   CountObj(ho_NOregion, &hv_Number3);

   re.ho_NGregion=ho_NGregion;
   re.ho_NGcross=ho_NGcross;
   re.ho_NOregion=ho_NOregion;
   re.ho_Mixregion=ho_Mixregion;

   re.hv_TupleMX=hv_Mix1Row;
   re.hv_TupleMY=hv_Mix1Col;
   re.hv_TupleNX=hv_NG1Row;
   re.hv_TupleNY=hv_NG1Col;


   return re;
}

VisionCheckResult1  AZJLocationArithmetic2::MBaction(HObject ho_Image,int min,int max)
{
   VisionCheckResult1 re;
     HObject   ho_medicine_regions, ho_ImageA;
     HObject  ho_ImageB, ho_ImageC, ho_ImageResult1, ho_ImageResult2;
     HObject  ho_ImageResult3, ho_GrayImage1, ho_ImageMedian1;
     HObject  ho_Circle, ho_RegionDifference, ho_ImageSurface;
     HObject  ho_ImageSub, ho_NGregion, ho_Mixregion, ho_NOregion;
     HObject  ho_ObjectSelected, ho_ImageReduced2, ho_ImageHun;
     HObject  ho_Rectangle1, ho_Cross, ho_Rectangle2,ho_NGcross;

     // Local control variables
     HTuple  hv_ImageFiles, hv_Index, hv_HomMat2Dto3D;
     HTuple  hv_ZENID, hv_Number, hv_Width, hv_Height, hv_Alpha;
     HTuple  hv_Beta, hv_Gamma, hv_Delta, hv_Epsilon, hv_Zeta;
     HTuple  hv_Area, hv_Row, hv_Column, hv_Mix1Row, hv_Mix1Col;
     HTuple  hv_NG1Row, hv_NG1Col, hv_NGAngle, hv_Index2, hv_Row3;
     HTuple  hv_Column3, hv_Row11, hv_Column11, hv_Row2, hv_Column2;
     HTuple  hv_Mean, hv_Deviation, hv_Qx, hv_Qy, hv_FRow, hv_FColumn;
     HTuple  hv_Angle, hv_Score, hv_Deg, hv_Number1, hv_Number2;
     HTuple  hv_Number3,hv_Mean1, hv_Deviation1;


         ReadTuple("MB2Dto3D.tup", &hv_HomMat2Dto3D);
         ReadShapeModel("Itemodel", &hv_ZENID);
         ReadObject(&ho_medicine_regions, "Serchmodel");
         Decompose3(ho_Image, &ho_ImageA, &ho_ImageB, &ho_ImageC);
         TransFromRgb(ho_ImageA, ho_ImageB, ho_ImageC, &ho_ImageResult1, &ho_ImageResult2,
             &ho_ImageResult3, "hsv");
         Rgb1ToGray(ho_Image, &ho_GrayImage1);
         MedianImage(ho_GrayImage1, &ho_ImageMedian1, "circle", 5, "mirrored");
         CountObj(ho_medicine_regions, &hv_Number);
         GetImageSize(ho_ImageMedian1, &hv_Width, &hv_Height);
         GenCircle(&ho_Circle, 495, 630, 350.5);
         Difference(ho_ImageMedian1, ho_Circle, &ho_RegionDifference);
         FitSurfaceSecondOrder(ho_RegionDifference, ho_ImageMedian1, "regression", 5,
             2, &hv_Alpha, &hv_Beta, &hv_Gamma, &hv_Delta, &hv_Epsilon, &hv_Zeta);
         AreaCenter(ho_RegionDifference, &hv_Area, &hv_Row, &hv_Column);
         GenImageSurfaceSecondOrder(&ho_ImageSurface, "byte", hv_Alpha, hv_Beta, hv_Gamma,
             hv_Delta, hv_Epsilon, hv_Zeta, hv_Row, hv_Column, hv_Width, hv_Height);
         SubImage(ho_ImageMedian1, ho_ImageSurface, &ho_ImageSub, 1, 128);
        Intensity(ho_ImageResult1, ho_ImageResult1, &hv_Mean1, &hv_Deviation1);
         hv_Mix1Row = HTuple();
         hv_Mix1Col = HTuple();
         hv_NG1Row = HTuple();
         hv_NG1Col = HTuple();
         hv_NGAngle = HTuple();

         //反料
         GenEmptyObj(&ho_NGregion);
         GenEmptyObj(&ho_NGcross);
         //混料
         GenEmptyObj(&ho_Mixregion);
         //空料
         GenEmptyObj(&ho_NOregion);
         for (hv_Index2=1; hv_Index2<=hv_Number; hv_Index2+=1)
         {
           SelectObj(ho_medicine_regions, &ho_ObjectSelected, hv_Index2);
           AreaCenter(ho_ObjectSelected, &hv_Area, &hv_Row3, &hv_Column3);
           SmallestRectangle1(ho_ObjectSelected, &hv_Row11, &hv_Column11, &hv_Row2, &hv_Column2);
           ReduceDomain(ho_ImageSub, ho_ObjectSelected, &ho_ImageReduced2);
           ReduceDomain(ho_ImageResult1, ho_ObjectSelected, &ho_ImageHun);
           Intensity(ho_ObjectSelected, ho_ImageHun, &hv_Mean, &hv_Deviation);
           if (0 != (int(hv_Mean<min)))
           {
             GenRectangle1(&ho_Rectangle1, hv_Row11, hv_Column11, hv_Row2, hv_Column2);
             ConcatObj(ho_Mixregion, ho_Rectangle1, &ho_Mixregion);
             AffineTransPoint2d(hv_HomMat2Dto3D, hv_Row3, hv_Column3, &hv_Qx, &hv_Qy);
             TupleConcat(hv_Mix1Row, hv_Qx, &hv_Mix1Row);
             TupleConcat(hv_Mix1Col, hv_Qy, &hv_Mix1Col);
           }

           if (0 != (HTuple(int((hv_Index2%2)==1)).TupleAnd(int(hv_Mean>min))))
           {
             FindShapeModel(ho_ImageReduced2, hv_ZENID, (HTuple(190).TupleRad()), HTuple(340).TupleRad(),
                 0.5, 1, 0.5, "least_squares", 0, 0.9, &hv_FRow, &hv_FColumn, &hv_Angle,
                 &hv_Score);
             if (0 != (hv_FRow.TupleLength()))
             {
               TupleDeg(hv_Angle, &hv_Deg);
               AffineTransPoint2d(hv_HomMat2Dto3D, hv_Row3, hv_Column3, &hv_Qx, &hv_Qy);
               TupleConcat(hv_NG1Row, hv_Qx, &hv_NG1Row);
               TupleConcat(hv_NG1Col, hv_Qy, &hv_NG1Col);
               TupleConcat(hv_NGAngle, hv_Deg, &hv_NGAngle);
               GenRectangle1(&ho_Rectangle1, hv_Row11, hv_Column11, hv_Row2, hv_Column2);
               GenCrossContourXld(&ho_Cross, hv_FRow, hv_FColumn, 100, hv_Angle);
               ConcatObj(ho_NGregion, ho_Rectangle1, &ho_NGregion);
               ConcatObj(ho_NGcross, ho_Cross, &ho_NGcross);
             }

           }
           else if (0 != (HTuple(int((hv_Index2%2)==0)).TupleAnd(int(hv_Mean>min))))
           {
             FindShapeModel(ho_ImageReduced2, hv_ZENID, (HTuple(10).TupleRad()), HTuple(340).TupleRad(),
                 0.5, 1, 0.5, "least_squares", 0, 0.9, &hv_FRow, &hv_FColumn, &hv_Angle,
                 &hv_Score);
             if (0 != (hv_FRow.TupleLength()))
             {
               TupleDeg(hv_Angle, &hv_Deg);
               AffineTransPoint2d(hv_HomMat2Dto3D, hv_Row3, hv_Column3, &hv_Qx, &hv_Qy);
               TupleConcat(hv_NG1Row, hv_Qx, &hv_NG1Row);
               TupleConcat(hv_NG1Col, hv_Qy, &hv_NG1Col);
               TupleConcat(hv_NGAngle, hv_Deg, &hv_NGAngle);
               GenRectangle1(&ho_Rectangle2, hv_Row11, hv_Column11, hv_Row2, hv_Column2);
               GenCrossContourXld(&ho_Cross, hv_FRow, hv_FColumn, 100, hv_Angle);
               ConcatObj(ho_NGregion, ho_Rectangle2, &ho_NGregion);
                ConcatObj(ho_NGcross, ho_Cross, &ho_NGcross);
             }
           }
         }
         CountObj(ho_Mixregion, &hv_Number1);
         CountObj(ho_NGregion, &hv_Number2);
         CountObj(ho_NOregion, &hv_Number3);
        re.ho_NGregion=ho_NGregion;
        re.ho_NGcross=ho_NGcross;
        re.ho_NOregion=ho_NOregion;
        re.ho_Mixregion=ho_Mixregion;

           //角度hv_NGAngle

        re.hv_gray=hv_Mean1;
        re.hv_TupleMX=hv_Mix1Row;
        re.hv_TupleMY=hv_Mix1Col;

        re.hv_TupleNX=hv_NG1Row;
        re.hv_TupleNY=hv_NG1Col;
        re.hv_NGAngle=hv_NGAngle;
        return re;

}

VisionCheckResult1 AZJLocationArithmetic2::cyAZJaction(HObject ho_Image)
{
   VisionCheckResult1 re;
   HObject  ho_medicine_regions, ho_ImageA;
   HObject  ho_ImageB, ho_ImageC, ho_ImageResult1, ho_ImageResult2;
   HObject  ho_ImageResult3, ho_GrayImage1, ho_ImageMedian1;
   HObject  ho_Circle, ho_RegionDifference, ho_ImageSurface;
   HObject  ho_ImageSub, ho_NGregion, ho_Mixregion, ho_NOregion;
   HObject  ho_ObjectSelected, ho_Rectangle1, ho_ImageReduced2;
   HObject  ho_ImageHun, ho_Cross;

   // Local control variables
   HTuple  hv_ImageFiles, hv_Index, hv_WindowHandle1;
   HTuple  hv_HomMat2Dto3D, hv_ZENID, hv_Number, hv_Width;
   HTuple  hv_Height, hv_Alpha, hv_Beta, hv_Gamma, hv_Delta;
   HTuple  hv_Epsilon, hv_Zeta, hv_Area, hv_Row, hv_Column;
   HTuple  hv_Mix1Row, hv_Mix1Col, hv_NG1Row, hv_NG1Col, hv_NGAngle;
   HTuple  hv_NGscore, hv_Index2, hv_Row3, hv_Column3, hv_Row11;
   HTuple  hv_Column11, hv_Row2, hv_Column2, hv_Mean, hv_Deviation;
   HTuple  hv_Qx, hv_Qy, hv_FRow, hv_FColumn, hv_Angle, hv_Score;
   HTuple  hv_Deg, hv_Number1, hv_Number2, hv_Number3;



   ReadTuple("MB2Dto3D.tup", &hv_HomMat2Dto3D);
   ReadShapeModel("Itemodel", &hv_ZENID);
   ReadObject(&ho_medicine_regions, "Serchmodel");
   Decompose3(ho_Image, &ho_ImageA, &ho_ImageB, &ho_ImageC);
   TransFromRgb(ho_ImageA, ho_ImageB, ho_ImageC, &ho_ImageResult1, &ho_ImageResult2,
       &ho_ImageResult3, "hsv");
   Rgb1ToGray(ho_Image, &ho_GrayImage1);
   MedianImage(ho_GrayImage1, &ho_ImageMedian1, "circle", 5, "mirrored");
   CountObj(ho_medicine_regions, &hv_Number);


   GetImageSize(ho_ImageMedian1, &hv_Width, &hv_Height);
   GenCircle(&ho_Circle, 495, 630, 350.5);
   Difference(ho_ImageMedian1, ho_Circle, &ho_RegionDifference);
   FitSurfaceSecondOrder(ho_RegionDifference, ho_ImageMedian1, "regression", 5,
       2, &hv_Alpha, &hv_Beta, &hv_Gamma, &hv_Delta, &hv_Epsilon, &hv_Zeta);
   AreaCenter(ho_RegionDifference, &hv_Area, &hv_Row, &hv_Column);
   GenImageSurfaceSecondOrder(&ho_ImageSurface, "byte", hv_Alpha, hv_Beta, hv_Gamma,
       hv_Delta, hv_Epsilon, hv_Zeta, hv_Row, hv_Column, hv_Width, hv_Height);
   SubImage(ho_ImageMedian1, ho_ImageSurface, &ho_ImageSub, 1, 128);

   hv_Mix1Row = HTuple();
   hv_Mix1Col = HTuple();
   hv_NG1Row = HTuple();
   hv_NG1Col = HTuple();
   hv_NGAngle = HTuple();
   hv_NGscore = HTuple();
   //反料
   GenEmptyObj(&ho_NGregion);
   //混料
   GenEmptyObj(&ho_Mixregion);
   //空料
   GenEmptyObj(&ho_NOregion);
   {
   HTuple end_val40 = hv_Number;
   HTuple step_val40 = 1;
   for (hv_Index2=1; hv_Index2.Continue(end_val40, step_val40); hv_Index2 += step_val40)
   {
     SelectObj(ho_medicine_regions, &ho_ObjectSelected, hv_Index2);
     AreaCenter(ho_ObjectSelected, &hv_Area, &hv_Row3, &hv_Column3);
     SmallestRectangle1(ho_ObjectSelected, &hv_Row11, &hv_Column11, &hv_Row2,
         &hv_Column2);
     GenRectangle1(&ho_Rectangle1, hv_Row11, hv_Column11, hv_Row2, hv_Column2);
     ReduceDomain(ho_ImageSub, ho_ObjectSelected, &ho_ImageReduced2);
     ReduceDomain(ho_ImageResult1, ho_ObjectSelected, &ho_ImageHun);
     Intensity(ho_ObjectSelected, ho_ImageReduced2, &hv_Mean, &hv_Deviation);

     FindShapeModel(ho_ImageReduced2, hv_ZENID, -HTuple(10).TupleRad(), HTuple(720).TupleRad(),
           0.5, 1, 0.5, "least_squares", 0, 0.9, &hv_FRow, &hv_FColumn, &hv_Angle,
           &hv_Score);
       if (0 != (hv_FRow.TupleLength()))
       {
         TupleDeg(hv_Angle, &hv_Deg);
        AffineTransPoint2d(hv_HomMat2Dto3D, hv_FRow, hv_FColumn, &hv_Qx,
                 &hv_Qy);
        TupleConcat(hv_NG1Row, hv_Qx, &hv_NG1Row);
        TupleConcat(hv_NG1Col, hv_Qy, &hv_NG1Col);
        TupleConcat(hv_NGAngle, hv_Deg, &hv_NGAngle);
        TupleConcat(hv_NGscore, hv_Score, &hv_NGscore);
        GenRectangle1(&ho_Rectangle1, hv_Row11, hv_Column11, hv_Row2, hv_Column2);
        GenCrossContourXld(&ho_Cross, hv_FRow, hv_FColumn, 50, hv_Angle);
        ConcatObj(ho_NGregion, ho_Rectangle1, &ho_NGregion);


       }
       else if (0 != (int((hv_FRow.TupleLength())==0)))
       {
         ConcatObj(ho_NOregion, ho_Rectangle1, &ho_NOregion);

       }
     }


   }

   CountObj(ho_NGregion, &hv_Number2);
   CountObj(ho_NOregion, &hv_Number3);

   re.ho_NGregion=ho_NGregion;
   re.ho_NOregion=ho_NOregion;
   re.ho_Mixregion=ho_Mixregion;

   re.hv_TupleMX=hv_Mix1Row;
   re.hv_TupleMY=hv_Mix1Col;
   re.hv_TupleNX=hv_NG1Row;
   re.hv_TupleNY=hv_NG1Col;

   re.hv_NGAngle=hv_NGAngle;
   return re;
}


LocationArithmetic3::LocationArithmetic3()
{

}

VisionCheckResult LocationArithmetic3::action(HObject ho_Image)
{
   //声明处理结果
   VisionCheckResult re;


   return re;
}

LocationArithmetic4::LocationArithmetic4()
{

}

VisionCheckResult1 AZJLocationArithmetic2::MBCYaction(HObject ho_Image)
{
     VisionCheckResult1 re;

     HObject  ho_medicine_regions, ho_ImageA,ho_NGregion;
     HObject  ho_ImageB, ho_ImageC, ho_ImageResult1, ho_ImageResult2;
     HObject  ho_ImageResult3, ho_GrayImage1, ho_ImageMedian1;
     HObject  ho_Circle, ho_RegionDifference, ho_ImageSurface;
     HObject  ho_ImageSub, ho_Cross;

     // Local control variables
     HTuple  hv_ImageFiles, hv_Index, hv_HomMat2Dto3D;
     HTuple  hv_ZENID, hv_Number, hv_Width, hv_Height, hv_Alpha;
     HTuple  hv_Beta, hv_Gamma, hv_Delta, hv_Epsilon, hv_Zeta;
     HTuple  hv_Area, hv_Row, hv_Column, hv_Mix1Row, hv_Mix1Col;
     HTuple  hv_NG1Row, hv_NG1Col, hv_NGAngle, hv_FRow, hv_FColumn;
     HTuple  hv_Angle, hv_Score;

     ReadTuple("MB2Dto3D.tup", &hv_HomMat2Dto3D);
     ReadShapeModel("Itemodel", &hv_ZENID);
     ReadObject(&ho_medicine_regions, "Serchmodel");
     Decompose3(ho_Image, &ho_ImageA, &ho_ImageB, &ho_ImageC);
     TransFromRgb(ho_ImageA, ho_ImageB, ho_ImageC, &ho_ImageResult1, &ho_ImageResult2,
         &ho_ImageResult3, "hsv");
     Rgb1ToGray(ho_Image, &ho_GrayImage1);
     MedianImage(ho_GrayImage1, &ho_ImageMedian1, "circle", 5, "mirrored");
     CountObj(ho_medicine_regions, &hv_Number);

     GetImageSize(ho_ImageMedian1, &hv_Width, &hv_Height);
     GenCircle(&ho_Circle, 495, 630, 350.5);
     Difference(ho_ImageMedian1, ho_Circle, &ho_RegionDifference);
     FitSurfaceSecondOrder(ho_RegionDifference, ho_ImageMedian1, "regression", 5,
         2, &hv_Alpha, &hv_Beta, &hv_Gamma, &hv_Delta, &hv_Epsilon, &hv_Zeta);
     AreaCenter(ho_RegionDifference, &hv_Area, &hv_Row, &hv_Column);
     GenImageSurfaceSecondOrder(&ho_ImageSurface, "byte", hv_Alpha, hv_Beta, hv_Gamma,
         hv_Delta, hv_Epsilon, hv_Zeta, hv_Row, hv_Column, hv_Width, hv_Height);
     SubImage(ho_ImageMedian1, ho_ImageSurface, &ho_ImageSub, 1, 128);

     hv_NG1Row = HTuple();
     hv_NG1Col = HTuple();
     hv_NGAngle = HTuple();
     FindShapeModel(ho_ImageSub, hv_ZENID, HTuple(0).TupleRad(), HTuple(360).TupleRad(),
         0.5, 24, 0.5, "least_squares", 0, 0.9, &hv_FRow, &hv_FColumn, &hv_Angle,
         &hv_Score);

     GenCrossContourXld(&ho_Cross, hv_FRow, hv_FColumn, 50, hv_Angle);

           //角度hv_NGAngle
        re.hv_NGAngle=hv_NGAngle;

        re.hv_TupleNX=hv_NG1Row;
        re.hv_TupleNY=hv_NG1Col;

        re.ho_NGregion=ho_Cross;

        return re;
}
VisionCheckResult1 LocationArithmetic4::action(HObject ho_Image)
{
    VisionCheckResult1 re;


    return re;


}

void LocationArithmetic4::setModel(HTuple modelID)
{
   hv_ModelID111=modelID;
}

VisionCheckResult3 LocationArithmetic5::model(HObject ho_Image,HTuple hv_win)
{
   VisionCheckResult3 re;

   return re;

}



LocationArithmetic5::LocationArithmetic5()
{

}

VisionCheckResult3 LocationArithmetic5::action(HObject ho_Image)
{
   VisionCheckResult3 re;

     //360 890 1420 1950
   return  re;

}
