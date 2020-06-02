#include "windowsmanager.h"
#include "dbusmanager.h"
#include "utils.h"
#include "service.h"

#include <QDebug>

WindowsManager *WindowsManager::pManager = new WindowsManager();
WindowsManager *WindowsManager::instance()
{
    return  pManager;
}

void WindowsManager::runQuakeWindow(TermProperties properties)
{
    if (m_quakeWindow == nullptr) {
        qDebug() << "runQuakeWindow :create";
        m_quakeWindow = new QuakeWindow(properties);
        m_quakeWindow->show();
        return;
    }
    // Alt+F2的显隐功能实现点
    quakeWindowShowOrHide();
}

void WindowsManager::quakeWindowShowOrHide()
{
    //MainWindow *mainWindow = getMainWindow();
    qDebug() << "ShowOrHide" << m_quakeWindow->winId();

    // 没有显示，就显示．
    if (!m_quakeWindow->isVisible()) {
        qDebug() << "!mainWindow  isVisible now show !" << m_quakeWindow->winId();
        m_quakeWindow->show();
    }

    // 没有激活就激活
    if (!m_quakeWindow->isActiveWindow()) {
        qDebug() << "QuakeWindow is activate, now activateWindow" << m_quakeWindow->winId();
        int index = DBusManager::callKDECurrentDesktop();
        if (index != -1 && m_quakeWindow->getDesktopIndex() != index) {
            // 不在同一个桌面
            DBusManager::callKDESetCurrentDesktop(m_quakeWindow->getDesktopIndex());
        }
        m_quakeWindow->activateWindow();
        return;
    }

    // 如果已经激活，那么就隐藏
    qDebug() << "isWinVisible mainWindow->isActiveWindow() : start hide" << m_quakeWindow->winId();
    // 雷神的普通对话框,不处理
    if (Service::instance()->getIsDialogShow()) {
        return;
    }
    // 雷神设置框显示,不处理
    if (Service::instance()->isSettingDialogVisible() && Service::instance()->getSettingOwner() == m_quakeWindow) {
        if (m_quakeWindow->isActiveWindow()) {
            Service::instance()->showSettingDialog(m_quakeWindow);
        }
        return;
    }
    m_quakeWindow->hide();
}

void WindowsManager::createNormalWindow(TermProperties properties)
{
    TermProperties newProperties = properties;
    if (m_normalWindowList.count() == 0) {
        newProperties[SingleFlag] = true;
    }
    MainWindow *newWindow = new NormalWindow(newProperties);
    m_normalWindowList << newWindow;
    qDebug() << "create NormalWindow, current count =" << m_normalWindowList.count()
             << ", SingleFlag" << newProperties[SingleFlag].toBool();
    newWindow->show();
}

void WindowsManager::onMainwindowClosed(MainWindow *window)
{
    /***add begin by ut001121 zhangmeng 20200527 关闭终端窗口时重置设置框所有者 修复BUG28636***/
    if (window == Service::instance()->getSettingOwner()) {
        Service::instance()->resetSettingOwner();
    }
    /***add end by ut001121 zhangmeng***/

    if (window->isQuakeMode()) {
        Q_ASSERT(window == m_quakeWindow);
        m_quakeWindow->deleteLater();
        m_quakeWindow = nullptr;
        return;
    }
    if (m_normalWindowList.contains(window)) {
        m_normalWindowList.removeOne(window);
        window->deleteLater();
        return;
    }
    qDebug() << "unkown windows closed?? " << window;
}

WindowsManager::WindowsManager(QObject *parent) : QObject(parent)
{

}

int WindowsManager::widgetCount() const
{
    return m_widgetCount;
}

void WindowsManager::terminalCountIncrease()
{
    ++m_widgetCount;
    qDebug() << "++ Terminals Count : " << m_widgetCount;
}

void WindowsManager::terminalCountReduce()
{
    --m_widgetCount;
    qDebug() << "-- Terminals Count : " << m_widgetCount;
}
