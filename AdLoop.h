#ifndef ADLOOP_H
#define ADLOOP_H

#include <QObject>
#include <QTimer>
#include <QPoint>
#include <QImage>
#include <QProcess>
#include <QString>
#include <opencv2/opencv.hpp>

class AdLoop : public QObject {
    Q_OBJECT
public:
    explicit AdLoop(QObject *parent = nullptr);
    ~AdLoop();

public slots:
    void start();
    void stop();
    void loop();
    void updateCoordinates(QPoint x1, QPoint x1_l, QPoint x2, QPoint adWatch,
                           QPoint capTxt, QPoint capConfirm, QPoint contAd,
                           QString port, QString adbExecutablePath);

signals:
    void logMessage(QString message);
    void screenUpdated(QString imagePath);

private:
    int checkState();
    void captureScreen();
    void tap(int x, int y);
    QString solveCaptcha(QString imagePath);
    QPoint findTemplate(QImage sourceTemplate, QString templateName);
    void setupAdbEnvironment(QProcess &process);

    QTimer *timer = nullptr;
    QString adbPortAddress = "127.0.0.1:5555";
    QString adbPath;
    QImage screenImg;
    int state = 0;

    // Coordinates
    QPoint XButton1 = QPoint(857, 49);
    QPoint XButton1_left = QPoint(53, 49);
    QPoint XButton2 = QPoint(856, 388);
    QPoint adWatchButton = QPoint(211, 623);
    QPoint captchaTextbox = QPoint(328, 886);
    QPoint captchaConfirm = QPoint(598, 1000);
    QPoint continueAdButton = QPoint(618, 902);
    QPoint captchaCorner1 = QPoint(300, 680);
    QPoint captchaCorner2 = QPoint(595, 827);
};

#endif // ADLOOP_H
