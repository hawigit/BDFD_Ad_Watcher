#include "backend.h"
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QProcessEnvironment>

backend::backend(QObject *parent)
    : QObject{parent}
{
    loadSettings();
    adbPath = QDir::currentPath() + "/adb.exe";

    // Setup worker thread
    loopThread = new QThread(this);
    adLoop = new AdLoop();
    adLoop->moveToThread(loopThread);

    connect(this, &backend::requestStart, adLoop, &AdLoop::start);
    connect(this, &backend::requestStop, adLoop, &AdLoop::stop);
    connect(this, &backend::settingsUpdated, adLoop, &AdLoop::updateCoordinates);
    connect(adLoop, &AdLoop::logMessage, this, &backend::log);
    connect(adLoop, &AdLoop::screenUpdated, this, &backend::onScreenUpdated);
    connect(loopThread, &QThread::finished, adLoop, &QObject::deleteLater);

    loopThread->start();
    sendSettingsToWorker();

    // Start OCR server
    QString pythonPath = QDir::currentPath() + "/ocr_env/Scripts/python.exe";
    QString scriptPath = QDir::currentPath() + "/ocr.py";
    ocrProcess = new QProcess(this);
    ocrProcess->start(pythonPath, QStringList() << scriptPath);
    ocrProcess->waitForReadyRead(15000); // wait until "OCR server ready"
    log("OCR server started");
}

backend::~backend() {
    if (loopThread && loopThread->isRunning()) {
        emit requestStop();
        loopThread->quit();
        loopThread->wait(3000);
    }
    if (ocrProcess && ocrProcess->state() != QProcess::NotRunning) {
        log("Stopping OCR server...");
        ocrProcess->terminate();
        if (!ocrProcess->waitForFinished(3000)) {
            ocrProcess->kill();
        }
    }
    if (adbConnected) {
        QProcess adbProcess;
        setupAdbEnvironment(adbProcess);
        QStringList args;
        args << "disconnect" << adbPortAddress;
        adbProcess.start(adbPath, args);
        adbProcess.waitForFinished(2000);
        log("Disconnected from BlueStacks ADB.");
    }
}

void backend::sendSettingsToWorker() {
    emit settingsUpdated(XButton1, XButton1_left, XButton2, adWatchButton,
                         captchaTextbox, captchaConfirm, continueAdButton,
                         adbPortAddress, adbPath);
}

void backend::start() {
    sendSettingsToWorker();
    emit requestStart();
}

void backend::stop() {
    emit requestStop();
}

void backend::onScreenUpdated(QString path) {
    imgPath = path;
    emit imgPathChanged();
}

void backend::captureScreen() {
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
        imgPath = "file:///" + QDir::currentPath() + "/screenshot.png?r=" + QString::number(QDateTime::currentMSecsSinceEpoch());
        emit imgPathChanged();
    } else {
        log("Failed to decode bluestacks screen image");
    }
}

void backend::connectBlueStacks() {
    QProcess process;
    setupAdbEnvironment(process);
    QStringList args;
    if (adbConnected) {
        args << "disconnect" << adbPortAddress;
    } else {
        args << "connect" << adbPortAddress;
    }
    adbConnected = !adbConnected;
    process.start(adbPath, args);
    process.waitForFinished();

    QString output = process.readAllStandardOutput();
    backend::log("Output: " + output);
}

void backend::setLocXButton1(QPoint point) {
    XButton1 = point;
    saveSettings();
    sendSettingsToWorker();
}

void backend::setLocXButton1_left(QPoint point) {
    XButton1_left = point;
    saveSettings();
    sendSettingsToWorker();
}

void backend::setLocXButton2(QPoint point) {
    XButton2 = point;
    saveSettings();
    sendSettingsToWorker();
}

void backend::setAdWatchButton(QPoint point) {
    adWatchButton = point;
    saveSettings();
    sendSettingsToWorker();
}

void backend::setCaptchaTextbox(QPoint point) {
    captchaTextbox = point;
    saveSettings();
    sendSettingsToWorker();
}

void backend::setCaptchaConfirm(QPoint point) {
    captchaConfirm = point;
    saveSettings();
    sendSettingsToWorker();
}

void backend::updatePort(QString port) {
    if (adbPortAddress != port) {
        adbPortAddress = port;
        saveSettings();
        sendSettingsToWorker();
        emit adbPortChanged();
    }
}

void backend::setContinueAd(QPoint point) {
    continueAdButton = point;
    saveSettings();
    sendSettingsToWorker();
}

void backend::setupAdbEnvironment(QProcess &process) {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString adbHome = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir().mkpath(adbHome);
    env.insert("ANDROID_SDK_HOME", adbHome);
    process.setProcessEnvironment(env);
}

void backend::saveSettings() {
    QJsonObject obj;
    obj["XButton1_x"] = XButton1.x();
    obj["XButton1_y"] = XButton1.y();
    obj["XButton1_left_x"] = XButton1_left.x();
    obj["XButton1_left_y"] = XButton1_left.y();
    obj["XButton2_x"] = XButton2.x();
    obj["XButton2_y"] = XButton2.y();
    obj["adWatchButton_x"] = adWatchButton.x();
    obj["adWatchButton_y"] = adWatchButton.y();
    obj["captchaTextbox_x"] = captchaTextbox.x();
    obj["captchaTextbox_y"] = captchaTextbox.y();
    obj["captchaConfirm_x"] = captchaConfirm.x();
    obj["captchaConfirm_y"] = captchaConfirm.y();
    obj["continueAdButton_x"] = continueAdButton.x();
    obj["continueAdButton_y"] = continueAdButton.y();
    obj["adbPortAddress"] = adbPortAddress;

    QJsonDocument doc(obj);
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir().mkpath(dir);
    QFile file(dir + "/settings.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
        log("Settings saved to " + file.fileName());
    }
}

