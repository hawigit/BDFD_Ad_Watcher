#include "AdLoop.h"
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QThread>
#include <QProcessEnvironment>
#include <QDebug>

AdLoop::AdLoop(QObject *parent)
    : QObject{parent}
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &AdLoop::loop);
    adbPath = QDir::currentPath() + "/adb.exe";
}

AdLoop::~AdLoop() {
    if (timer && timer->isActive()) {
        timer->stop();
    }
}

void AdLoop::updateCoordinates(QPoint x1, QPoint x1_l, QPoint x2, QPoint adWatch,
                               QPoint capTxt, QPoint capConfirm, QPoint contAd,
                               QString port, QString adbExecutablePath) {
    XButton1 = x1;
    XButton1_left = x1_l;
    XButton2 = x2;
    adWatchButton = adWatch;
    captchaTextbox = capTxt;
    captchaConfirm = capConfirm;
    continueAdButton = contAd;
    adbPortAddress = port;
    if (!adbExecutablePath.isEmpty()) {
        adbPath = adbExecutablePath;
    }
}

void AdLoop::start() {
    if (timer->isActive()) {
        emit logMessage("The loop is currently active.");
    } else {
        emit logMessage("Loop started.");
        state = 0;
        timer->start(2000);
    }
}

void AdLoop::stop() {
    emit logMessage("Loop stopped.");
    timer->stop();
}

void AdLoop::setupAdbEnvironment(QProcess &process) {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString adbHome = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir().mkpath(adbHome);
    env.insert("ANDROID_SDK_HOME", adbHome);
    process.setProcessEnvironment(env);
}

void AdLoop::captureScreen() {
    QProcess process;
    setupAdbEnvironment(process);
    QStringList args;
    args << "-s" << adbPortAddress << "shell" << "screencap" << "-p" << "/sdcard/screenshot.png";
    process.start(adbPath, args);
    process.waitForFinished();

    QProcess pullProcess;
    setupAdbEnvironment(pullProcess);
    QStringList pullArgs;
    pullArgs << "-s" << adbPortAddress << "pull" << "/sdcard/screenshot.png" << "screenshot.png";
    pullProcess.start(adbPath, pullArgs);
    pullProcess.waitForFinished();

    QImage img;
    if (img.load("screenshot.png")) {
        screenImg = img;
        QString path = "file:///" + QDir::currentPath() + "/screenshot.png?r=" + QString::number(QDateTime::currentMSecsSinceEpoch());
        emit screenUpdated(path);
    } else {
        emit logMessage("Failed to decode bluestacks screen image");
    }
}

void AdLoop::tap(int x, int y) {
    QProcess process;
    setupAdbEnvironment(process);
    QStringList args;
    args << "-s" << adbPortAddress << "shell" << "input" << "tap" << QString::number(x) << QString::number(y);
    process.start(adbPath, args);
    process.waitForFinished();
}

QString AdLoop::solveCaptcha(QString imagePath) {
    if (imagePath.startsWith("file:///")) imagePath = imagePath.remove(0, 8);
    imagePath = QDir::toNativeSeparators(imagePath);
    QString encoded = imagePath.replace("\\", "%5C");

    QProcess process;
    process.start("curl", QStringList() << "-s" << ("http://localhost:9119/?path=" + encoded));
    process.waitForFinished(10000);

    QString result = process.readAllStandardOutput().trimmed();
    emit logMessage("Captcha prediction: " + result);
    return result;
}

QPoint AdLoop::findTemplate(QImage sourceTemplate, QString templateName) {
    QImage converted = sourceTemplate.convertToFormat(QImage::Format_RGB888);
    cv::Mat image(converted.height(), converted.width(), CV_8UC3, const_cast<uchar*>(converted.constBits()), converted.bytesPerLine());
    QDir templatePath("./templates");
    QStringList filter;
    filter << QString("*%1*").arg(templateName);
    QFileInfoList files = templatePath.entryInfoList(filter, QDir::Files);
    for (const QFileInfo &file : files) {
        cv::Mat temp = cv::imread(file.absoluteFilePath().toStdString(), cv::IMREAD_COLOR);
        if (temp.empty()) continue;
        if (temp.cols > image.cols || temp.rows > image.rows) continue;
        cv::cvtColor(temp, temp, cv::COLOR_BGR2RGB);
        cv::Mat results;
        cv::matchTemplate(image, temp, results, cv::TM_CCOEFF_NORMED);

        double maxVal;
        cv::Point maxLoc;
        cv::minMaxLoc(results, nullptr, &maxVal, nullptr, &maxLoc);
        if (maxVal >= 0.8) return QPoint(maxLoc.x, maxLoc.y);
    }
    return QPoint(-1, -1);
}

