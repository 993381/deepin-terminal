/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     sunchengxi <sunchengxi@uniontech.com>
*
* Maintainer: sunchengxi <sunchengxi@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "pagesearchbar.h"
#include "utils.h"
#include "mainwindow.h"

#include <DLog>

#include <QToolButton>
#include <QGraphicsOpacityEffect>
#include <QKeyEvent>
#include <QShortcut>
#include <QApplication>
#include <QAction>

PageSearchBar::PageSearchBar(QWidget *parent) : DFloatingWidget(parent)
{
    Utils::set_Object_Name(this);
    // Init
    hide();
    setFixedSize(barWidth, barHight + 12);
    // 设置窗体透明度的，需求为100%
    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect;
    setGraphicsEffect(opacityEffect);
    opacityEffect->setOpacity(opacity);

    initSearchEdit();
    initFindNextButton();
    initFindPrevButton();

    // Init layout and widgets.
    QHBoxLayout *m_layout = new QHBoxLayout();
    m_layout->setSpacing(widgetSpace);
    m_layout->setContentsMargins(layoutMargins, layoutMargins, layoutMargins, layoutMargins);
    m_layout->addWidget(m_searchEdit);
    m_layout->addWidget(m_findPrevButton);
    m_layout->addWidget(m_findNextButton);
    setLayout(m_layout);
}

bool PageSearchBar::isFocus()
{
    MainWindow *minwindow = Utils::getMainWindow(this);
    DIconButton *addButton = minwindow->findChild<DIconButton *>("AddButton");
    if (addButton != nullptr) {
        QWidget::setTabOrder(m_findNextButton, addButton);
    } else {
        qDebug() << "can not found AddButton in DIconButton";
    }
    //QWidget::setTabOrder(m_findPrevButton, m_findNextButton);

    return m_searchEdit->lineEdit()->hasFocus();
}

void PageSearchBar::focus()
{
    m_searchEdit->lineEdit()->setFocus();
    m_searchEdit->lineEdit()->selectAll();
    //　焦点一进入以后，就设置文字和图标，用于失去焦点后显示
    recoveryHoldContent();
}

QString PageSearchBar::searchKeytxt()
{
    return m_searchEdit->text();
}

void PageSearchBar::saveOldHoldContent()
{
    m_originalPlaceHolder = m_searchEdit->placeHolder();
}

//查找DSearchEdit中的DIconButton
/*******************************************************************************
 1. @函数:    findIconBtn
 2. @作者:    ut000439 wangpeili
 3. @日期:    2020-08-11
 4. @说明:    查找DSearchEdit中的DIconButton
*******************************************************************************/
DIconButton *findIconBtn(DSearchEdit *searchEdit)
{
    QWidget *iconWidget = searchEdit->findChild<QWidget *>("iconWidget");
    if (iconWidget != nullptr) {
        DIconButton *iconBtn = iconWidget->findChild<DIconButton *>();
        if (nullptr == iconBtn) {
            qDebug() << "can not found iconBtn in DIconButton";
        }
        return iconBtn;
    } else {
        qDebug() << "can not found iconWidget in QWidget";
        if (searchEdit->findChild<DIconButton *>() == nullptr) {
            qDebug() << "can not found searchEdit in DIconButton";
        }
        return searchEdit->findChild<DIconButton *>();
    }
}

void PageSearchBar::clearHoldContent()
{
    // 置空内容
    m_searchEdit->setPlaceHolder("");
    DIconButton *iconBtn = findIconBtn(m_searchEdit);
    if (iconBtn != nullptr) {
        iconBtn->setIcon(QIcon(""));
    }
}

void PageSearchBar::recoveryHoldContent()
{
    // 还原文本
    m_searchEdit->setPlaceHolder(m_originalPlaceHolder);
    DIconButton *iconBtn = findIconBtn(m_searchEdit);
    if (iconBtn != nullptr) {
        // 还原图标
        iconBtn->setIcon(DStyle::SP_IndicatorSearch);
    }
}

qint64 PageSearchBar::searchCostTime()
{
    if (0 == m_searchStartTime) {
        qDebug() << __FUNCTION__ << "search time error!";
        return -1;
    }
    qint64 costTime = QDateTime::currentMSecsSinceEpoch() - m_searchStartTime;
    m_searchStartTime = 0;
    return costTime;
}

