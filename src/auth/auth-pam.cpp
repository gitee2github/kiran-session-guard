/**
  * @Copyright (C) 2020 ~ 2021 KylinSec Co., Ltd.
  *
  * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; If not, see <http: //www.gnu.org/licenses/>. 
  */

#include "auth-pam.h"
#include "pam-message.h"

#include <qt5-log-i.h>
#include <QProcess>

#include <signal.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <QApplication>
#include <QDBusConnection>
#include <QJsonObject>
#include <QSocketNotifier>

#define CHECKPASS_PATH "/home/lxh/code/kiran-screensaver-dialog/cmake-build-debug/kiran-screensaver-checkpass/kiran-screensaver-checkpass"
#define CHECKPASS_NAME "kiran-screensaver-checkpass"

#define READ_CHANNEL 0
#define WRITE_CHANNEL 1

AuthPam::AuthPam(QObject *parent)
    : AuthBase(parent)
{
}

AuthPam::~AuthPam()
{
}

bool AuthPam::init()
{
    return false;
}

bool AuthPam::authenticate(const QString &userName)
{
    if (inAuthentication())
    {
        cancelAuthentication();
    }

    if (pipe(m_toParentPipe) == -1 || pipe(m_toChildPipe) == -1)
    {
        KLOG_ERROR() << "can't create pipe before fork," << strerror(errno);
        return false;
    }

    m_userName = userName;

    pid_t forkPid = fork();

    //fork出错
    if (forkPid == -1)
    {
        KLOG_ERROR() << "fork error," << strerror(errno);
        close(m_toParentPipe[WRITE_CHANNEL]);
        close(m_toParentPipe[READ_CHANNEL]);
        close(m_toChildPipe[WRITE_CHANNEL]);
        close(m_toChildPipe[READ_CHANNEL]);
        m_userName = "";
        return false;
    }

    //认证子进程
    if (forkPid == 0)
    {
        close(m_toParentPipe[READ_CHANNEL]);
        close(m_toChildPipe[WRITE_CHANNEL]);
        if (execlp(CHECKPASS_PATH,
                   QString::number(m_toChildPipe[READ_CHANNEL]).toStdString().c_str(),
                   QString::number(m_toParentPipe[WRITE_CHANNEL]).toStdString().c_str(),
                   m_userName.toStdString().c_str(), nullptr) == -1)
        {
            KLOG_ERROR() << "execl failed," << strerror(errno);
        }
        ::_exit(-1);
    }

    m_inAuthenticating = true;

    //父进程
    m_authPid = forkPid;

    close(m_toParentPipe[WRITE_CHANNEL]);
    m_toParentPipe[WRITE_CHANNEL] = 0;

    close(m_toChildPipe[READ_CHANNEL]);
    m_toChildPipe[READ_CHANNEL] = 0;

    //监听管道可读消息
    m_socketNotifier = new QSocketNotifier(m_toParentPipe[READ_CHANNEL], QSocketNotifier::Read);
    connect(m_socketNotifier, &QSocketNotifier::activated, this, &AuthPam::handlePipeActivated);

    return true;
}

void AuthPam::respond(const QString &response)
{
    if (!inAuthentication())
    {
        return;
    }
    PromptReplyEvent promptReplyEvent(true, response);
    kiran_pam_message_send_event(m_toChildPipe[WRITE_CHANNEL], &promptReplyEvent);
}

bool AuthPam::inAuthentication() const
{
    return m_inAuthenticating;
}

bool AuthPam::isAuthenticated() const
{
    return m_isAuthenticated;
}

QString AuthPam::authenticationUser() const
{
    return m_userName;
}

void AuthPam::cancelAuthentication()
{
    if (m_authPid != 0)
    {
        kill(m_authPid, SIGKILL);
        waitpid(m_authPid, nullptr, WNOHANG);
        m_authPid = 0;
    }

    if (m_toChildPipe[WRITE_CHANNEL] != 0)
    {
        close(m_toChildPipe[WRITE_CHANNEL]);
    }

    if (m_toParentPipe[READ_CHANNEL] != 0)
    {
        close(m_toParentPipe[READ_CHANNEL]);
    }

    m_isAuthenticated = false;
    m_inAuthenticating = false;
    m_hasSendCompleteSignal = false;

    if (m_socketNotifier)
    {
        disconnect(m_socketNotifier, &QSocketNotifier::activated, this, &AuthPam::handlePipeActivated);
        delete m_socketNotifier;
        m_socketNotifier = nullptr;
    }

    m_userName.clear();
}

void AuthPam::handlePipeActivated()
{
    QJsonDocument doc;

    PamEvent *pamEvent = nullptr;
    if (!kiran_pam_message_recv_event(m_toParentPipe[READ_CHANNEL], &pamEvent))
    {
        handleChildExit();
        return;
    }

    switch (pamEvent->type())
    {
    case PamEvent::Error:
        KLOG_ERROR() << "recv checkpass pam error:" << pamEvent->text();
        break;
    case PamEvent::PromptRequest:
    {
        auto event = dynamic_cast<PromptRequestEvent *>(pamEvent);
        Kiran::PromptType promptType = event->secret() ? Kiran::PromptTypeSecret : Kiran::PromptTypeQuestion;
        KLOG_DEBUG() << "recv checkpass prompt message\n"
                     << "type:" << promptType << "\n"
                     << "text:" << event->text();
        emit showPrompt(event->text(), promptType);
        break;
    }
    case PamEvent::Message:
    {
        auto event = dynamic_cast<MessageEvent *>(pamEvent);
        Kiran::MessageType messageType = event->isError() ? Kiran::MessageTypeError : Kiran::MessageTypeInfo;
        KLOG_DEBUG() << "recv checkpass auth message\n"
                     << "type:" << messageType << "\n"
                     << "text:" << event->text();
        emit showMessage(event->text(), messageType);
        break;
    }
    case PamEvent::Complete:
    {
        auto event = dynamic_cast<CompleteEvent *>(pamEvent);
        m_hasSendCompleteSignal = true;
        KLOG_DEBUG() << "recv checkpass auth complete\n"
                     << "complete:" << event->isComplete() << "\n"
                     << "result:  " << event->authResult() << "\n"
                     << "text:    " << event->text();
        m_isAuthenticated = event->authResult();
        emit authenticationComplete();
        break;
    }
    case PamEvent::LAST:
    case PamEvent::PromptReply:
    default:
        KLOG_ERROR() << "can't supported this event" << pamEvent->type();
        break;
    }

    kiran_pam_message_free(&pamEvent);
}

void AuthPam::handleChildExit()
{
    KLOG_DEBUG() << "handle child process exit";
    waitpid(-1, nullptr, WNOHANG);
    KLOG_DEBUG() << "child process exit finished";

    m_inAuthenticating = false;

    if (m_toParentPipe[READ_CHANNEL])
    {
        close(m_toParentPipe[READ_CHANNEL]);
    }

    if (m_toChildPipe[WRITE_CHANNEL])
    {
        close(m_toChildPipe[WRITE_CHANNEL]);
    }

    if (m_socketNotifier != nullptr)
    {
        delete m_socketNotifier;
        m_socketNotifier = nullptr;
    }

    //中途意外结束，未发认证完成信号的时候，补上认证完成信号
    if (!m_hasSendCompleteSignal)
    {
        emit authenticationComplete();
    }
}