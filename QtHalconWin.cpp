#include "QtHalconWin.h"

QtHalconWin::QtHalconWin()
{

}

QtHalconWin::QtHalconWin(QWidget *widget)
{
    openwindow(widget);
    InitDisplay();
}

void QtHalconWin::InitDisplay()
{
    this->setDraw("margin");
    this->setLineWidth(5);
    this->setColor("green");
    this->SetDisplayFont();
}

HTuple QtHalconWin::QString2Htuple(QString string)
{
    QByteArray ba = string.toLocal8Bit();
    HalconCpp::HTuple hv_string(ba.data());

    //或者
    //std::string str = string.toStdString();
    //HTuple hv_string;
    //hv_string = str.c_str();

    return hv_string;
}

void QtHalconWin::openwindow(QWidget *widget)
{
    WId windowID = widget->winId();
    OpenWindow(0, 0,\
               widget->width(), widget->height(),\
               (Hlong)windowID,\
               "visible", "",\
               &hv_WindowHandle);

}

HObject QtHalconWin::readimage(const QString imageFile)
{
    HObject image;
    HTuple imagepath = QString2Htuple(imageFile);
    ReadImage(&image, imagepath);
    devDisplay(image);
    return image;
}

void QtHalconWin::dispMessage(HTuple hv_String, HTuple hv_Color, HTuple hv_Row, HTuple hv_Column, HTuple hv_Box, HTuple hv_CoordSystem)
{
    // Local control variables
    HTuple  hv_Red, hv_Green, hv_Blue, hv_Row1Part;
    HTuple  hv_Column1Part, hv_Row2Part, hv_Column2Part, hv_RowWin;
    HTuple  hv_ColumnWin, hv_WidthWin, hv_HeightWin, hv_MaxAscent;
    HTuple  hv_MaxDescent, hv_MaxWidth, hv_MaxHeight, hv_R1;
    HTuple  hv_C1, hv_FactorRow, hv_FactorColumn, hv_UseShadow;
    HTuple  hv_ShadowColor, hv_Exception, hv_Width, hv_Index;
    HTuple  hv_Ascent, hv_Descent, hv_W, hv_H, hv_FrameHeight;
    HTuple  hv_FrameWidth, hv_R2, hv_C2, hv_DrawMode, hv_CurrentColor;

    GetRgb(hv_WindowHandle, &hv_Red, &hv_Green, &hv_Blue);
    GetPart(hv_WindowHandle, &hv_Row1Part, &hv_Column1Part, &hv_Row2Part, &hv_Column2Part);
    GetWindowExtents(hv_WindowHandle, &hv_RowWin, &hv_ColumnWin, &hv_WidthWin, &hv_HeightWin);
    SetPart(hv_WindowHandle, 0, 0, hv_HeightWin - 1, hv_WidthWin - 1);

    if (0 != (hv_Row == -1))
    {
        hv_Row = 12;
    }
    if (0 != (hv_Column == -1))
    {
        hv_Column = 12;
    }
    if (0 != (hv_Color == HTuple()))
    {
        hv_Color = "";
    }
    hv_String = (("" + hv_String) + "").TupleSplit("\n");

    GetFontExtents(hv_WindowHandle, &hv_MaxAscent, &hv_MaxDescent, &hv_MaxWidth, &hv_MaxHeight);
    if (0 != (hv_CoordSystem == HTuple("window")))
    {
        hv_R1 = hv_Row;
        hv_C1 = hv_Column;
    }
    else
    {
        hv_FactorRow = (1. * hv_HeightWin) / ((hv_Row2Part - hv_Row1Part) + 1);
        hv_FactorColumn = (1. * hv_WidthWin) / ((hv_Column2Part - hv_Column1Part) + 1);
        hv_R1 = ((hv_Row - hv_Row1Part) + 0.5) * hv_FactorRow;
        hv_C1 = ((hv_Column - hv_Column1Part) + 0.5) * hv_FactorColumn;
    }
    hv_UseShadow = 1;
    hv_ShadowColor = "gray";
    if (0 != (HTuple(hv_Box[0]) == HTuple("true")))
    {
        hv_Box[0] = "#fce9d4";
        hv_ShadowColor = "#f28d26";
    }
    if (0 != ((hv_Box.TupleLength()) > 1))
    {
        if (0 != (HTuple(hv_Box[1]) == HTuple("true")))
        {
            //Use default ShadowColor set above
        }
        else if (0 != (HTuple(hv_Box[1]) == HTuple("false")))
        {
            hv_UseShadow = 0;
        }
        else
        {
            hv_ShadowColor = ((const HTuple&)hv_Box)[1];
            //Valid color?
            try
            {
                SetColor(hv_WindowHandle, HTuple(hv_Box[1]));
            }
            // catch (Exception)
            catch (HalconCpp::HException& HDevExpDefaultException)
            {
                HDevExpDefaultException.ToHTuple(&hv_Exception);
                hv_Exception = "Wrong value of control parameter Box[1] (must be a 'true', 'false', or a valid color string)";
                throw HalconCpp::HException(hv_Exception);
            }
        }
    }
    if (0 != (HTuple(hv_Box[0]) != HTuple("false")))
    {
        //Valid color?
        try
        {
            SetColor(hv_WindowHandle, HTuple(hv_Box[0]));
        }
        // catch (Exception)
        catch (HalconCpp::HException& HDevExpDefaultException)
        {
            HDevExpDefaultException.ToHTuple(&hv_Exception);
            hv_Exception = "Wrong value of control parameter Box[0] (must be a 'true', 'false', or a valid color string)";
            throw HalconCpp::HException(hv_Exception);
        }
        //Calculate box extents
        hv_String = (" " + hv_String) + " ";
        hv_Width = HTuple();
        {
            HTuple end_val93 = (hv_String.TupleLength()) - 1;
            HTuple step_val93 = 1;
            for (hv_Index = 0; hv_Index.Continue(end_val93, step_val93); hv_Index += step_val93)
            {
                GetStringExtents(hv_WindowHandle, HTuple(hv_String[hv_Index]), &hv_Ascent,
                                 &hv_Descent, &hv_W, &hv_H);
                hv_Width = hv_Width.TupleConcat(hv_W);
            }
        }
        hv_FrameHeight = hv_MaxHeight * (hv_String.TupleLength());
        hv_FrameWidth = (HTuple(0).TupleConcat(hv_Width)).TupleMax();
        hv_R2 = hv_R1 + hv_FrameHeight;
        hv_C2 = hv_C1 + hv_FrameWidth;
        //Display rectangles
        GetDraw(hv_WindowHandle, &hv_DrawMode);
        SetDraw(hv_WindowHandle, "fill");
        //Set shadow color
        SetColor(hv_WindowHandle, hv_ShadowColor);
        if (0 != hv_UseShadow)
        {
            DispRectangle1(hv_WindowHandle, hv_R1 + 1, hv_C1 + 1, hv_R2 + 1, hv_C2 + 1);
        }
        //Set box color
        SetColor(hv_WindowHandle, HTuple(hv_Box[0]));
        DispRectangle1(hv_WindowHandle, hv_R1, hv_C1, hv_R2, hv_C2);
        SetDraw(hv_WindowHandle, hv_DrawMode);
    }
    //Write text.
    {
        HTuple end_val115 = (hv_String.TupleLength()) - 1;
        HTuple step_val115 = 1;
        for (hv_Index = 0; hv_Index.Continue(end_val115, step_val115); hv_Index += step_val115)
        {
            hv_CurrentColor = ((const HTuple&)hv_Color)[hv_Index % (hv_Color.TupleLength())];
            if (0 != (HTuple(hv_CurrentColor != HTuple("")).TupleAnd(hv_CurrentColor != HTuple("auto"))))
            {
                SetColor(hv_WindowHandle, hv_CurrentColor);
            }
            else
            {
                SetRgb(hv_WindowHandle, hv_Red, hv_Green, hv_Blue);
            }
            hv_Row = hv_R1 + (hv_MaxHeight * hv_Index);
            SetTposition(hv_WindowHandle, hv_Row, hv_C1);
            WriteString(hv_WindowHandle, HTuple(hv_String[hv_Index]));
        }
    }
    //Reset changed window settings
    SetRgb(hv_WindowHandle, hv_Red, hv_Green, hv_Blue);
    SetPart(hv_WindowHandle, hv_Row1Part, hv_Column1Part, hv_Row2Part, hv_Column2Part);
    return;
}

