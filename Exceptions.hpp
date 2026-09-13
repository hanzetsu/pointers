#pragma once

#include <string>
#include <sstream>

class IException {
public:
    virtual ~IException() = default;
    virtual const char* what() const noexcept = 0;
};

class DataStructureException : public IException {
    std::string message;
public:
    explicit DataStructureException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
};

class IndexOutOfRange : public DataStructureException {
public:
    IndexOutOfRange(std::size_t index, std::size_t size, const std::string& context)
        : DataStructureException(buildMessage(index, size, context)),
          m_index(index), m_size(size) {}

    std::size_t getIndex() const noexcept { return m_index; }
    std::size_t getSize() const noexcept { return m_size; }

private:
    static std::string buildMessage(std::size_t index, std::size_t size, const std::string& context) {
        std::ostringstream oss;
        oss << "Ошибка в " << context << ": индекс " << index
            << " вне допустимого диапазона [0, " << (size == 0 ? 0 : size - 1) << "]";
        return oss.str();
    }
    std::size_t m_index;
    std::size_t m_size;
};

class EmptyStructureException : public DataStructureException {
public:
    explicit EmptyStructureException(const std::string& context)
        : DataStructureException("Ошибка в " + context + ": последовательность пуста") {}
};

class InvalidArgument : public DataStructureException {
public:
    explicit InvalidArgument(const std::string& msg) : DataStructureException(msg) {}
};

class EmptySequenceException : public EmptyStructureException {
public:
    explicit EmptySequenceException(const std::string& msg = "Последовательность пуста")
        : EmptyStructureException(msg) {}
};

class InvalidArgumentException : public InvalidArgument {
public:
    explicit InvalidArgumentException(const std::string& msg = "Неверный аргумент")
        : InvalidArgument(msg) {}
};

class OutOfRangeException : public IndexOutOfRange {
public:
    OutOfRangeException(const std::string& msg = "Индекс вне диапазона")
        : IndexOutOfRange(0, 0, msg) {}
    OutOfRangeException(std::size_t index, std::size_t size, const std::string& context)
        : IndexOutOfRange(index, size, context) {}
};

class IsInfiniteLengthException : public DataStructureException {
public:
    explicit IsInfiniteLengthException(const std::string& msg = "Длина бесконечна")
        : DataStructureException(msg) {}
};