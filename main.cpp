#include <QDebug>
#include "spdlog/spdlog.h"

int main()
{
    qDebug() << "Hello, World!";

    spdlog::info("Hello, World!");
    spdlog::error("Hello, World!");
    return 0;
}