void backend::loadSettings() {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QFile file(dir + "/settings.json");
    if (!file.exists()) {
        return;
    }
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isNull() && doc.isObject()) {
            QJsonObject obj = doc.object();
            if (obj.contains("XButton1_x")) XButton1.setX(obj["XButton1_x"].toInt());
            if (obj.contains("XButton1_y")) XButton1.setY(obj["XButton1_y"].toInt());
            if (obj.contains("XButton1_left_x")) XButton1_left.setX(obj["XButton1_left_x"].toInt());
            if (obj.contains("XButton1_left_y")) XButton1_left.setY(obj["XButton1_left_y"].toInt());
            if (obj.contains("XButton2_x")) XButton2.setX(obj["XButton2_x"].toInt());
            if (obj.contains("XButton2_y")) XButton2.setY(obj["XButton2_y"].toInt());
            if (obj.contains("adWatchButton_x")) adWatchButton.setX(obj["adWatchButton_x"].toInt());
            if (obj.contains("adWatchButton_y")) adWatchButton.setY(obj["adWatchButton_y"].toInt());
            if (obj.contains("captchaTextbox_x")) captchaTextbox.setX(obj["captchaTextbox_x"].toInt());
            if (obj.contains("captchaTextbox_y")) captchaTextbox.setY(obj["captchaTextbox_y"].toInt());
            if (obj.contains("captchaConfirm_x")) captchaConfirm.setX(obj["captchaConfirm_x"].toInt());
            if (obj.contains("captchaConfirm_y")) captchaConfirm.setY(obj["captchaConfirm_y"].toInt());
            if (obj.contains("continueAdButton_x")) continueAdButton.setX(obj["continueAdButton_x"].toInt());
            if (obj.contains("continueAdButton_y")) continueAdButton.setY(obj["continueAdButton_y"].toInt());
            if (obj.contains("adbPortAddress")) adbPortAddress = obj["adbPortAddress"].toString();
            log("Settings loaded from " + file.fileName());
        }
    }
}

void backend::log(QString text) {
    qDebug() << text;
    emit logUpdated(QString("%1").arg(text));
}

void backend::captureWidget(QString widgetName) {
    QRect rectangle = QRect(captureWidget1, captureWidget2).normalized();
    QDir directory = QDir("./templates");
    if (!directory.exists()) {
        if (!directory.mkpath(".")) {
            log("Can't create templates directory.");
        }
    }
    QImage croppedImg = screenImg.copy(rectangle);
    QString filePath = directory.filePath(widgetName + ".png");
    if (croppedImg.save(filePath)) {
        log(QString("Saved %1.png successfully").arg(widgetName));
    } else {
        log(QString("Failed to save %1.png").arg(widgetName));
    }
}

QPoint backend::getPos(int x, int y, double width, double height, int mode, int clickCount, QString widgetName) {
    if (screenImg.isNull()) log("No Image");
    int realX = x * (screenImg.width() / width);
    int realY = y * (screenImg.height() / height);
    if (realX < 0 || realX > screenImg.width() || realY < 0 || realY > screenImg.height()) {
        log("Position out of bounds.");
    } else {
        QColor color = screenImg.pixelColor(realX, realY);
        if (mode == 0) {
            log(QString("Pixel: %1 x %2 | Color: Hex %3 | RGB %4, %5, %6").arg(realX).arg(realY).arg(color.name()).arg(color.red()).arg(color.green()).arg(color.blue()));
            return QPoint(realX, realY);
        } else if (mode == 1) {
            setLocXButton1(QPoint(realX, realY));
            log(QString("Set X button 1 to pixel %1 x %2").arg(realX).arg(realY));
        } else if (mode == 2) {
            setLocXButton2(QPoint(realX, realY));
            log(QString("Set X button 2 to pixel %1 x %2").arg(realX).arg(realY));
        } else if (mode == 3) {
            setAdWatchButton(QPoint(realX, realY));
            log(QString("Set Ad Watch Button to pixel %1 x %2").arg(realX).arg(realY));
        } else if (mode == 4) {
            setCaptchaTextbox(QPoint(realX, realY));
            log(QString("Set Captcha Textbox to pixel %1 x %2").arg(realX).arg(realY));
        } else if (mode == 5) {
            setCaptchaConfirm(QPoint(realX, realY));
            log(QString("Set Captcha Confirm to pixel %1 x %2").arg(realX).arg(realY));
        } else if (mode == 6) {
            if (clickCount == 1) captureWidget1 = QPoint(realX, realY);
            else if (clickCount == 2) {
                captureWidget2 = QPoint(realX, realY);
                captureWidget(widgetName);
            }
        } else if (mode == 7) {
            setContinueAd(QPoint(realX, realY));
            log(QString("Set Continue Ad Button to pixel %1 x %2").arg(realX).arg(realY));
        } else if (mode == 8) {
            setLocXButton1_left(QPoint(realX, realY));
            log(QString("Set X Button (left) to pixel %1 x %2").arg(realX).arg(realY));
        }
    }
    return QPoint(-1, -1);
}



