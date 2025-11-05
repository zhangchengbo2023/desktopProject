#include "UI/mainwindow.h"
#include "QsLog/QsLog.h"
#include <QApplication>
#include <QDir>
#include "controller.h"
#include <QTextCodec>
using namespace QsLogging;

void initFileLogger(QString filePath)
{
    Logger &logger = Logger::instance();
    logger.setLoggingLevel(QsLogging::DebugLevel);
    const QString sLogPath(filePath);

    DestinationPtr fileDestination(DestinationFactory::MakeFileDestination(
                                       sLogPath, EnableLogRotation, MaxSizeBytes(1024 * 1024 * 10), MaxOldLogCount(2)));

    logger.addDestination(fileDestination);
}

int main(int argc, char *argv[])
{
;
    QApplication a(argc, argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    //日志
    QDir dir(QCoreApplication::applicationDirPath() + "/logs");
    if (!dir.exists())
        dir.mkpath(QCoreApplication::applicationDirPath() + "/logs");
    initFileLogger(QDir(a.applicationDirPath()).filePath("logs/log.txt"));
    MainWindow w;
    w.show();
    qDebug() << "MainWindow UI constructed...";

    return a.exec();
}
