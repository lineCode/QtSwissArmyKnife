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
#include <QList>
#include <QDebug>
#include <QMetaEnum>
#include <QLineEdit>

#include "SAKGlobal.hh"
#include "SAKTcpServerDeviceController.hh"
#include "ui_SAKTcpServerDeviceController.h"
SAKTcpServerDeviceController::SAKTcpServerDeviceController(QWidget *parent)
    :QWidget (parent)
    ,ui (new Ui::SAKTcpServerDeviceController)
{
    ui->setupUi(this);

    serverHostComboBox = ui->serverhostComboBox;
    serverPortLineEdit = ui->serverPortLineEdit;
    clientHostComboBox = ui->clientHostComboBox;

    refresh();
}

SAKTcpServerDeviceController::~SAKTcpServerDeviceController()
{
    delete ui;
}

QString SAKTcpServerDeviceController::serverHost()
{
    uiMutex.lock();
    QString host = serverHostComboBox->currentText();
    uiMutex.unlock();
    return host;
}

quint16 SAKTcpServerDeviceController::serverPort()
{
    uiMutex.lock();
    quint16 port = static_cast<quint16>(serverPortLineEdit->text().toInt());
    uiMutex.unlock();
    return port;
}

QString SAKTcpServerDeviceController::currentClientHost()
{
    uiMutex.lock();
    QStringList host = clientHostComboBox->currentText().split(":");
    QString address = host.first();
    uiMutex.unlock();
    return address;
}

quint16 SAKTcpServerDeviceController::currentClientPort()
{
    uiMutex.lock();
    QString port = clientHostComboBox->currentText().split(":").last();
    quint16 portTemp = static_cast<quint16>(port.toInt());
    uiMutex.unlock();
    return portTemp;
}

void SAKTcpServerDeviceController::refresh()
{
    SAKGlobal::initIpComboBox(serverHostComboBox);
}

void SAKTcpServerDeviceController::setUiEnable(bool enable)
{
    serverHostComboBox->setEnabled(enable);
    serverPortLineEdit->setEnabled(enable);
}

void SAKTcpServerDeviceController::addClient(QString host, quint16 port, QTcpSocket *socket)
{
    QString item = host.append(":");
    item.append(QString::number(port));

    uiMutex.lock();
    for(int i = 0; i < clientHostComboBox->count(); i++){
        if (clientHostComboBox->itemText(i).compare(item) == 0){
            uiMutex.unlock();
            return;
        }
    }

    clientHostComboBox->addItem(item, QVariant::fromValue(socket));
    uiMutex.unlock();
}

void SAKTcpServerDeviceController::removeClient(QTcpSocket *socket)
{
    uiMutex.lock();
    for(int i = 0; i < clientHostComboBox->count(); i++){
        if (clientHostComboBox->itemData(i).value<QTcpSocket*>() == socket){
            clientHostComboBox->removeItem(i);
            break;
        }
    }
    uiMutex.unlock();
}
