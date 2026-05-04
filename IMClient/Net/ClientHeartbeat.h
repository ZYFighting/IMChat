#ifndef CLIENTHEARTBEAT_H
#define CLIENTHEARTBEAT_H

#include <QObject>
#include <QTimer>
#include <QDebug>
#include <QPointer>
#include <QMutex>
#include "Net/def.h"

class INetMediator;
class ClientHeartbeat : public QObject
{
    Q_OBJECT
public:
    explicit ClientHeartbeat(INetMediator* mediator, QObject* parent = nullptr);
    ~ClientHeartbeat() override;

    //开始心跳检测
    void start();
    //停止所有定时器
    void stopFromAnyThread();

signals:
    //停止定时器信号
    void sig_stopTimerSignal();
    //心跳包超时信号
    void sig_heartbeatTimeout();

public slots:
    //心跳包响应
    void onHeartbeatReceived();

private slots:
    //发送心跳包
    void slot_onSendHeartbeat();
    //检测超时
    void slot_onCheckTimeout();

private:
    QPointer<INetMediator> m_mediator;
    QTimer m_sendTimer;
    QTimer m_timeoutTimer;
    QMutex m_mutex;
    bool m_respReceived;
    bool m_isRunning;
};

#endif
