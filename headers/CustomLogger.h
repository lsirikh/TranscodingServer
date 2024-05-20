#ifndef CUSTOM_LOGGER_H
#define CUSTOM_LOGGER_H

#include "crow.h"
#include <iostream>
#include <iomanip>
#include <ctime>

class CustomLogger : public crow::ILogHandler
{
public:
    void log(std::string message, crow::LogLevel level) override;
};

#endif // CUSTOM_LOGGER_H