int AdLoop::checkState() {
    captureScreen();
    QPoint adWatchButtonLoc = findTemplate(screenImg, QString("adWatchButton"));
    QPoint XButton1Loc = findTemplate(screenImg, QString("XButton1"));
    QPoint XButton1Loc_black = findTemplate(screenImg, QString("XButton1_black"));
    QPoint XButton1Loc_skip = findTemplate(screenImg, QString("XButton1_skip"));
    QPoint XButton2Loc = findTemplate(screenImg, QString("XButton2"));
    QPoint captchaTextboxLoc = findTemplate(screenImg, QString("captchaTextbox"));
    QPoint successScreen = findTemplate(screenImg, QString("successScreen"));
    QPoint adContinueScreen = findTemplate(screenImg, QString("adContinueScreen"));
    QPoint adWaitingScreen = findTemplate(screenImg, QString("adWaitingScreen"));

    if (adContinueScreen.x() != -1) {
        return 5;
    } else if (XButton2Loc.x() != -1) {
        return 3;
    } else if (XButton1Loc.x() != -1 || XButton1Loc_black.x() != -1 || XButton1Loc_skip.x() != -1) {
        if ((XButton1Loc.x() < 100 && XButton1Loc.x() > 0) ||
            (XButton1Loc_black.x() < 100 && XButton1Loc_black.x() > 0) ||
            (XButton1Loc_skip.x() < 100 && XButton1Loc_skip.x() > 0)) {
            return 21;
        } else {
            return 22;
        }
    } else if (adWaitingScreen.x() != -1) {
        return 20;
    } else if (adWatchButtonLoc.x() != -1) {
        return 0;
    } else if (captchaTextboxLoc.x() != -1) {
        return 1;
    } else if (successScreen.x() != -1) {
        return 4;
    }
    return 2;
}

void AdLoop::loop() {
    int checkedState = checkState();
    if (checkedState == 22) {
        emit logMessage("state 2: Ad / Waiting state");
        tap(XButton1.x(), XButton1.y());
        emit logMessage("Tap at XButton1");
    } else if (checkedState == 21) {
        emit logMessage("state 2: Ad / Waiting state");
        tap(XButton1_left.x(), XButton1_left.y());
        emit logMessage("Tap at XButton1_left");
    } else if (checkedState == 20) {
        emit logMessage("state 2: ad waiting screen");
    } else if (checkedState == 0) {
        emit logMessage("state 0: main screen");
        tap(adWatchButton.x(), adWatchButton.y());
        emit logMessage("Tapped at adWatchButton");
    } else if (checkedState == 1) {
        emit logMessage("state 1: captcha");
        QRect captchaRec = QRect(captchaCorner1, captchaCorner2).normalized();
        QImage captchaImg = screenImg.copy(captchaRec);
        captchaImg.save("temp_captcha.png");
        QString code = solveCaptcha(QDir::currentPath() + "/temp_captcha.png");
        if (code.length() == 5) {
            tap(captchaTextbox.x(), captchaTextbox.y());
            emit logMessage("Tapped at captchaTextbox");
            QThread::msleep(300);
            QProcess process;
            setupAdbEnvironment(process);
            QStringList args;
            args << "-s" << adbPortAddress << "shell" << "input" << "text" << code;
            process.start(adbPath, args);
            process.waitForFinished();
            tap(captchaConfirm.x(), captchaConfirm.y());
            emit logMessage("Tapped at captchaConfirm");
        } else {
            tap(XButton1.x(), XButton1.y());
            emit logMessage("Tapped at XButton1. Captcha retry.");
        }
    } else if (checkedState == 3) {
        emit logMessage("state 3: Ad popup");
        tap(XButton2.x(), XButton2.y());
        emit logMessage("Tap at XButton2");
    } else if (checkedState == 4) {
        emit logMessage("State 4: Success screen");
        tap(XButton1.x(), XButton1.y());
        emit logMessage("Tapped at XButton1");
    } else if (checkedState == 5) {
        emit logMessage("State 5: Continue Ad screen");
        tap(continueAdButton.x(), continueAdButton.y());
        emit logMessage("Tapped at continue button.");
    } else {
        emit logMessage("Undefined state");
    }
}