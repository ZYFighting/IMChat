#include "ClientHeartbeat.h"
#include "mediator/INetMediator.h"

ClientHeartbeat::ClientHeartbeat(INetMediator* mediator, QObject* parent)
    : QObject(parent)
    , m_mediator(mediator)
    , m_respReceived(false)
    , m_isRunning(false)
{
    //绑定触发定时器停止的信号和槽函数
    connect(this, &ClientHeartbeat::sig_stopTimerSignal, &m_sendTimer, &QTimer::stop);
    connect(this, &ClientHeartbeat::sig_stopTimerSignal, &m_timeoutTimer, &QTimer::stop);
    m_timeoutTimer.setSingleShot(true);
    //绑定触发发送心跳包的定时器的信号和槽函数
    connect(&m_sendTimer, &QTimer::timeout, this, &ClientHeartbeat::slot_onSendHeartbeat);
    //绑定触发心跳包超时的信号和槽函数
    connect(&m_timeoutTimer, &QTimer::timeout, this, &ClientHeartbeat::slot_onCheckTimeout);
}

ClientHeartbeat::~ClientHeartbeat()
{
    m_sendTimer.stop();
    m_timeoutTimer.stop();
    m_isRunning = false;
}

//开始心跳检测
void ClientHeartbeat::start()
{
    if (!m_mediator || m_isRunning) return;
    QMutexLocker locker(&m_mutex);
    m_isRunning = true;
    m_respReceived = true;
    m_sendTimer.start(HEARTBEAT_INTERVAL);
    qDebug() << "心跳检测已启动";
}

//停止所有定时器
void ClientHeartbeat::stopFromAnyThread()
{
    Q_EMIT sig_stopTimerSignal();
    QMutexLocker locker(&m_mutex);
    m_isRunning = false;
    qDebug() << "心跳检测已停止";
}

//发送心跳包
void ClientHeartbeat::slot_onSendHeartbeat()
{
    QMutexLocker locker(&m_mutex);
    if (!m_isRunning || !m_mediator) {
        locker.unlock();
        stopFromAnyThread();
        Q_EMIT sig_heartbeatTimeout();
        return;
    }
    if (!m_mediator->sendData((char*)HEARTBEAT_PACKET, sizeof(HEARTBEAT_PACKET)-1, 0)) {
        qWarning() << "心跳包发送失败";
        locker.unlock();
        stopFromAnyThread();
        Q_EMIT sig_heartbeatTimeout();
        return;
    }
    qDebug() << "心跳包已发送";
    m_respReceived = false;
    m_timeoutTimer.start(HEARTBEAT_TIMEOUT);
}

//检测超时
void ClientHeartbeat::slot_onCheckTimeout()
{
    QMutexLocker locker(&m_mutex);
    if (!m_respReceived && m_isRunning) {
        qWarning() << "心跳包超时！";
        locker.unlock();
        stopFromAnyThread();
        Q_EMIT sig_heartbeatTimeout();
    }
}

//心跳包响应
void ClientHeartbeat::onHeartbeatReceived()
{
    QMutexLocker locker(&m_mutex);
    if (!m_isRunning) return;
    m_respReceived = true;
    m_timeoutTimer.stop();
    qDebug() << "心跳包收到服务端响应";
}
