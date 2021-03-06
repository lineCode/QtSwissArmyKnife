﻿/*
 * Copyright (C) 2018-2020 wuuhii. All rights reserved.
 *
 * The file is encoding with utf-8 (with BOM). It is a part of QtSwissArmyKnife
 * project. The project is a open source project, you can get the source from:
 *     https://github.com/qsak/QtSwissArmyKnife
 *     https://gitee.com/qsak/QtSwissArmyKnife
 *
 * For more information about the project, please join our QQ group(952218522).
 * In addition, the email address of the project author is wuuhii@outlook.com.
 */
#include <QDebug>
#include <QEventLoop>
#include <QApplication>

#include "SAKSerialPortDevice.hh"
#include "SAKSerialPortDebugPage.hh"
#include "SAKSerialPortDeviceController.hh"

SAKSerialPortDevice::SAKSerialPortDevice(SAKSerialPortDebugPage *debugPage, QObject *parent)
    :SAKDevice(parent)
    ,serialPort(Q_NULLPTR)
    ,debugPage(debugPage)
{

}

SAKSerialPortDevice::~SAKSerialPortDevice()
{

}

void SAKSerialPortDevice::run()
{
    QEventLoop eventLoop;
    SAKSerialPortDeviceController *controller = debugPage->controllerInstance();
    name = controller->name();
    baudRate = controller->baudRate();
    dataBits = controller->dataBits();
    stopBits = controller->stopBits();
    parity = controller->parity();
    flowControl = controller->flowControl();

    serialPort = new QSerialPort;
    serialPort->setPortName(name);
    serialPort->setBaudRate(baudRate);
    serialPort->setDataBits(dataBits);
    serialPort->setStopBits(stopBits);
    serialPort->setParity(parity);
    serialPort->setFlowControl(flowControl);

    if (serialPort->open(QSerialPort::ReadWrite)){
        emit deviceStateChanged(true);
        while (true){
            /// @brief 优雅地退出线程
            if (isInterruptionRequested()){
                break;
            }

            /// @brief 读取数据
            serialPort->waitForReadyRead(debugPage->readWriteParameters().waitForReadyReadTime);
            QByteArray bytes = serialPort->readAll();
            if (bytes.length()){
                emit bytesRead(bytes);
            }

            /// @brief 写入数据
            while (true){
                QByteArray var = takeWaitingForWrittingBytes();
                if (var.length()){
                    qint64 ret = serialPort->write(var);
                    serialPort->waitForBytesWritten(debugPage->readWriteParameters().waitForBytesWrittenTime);
                    if (ret == -1){
                        emit messageChanged(tr("串口发送数据失败：") + serialPort->errorString(), false);
                    }else{
                        emit bytesWritten(var);
                    }
                }else{
                    break;
                }
            }

            /// @brief 处理线程事件
            eventLoop.processEvents();

            /// @brief 线程睡眠
            threadMutex.lock();
            threadWaitCondition.wait(&threadMutex, 25);
            threadMutex.unlock();
        }

        /// @brief 关闭清理串口
        serialPort->clear();
        serialPort->close();
        delete serialPort;
        serialPort = Q_NULLPTR;
        emit deviceStateChanged(false);
    }else{        
        emit deviceStateChanged(false);
        emit messageChanged(tr("串口打开失败：") + serialPort->errorString(), false);
        return;
    }
}
