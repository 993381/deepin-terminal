#ifndef UT_REMOTEMANAGEMENTPANEL_TEST_H
#define UT_REMOTEMANAGEMENTPANEL_TEST_H

#include "../common/ut_defines.h"
#include "serverconfigmanager.h"

#include <gtest/gtest.h>

class UT_RemoteManagementPanel_Test : public ::testing::Test
{
public:
    UT_RemoteManagementPanel_Test();

public:
    //这里的几个函数都会自动调用

    //用于做一些初始化操作
    virtual void SetUp();

    //用于做一些清理操作
    virtual void TearDown();

    ServerConfigManager *m_serverConfigManager = nullptr;
};

#endif // UT_REMOTEMANAGEMENTPANEL_TEST_H
