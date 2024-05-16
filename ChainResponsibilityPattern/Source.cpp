#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <exception>

enum class LogMessageType {
    Warning,
    Error,
    FatalError,
    Unknown
};

class LogMessage {
private:
    LogMessageType msgType;
    std::string msg;

public:
    LogMessage(LogMessageType type, const std::string& message) : msgType(type), msg(message) {}

    LogMessageType type() const {
        return msgType;
    }

    const std::string& message() const {
        return msg;
    }
};

class LogHandler {
protected:
    std::shared_ptr<LogHandler> next;

public:
    void setNext(std::shared_ptr<LogHandler> nextHandler) {
        next = nextHandler;
    }

    virtual void handle(const LogMessage& logMessage) {
        if (next) {
            next->handle(logMessage);
        }
        else {
            throw std::runtime_error("Unhandled log message: " + logMessage.message());
        }
    }
};
class FatalErrorHandler : public LogHandler {
public:
    void handle(const LogMessage& logMessage) override {
        if (logMessage.type() == LogMessageType::FatalError) {
            throw std::runtime_error("Fatal Error: " + logMessage.message());
        }
        else {
            LogHandler::handle(logMessage);
        }
    }
};

class ErrorHandler : public LogHandler {
private:
    std::string filePath;

public:
    ErrorHandler(const std::string& path) : filePath(path) {}

    void handle(const LogMessage& logMessage) override {
        if (logMessage.type() == LogMessageType::Error) {
            std::ofstream file(filePath, std::ios::app);
            if (file.is_open()) {
                file << "Error: " + logMessage.message() << std::endl;
            }
        }
        else {
            LogHandler::handle(logMessage);
        }
    }
};
class WarningHandler : public LogHandler {
public:
    void handle(const LogMessage& logMessage) override {
        if (logMessage.type() == LogMessageType::Warning) {
            std::cout << "Warning: " + logMessage.message() << std::endl;
        }
        else {
            LogHandler::handle(logMessage);
        }
    }
};
class UnknownHandler : public LogHandler {
public:
    void handle(const LogMessage& logMessage) override {
        if (logMessage.type() == LogMessageType::Unknown) {
            throw std::runtime_error("Unknown log message: " + logMessage.message());
        }
        else {
            LogHandler::handle(logMessage);
        }
    }
};
int main() {
    auto fatalHandler = std::make_shared<FatalErrorHandler>();
    auto errorHandler = std::make_shared<ErrorHandler>("errors.log");
    auto warningHandler = std::make_shared<WarningHandler>();
    auto unknownHandler = std::make_shared<UnknownHandler>();

    fatalHandler->setNext(errorHandler);
    errorHandler->setNext(warningHandler);
    warningHandler->setNext(unknownHandler);

    LogMessage warningMessage(LogMessageType::Warning, "This is a warning");
    LogMessage errorMessage(LogMessageType::Error, "This is an error");
    LogMessage fatalErrorMessage(LogMessageType::FatalError, "This is a fatal error");
    LogMessage unknownMessage(LogMessageType::Unknown, "This is an unknown message");

    try {
        fatalHandler->handle(warningMessage);
        fatalHandler->handle(errorMessage);
        fatalHandler->handle(fatalErrorMessage);
    }
    catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }

    try {
        fatalHandler->handle(unknownMessage);
    }
    catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }

    return 0;
}