void QtHalconWin::devDisplay(HObject &img)
{
   HTuple hv_cls;
   //得到img对象的类
   GetObjClass(img, &hv_cls);
   QString qstr = "image";

   //HTuple.S()方法用于将HTuple对象转换为字符串。它返回一个HString字符串。
   if (hv_cls.S() == qstr)
   {
       //如果传入img是image,就将m_objs容器清空,重新显示原图
       m_objs.clear();
   }
   display(img);
   m_objs.push_back(img);
}

void QtHalconWin::devDisplay(vector<HObject> &objs)
{
    int len = (int)objs.size();
    for (int i = 0; i < len; i++)
    {
        HTuple hv_cls;
        GetObjClass(objs.at(i), &hv_cls);
        QString qstr = "image";
        //如果obj是image类型，清空m_objs
        if (hv_cls.S() == qstr)
        {
            m_objs.clear();
        }
        display(objs.at(i));
        m_objs.push_back(objs.at(i));
    }
}

void QtHalconWin::dispCross(HTuple row, HTuple col, HTuple size, HTuple angle)
{
    HalconCpp::DispCross(hv_WindowHandle, row, col, size, angle);
}

void QtHalconWin::clearWindow()
{
    HalconCpp::ClearWindow(hv_WindowHandle);
}

