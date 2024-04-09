#ifdef _WINDOWS
#include <windows.h>
#endif
#include <gtest/gtest.h>
#include "email.h"

TEST(test, basics)
{
    EmailComposer composer;
    composer.setSubject("Test 1");
    composer.setBody("This is a test. Will this work?");
    composer.addTo("jmjatlanta@yahoo.com");
    composer.addBCC("john.jones@jmjatlanta.com");
    if (std::filesystem::exists("CMakeLists.txt"))
	    composer.addAttachment(std::filesystem::absolute("CMakeLists.txt"));
    else
    {
        if (std::filesystem::exists("cmake_install.cmake"))
            composer.addAttachment(std::filesystem::absolute("cmake_install.cmake"));
    }

    EXPECT_TRUE(composer.compose());
}

