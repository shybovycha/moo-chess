#include <iostream>
#include <format>
#include <string>
#include <print>

#include "Application.hpp"

int main(int argc, char **argv)
{
    Application *app = new Application();

    app->run();

    return 0;
}