void PageSearchBar::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return: {
        // 搜索框有焦点的情况下
        if (m_searchEdit->lineEdit()->hasFocus()) {
            Qt::KeyboardModifiers modify = event->modifiers();
            // 有shift修饰， Qt::KeypadModifier 修饰是否是小键盘
            if (modify == Qt::ShiftModifier
                    || modify  == (Qt::ShiftModifier | Qt::KeypadModifier)) {
                // shift + enter 查找前一个
                m_findPrevButton->animateClick(80);
            }
            // 没有shift修饰
            else if (modify == Qt::NoModifier || modify == Qt::KeypadModifier)
                // 查找下一个
                m_findNextButton->animateClick(80);
        }
    }
    break;
    default:
        DFloatingWidget::keyPressEvent(event);
        break;
    }
}

void PageSearchBar::initFindPrevButton()
{
    m_findPrevButton = new DIconButton(QStyle::SP_ArrowUp);
    m_findNextButton->setObjectName("PageSearchBarFindNextDIconButton");//Add by ut001000 renfeixiang 2020-08-13
    m_findPrevButton->setFixedSize(widgetHight, widgetHight);
    m_findPrevButton->setFocusPolicy(Qt::TabFocus);

    connect(m_findPrevButton, &DIconButton::clicked, this, [this]() {
        if (!m_searchEdit->lineEdit()->text().isEmpty()) {
            m_searchStartTime = QDateTime::currentMSecsSinceEpoch();
            emit findPrev();
        }
    });
}

void PageSearchBar::initFindNextButton()
{
    m_findNextButton = new DIconButton(QStyle::SP_ArrowDown);
    m_findNextButton->setObjectName("PageSearchBarFindNextDIconButton");//Add by ut001000 renfeixiang 2020-08-13
    m_findNextButton->setFixedSize(widgetHight, widgetHight);
    m_findNextButton->setFocusPolicy(Qt::TabFocus);

    connect(m_findNextButton, &DIconButton::clicked, this, [this]() {
        if (!m_searchEdit->lineEdit()->text().isEmpty()) {
            m_searchStartTime = QDateTime::currentMSecsSinceEpoch();
            emit findNext();
        }
    });
}

void PageSearchBar::initSearchEdit()
{
    m_searchEdit = new DSearchEdit(this);
    //fix bug#64976 按tab键循环切换到右上角X按钮时继续按tab键，焦点不能切换到搜索框
    //设置m_searchEdit->lineEdit()为m_searchEdit的focus代理
    m_searchEdit->setFocusPolicy(Qt::StrongFocus);
    m_searchEdit->setFocusProxy(m_searchEdit->lineEdit());
    m_searchEdit->setObjectName("PageSearchBarSearchEdit");//Add by ut001000 renfeixiang 2020-08-13
    m_searchEdit->lineEdit()->setMinimumHeight(widgetHight);

    //　保留原文字，图标
    saveOldHoldContent();
    // 置空
    clearHoldContent();

    // 把ＤＴＫ好容易改造的功能，还原了．．．．．．．．
    // 把那个＂Ｘ＂控件功能还原成仅为清空文本
    QList<QToolButton *> list = m_searchEdit->lineEdit()->findChildren<QToolButton *>();
    QAction *clearAction = m_searchEdit->lineEdit()->findChild<QAction *>(QLatin1String("_q_qlineeditclearaction"));
    if (clearAction != nullptr) {
        for (int i = 0; i < list.count(); i++) {
            if (list.at(i)->defaultAction() == clearAction) {
                QToolButton *clearBtn = list.at(i);
                //屏蔽lineedit清除按钮的槽函数,_q_clearFocus()获得有效的判断条件
                clearBtn->disconnect(SIGNAL(clicked()));
                connect(clearBtn, &QToolButton::clicked, this, [this]() {
                    m_searchEdit->lineEdit()->setText("");
                });
            }
        }
    } else {
        qDebug() << "can not found _q_qlineeditclearaction in QAction";
    }

    // 需求不让自动查找，这个接口预留
    connect(m_searchEdit, &DSearchEdit::textChanged, this, [this]() {
        emit keywordChanged(m_searchEdit->lineEdit()->text());
    });
}

void PageSearchBar::setNoMatchAlert(bool isAlert)
{
    m_searchEdit->setAlert(isAlert);
}
