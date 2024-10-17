#ifdef _WINDOWS
#include <windows.h>
#endif
#include <gtest/gtest.h>
#include "email.h"
#include "DateUtil.hpp"

TEST(test, basics)
{
    EmailComposer composer;
    composer.setFrom("jmjatlanta@gmail.com");
    composer.setSubject("My Test Subject Line");
    composer.setBody("This is a test. I am sending from a test application. Sent at " + to_string(std::chrono::system_clock::now()));
    composer.addTo("jmjatlanta@yahoo.com");
    composer.setSmtpUserPassword("jgfglsazhbcydnsr");

    if (std::filesystem::exists("CMakeLists.txt"))
        composer.addAttachment(std::filesystem::absolute("CMakeLists.txt"));
    else
    {
        if (std::filesystem::exists("cmake_install.cmake"))
            composer.addAttachment(std::filesystem::absolute("cmake_install.cmake"));
    }

    EXPECT_TRUE(composer.compose());
}

