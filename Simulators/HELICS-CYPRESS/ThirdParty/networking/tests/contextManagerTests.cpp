#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "gmlc/networking/AsioContextManager.h"
#include <stdlib.h>

using namespace gmlc::networking;

TEST_CASE("getContextPointerTest", "[contextManager]")
{
    auto io_context =
        gmlc::networking::AsioContextManager::getContextPointer("io_context");
    auto new_context =
        gmlc::networking::AsioContextManager::getContextPointer("io_context");
    asio::io_context* cntxt_p = &io_context->getBaseContext();
    asio::io_context* new_cntxt_p = &new_context->getBaseContext();
    CHECK(cntxt_p == new_cntxt_p);
}

TEST_CASE("closeContext", "[contextManager]")
{
    auto io_context =
        gmlc::networking::AsioContextManager::getContextPointer("io_context");
    io_context->startContextLoop();
    io_context->closeContext();
    CHECK_FALSE(io_context->isRunning());
}

TEST_CASE("getNameTest", "[contextManager]")
{
    auto context_pointer =
        gmlc::networking::AsioContextManager::getContextPointer("io_context");
    CHECK(context_pointer->getName() == "io_context");
}
TEST_CASE("getContext", "[contextManager]")
{
    auto context_pointer =
        gmlc::networking::AsioContextManager::getContextPointer("io_context");
    auto* p1 = &context_pointer->getBaseContext();
    auto* p2 = &gmlc::networking::AsioContextManager::getContext("io_context");
    CHECK(p1 == p2);
}

TEST_CASE("startContextTest", "[contextManager]")
{
    auto context_pointer =
        gmlc::networking::AsioContextManager::getContextPointer("io_context");
    CHECK_NOTHROW(context_pointer->startContextLoop());
}
TEST_CASE("runContextTest", "[contextManager]")
{
    auto context_pointer =
        gmlc::networking::AsioContextManager::getContextPointer(
            std::string("io_context"));
    CHECK_NOTHROW(gmlc::networking::AsioContextManager::runContextLoop(
        std::string("io_context")));
}
TEST_CASE("runContextTestFail", "[contextManager]")
{
    CHECK_THROWS_WITH(
        gmlc::networking::AsioContextManager::runContextLoop("nonexistent"),
        "the context name specified was not available");
}
