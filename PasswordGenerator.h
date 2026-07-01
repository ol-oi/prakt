// PasswordGenerator.h
#pragma once
#include <string>
#include <vector>
#include <cstdint>

class PasswordGenerator {
public:
    PasswordGenerator();
    
    std::string generatePassword(
        const std::string& login,
        const std::string& masterPassword,
        size_t length = 16
    );
    
private:
    static const std::string LOWERCASE;
    static const std::string UPPERCASE;
    static const std::string DIGITS;
    static const std::string SPECIAL;
    
    std::vector<uint8_t> hashPassword(const std::string& data);
    std::string hashToPassword(const std::vector<uint8_t>& hash, size_t length);
    uint32_t getRandomFromHash(const std::vector<uint8_t>& hash, size_t& index);
};
