#ifndef VISIONALGORITHM_H
#define VISIONALGORITHM_H
#include <QImage>
#include <QObject>

class VisionAlgorithm : public QObject
{
    Q_OBJECT
public:
    VisionAlgorithm();
    ~VisionAlgorithm(){

    }
public slots:
    void processImage(unsigned char* pRgbFrameBuf, int nWidth, int nHeight, uint64_t nPixelFormat);
signals:
    void imageProcessed(int result);
};

#endif // VISIONALGORITHM_H