void QtHalconWin::setLineWidth(HTuple hv_Width)
{
    HalconCpp::SetLineWidth(hv_WindowHandle,hv_Width);
}

void QtHalconWin::setColor(QString color)
{
    HTuple hv_color = QString2Htuple(color);
    HalconCpp::SetColor(hv_WindowHandle, hv_color);
}

void QtHalconWin::setColored(int colorNum)
{
    HalconCpp::SetColored(hv_WindowHandle,colorNum);
}

void QtHalconWin::setDraw(QString drawType)
{
    HTuple hv_drawType = QString2Htuple(drawType);
    HalconCpp::SetDraw(hv_WindowHandle, hv_drawType);
}

void QtHalconWin::SetDisplayFont(HTuple size, HTuple fontName, bool bold, bool slant)
{
    HTuple  hv_Font;
    HTuple _slant = slant ? "1" : "*";
    HTuple _bold = bold ? "1" : "*";
    hv_Font = "-" + fontName + "-" + size + "-*-" + _slant + "-*-*-" + _bold + "-";
    HalconCpp::SetFont(hv_WindowHandle, hv_Font);
}

HTuple QtHalconWin::getWindow()
{
    return hv_WindowHandle;
}

void QtHalconWin::display(HObject &obj)
{
    HTuple hv_cls;
    GetObjClass(obj, &hv_cls);
    QString cstr = "image";
    if (hv_cls.S() == cstr)
    {
        //qDebug()<<"=image";
        HTuple width, height;
        GetImageSize(obj, &width, &height);
        SetPart(hv_WindowHandle, 0, 0, height - 1, width - 1);
        HalconCpp::DispObj(obj, hv_WindowHandle);
    }
    else
    {
        //qDebug()<<"!=image";
        HalconCpp::DispObj(obj, hv_WindowHandle);
    }
}


