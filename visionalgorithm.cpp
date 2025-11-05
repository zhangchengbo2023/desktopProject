#include "visionalgorithm.h"
//using namespace HalconCpp;
VisionAlgorithm::VisionAlgorithm()
{

}

void VisionAlgorithm::processImage(unsigned char* pRgbFrameBuf, int nWidth, int nHeight, uint64_t nPixelFormat)
{


//    using namespace HalconCpp;
//    HObject ho_Image, ho_GrayImage,ho_Rectangle;
//    HObject ho_ImageReduced,ho_Cross1;
//    HTuple hv_ImageFiles,hv_Width,hv_Height,hv_WindowHandle;
//    HTuple hv_Row1,hv_Column1,hv_Row2,hv_Column2,hv_ModelID;
//    HTuple hv_Row,hv_Column,hv_Angle,hv_Score;

//    GenImage1(&ho_Image,"byte",nWidth,nHeight,reinterpret_cast<Hlong>(pRgbFrameBuf));
//    GetImageSize(ho_Image,&hv_Width,&hv_Height);

//    OpenWindow(0,0,hv_Width/3,hv_Height/3,0,"visible","",&hv_WindowHandle);
//    WId windowId = ui->label_Pixmap_2->winId();//ui handle
//    // ui handle -> HTuple
//    hv_WindowHandle.setLong(HTuple(reinterpret_cast<ht_long>(windowId)));

//    HDevWindowStack::Push(hv_WindowHandle);
//    SetWindowAttr(hv_WindowHandle,"background_color","black");
}
