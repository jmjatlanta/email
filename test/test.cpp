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
	composer.addAttachment("CMakeLists.txt");

    EXPECT_TRUE(composer.compose());
}
