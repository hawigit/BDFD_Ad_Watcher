#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QProcess>
#include <QPoint>
#include <QColor>
#include <QImage>
#include <QDir>
#include <QDateTime>
#include <QRect>
#include <QThread>
#include <QString>
#include <QDebug>
#include <windows.h>
#include <opencv2/opencv.hpp>
#include "AdLoop.h"

class backend : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString imgPath READ getImgPath NOTIFY imgPathChanged)
    Q_PROPERTY(QString adbPort READ getAdbPort WRITE updatePort NOTIFY adbPortChanged)
    Q_PROPERTY(bool isReady READ isReady NOTIFY isReadyChanged)
public:
    explicit backend(QObject *parent = nullptr);
    ~backend();
    Q_INVOKABLE void connectBlueStacks();
    Q_INVOKABLE void captureScreen();
    Q_INVOKABLE void captureWidget(QString widgetName);

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void log(QString text);
    Q_INVOKABLE QString getImgPath() {
        return imgPath;
    }
    QString getAdbPort() {
        return adbPortAddress;
    }
    bool isReady() const {
        return m_isReady;
    }
    bool isColorSimilar(QColor firstColor, QColor secondColor, int acceptedRange);

    Q_INVOKABLE QPoint getPos(int x, int y, double width, double height, int mode = 0, int clickCount = 0, QString widgetName = "");
    Q_INVOKABLE void updatePort(QString port);
    Q_INVOKABLE void setLocXButton1(QPoint point);
    Q_INVOKABLE void setLocXButton1_left(QPoint point);
    Q_INVOKABLE void setLocXButton2(QPoint point);
    Q_INVOKABLE void setAdWatchButton(QPoint point);
    Q_INVOKABLE void setCaptchaTextbox(QPoint point);
    Q_INVOKABLE void setCaptchaConfirm(QPoint point);
    Q_INVOKABLE void setContinueAd(QPoint point);

public slots:
    void onScreenUpdated(QString path);

signals:
    void logUpdated(QString message);
    void imgPathChanged();
    void adbPortChanged();
    void isReadyChanged();

    void requestStart();
    void requestStop();
    void settingsUpdated(QPoint x1, QPoint x1_l, QPoint x2, QPoint adWatch,
                         QPoint capTxt, QPoint capConfirm, QPoint contAd,
                         QString port, QString adbExecutablePath);

private:
    void initializeTemplates();
    void saveSettings();
    void loadSettings();
    void setupAdbEnvironment(QProcess &process);
    void sendSettingsToWorker();

    QThread *loopThread = nullptr;
    AdLoop *adLoop = nullptr;

    QProcess *ocrProcess = nullptr;
    bool m_isReady = false;
    QString adbPortAddress = "127.0.0.1:5555";
    QString adbPath;
    QString imgPath;
    QImage screenImg;
    int adbConnected = 0;

    QPoint XButton1 = QPoint(857, 49);
    QPoint XButton1_left = QPoint(53, 49);
    QPoint XButton2 = QPoint(856, 388);
    QPoint adWatchButton = QPoint(211, 623);
    QPoint captchaTextbox = QPoint(328, 886);
    QPoint captchaConfirm = QPoint(598, 1000);
    QPoint continueAdButton = QPoint(618, 902);
    QPoint captureWidget1 = QPoint(0, 0);
    QPoint captureWidget2 = QPoint(0, 0);
    QString widgetName;
};

#endif // BACKEND_H
