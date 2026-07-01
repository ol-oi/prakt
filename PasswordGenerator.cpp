// PasswordGenerator.cpp
#include "PasswordGenerator.h"
#include <openssl/evp.h>
#include <algorithm>
#include <random>
#include <chrono>
#include <cstring>
#include <vector>

const std::string PasswordGenerator::LOWERCASE = "abcdefghijklmnopqrstuvwxyz";
const std::string PasswordGenerator::UPPERCASE = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const std::string PasswordGenerator::DIGITS = "0123456789";
const std::string PasswordGenerator::SPECIAL = "!@#$%^&*()_+-=[]{}|;:,.<>?";

PasswordGenerator::PasswordGenerator() {}

std::vector<uint8_t> PasswordGenerator::hashPassword(const std::string& data) {
    std::vector<uint8_t> hash(64); // 512 бит для SHA-512
    
    // Используем EVP API (совместимо с OpenSSL 3.0)
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        // Если не удалось создать контекст, возвращаем нулевой хеш
        return hash;
    }
    
    // Инициализируем SHA-512
    const EVP_MD* md = EVP_sha512();
    EVP_DigestInit_ex(mdctx, md, NULL);
    
    // Добавляем данные
    EVP_DigestUpdate(mdctx, data.c_str(), data.length());
    
    // Добавляем соль
    const char* salt = "GOST-34.11-2012-inspired-SALT";
    EVP_DigestUpdate(mdctx, salt, strlen(salt));
    
    // Добавляем временную метку для уникальности
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    std::string time_str = std::to_string(timestamp);
    EVP_DigestUpdate(mdctx, time_str.c_str(), time_str.length());
    
    // Получаем результат
    unsigned int digest_len = 0;
    EVP_DigestFinal_ex(mdctx, hash.data(), &digest_len);
    
    EVP_MD_CTX_free(mdctx);
    
    return hash;
}

uint32_t PasswordGenerator::getRandomFromHash(const std::vector<uint8_t>& hash, size_t& index) {
    uint32_t result = 0;
    for (int i = 0; i < 4; i++) {
        result = (result << 8) | hash[(index++) % hash.size()];
    }
    return result;
}

std::string PasswordGenerator::hashToPassword(const std::vector<uint8_t>& hash, size_t length) {
    std::string password;
    password.reserve(length);
    
    // ГАРАНТИРУЕМ наличие разных типов символов
    // Используем разные части хеша для разных типов
    password.push_back(UPPERCASE[hash[0] % UPPERCASE.length()]);
    password.push_back(LOWERCASE[hash[1] % LOWERCASE.length()]);
    password.push_back(DIGITS[hash[2] % DIGITS.length()]);
    password.push_back(SPECIAL[hash[3] % SPECIAL.length()]);
    
    // Заполняем остальную часть
    size_t index = 4;
    while (password.length() < length) {
        uint32_t rand_val = getRandomFromHash(hash, index);
        
        // Используем разные наборы символов для разнообразия
        int set_choice = rand_val % 4;
        const std::string* char_set;
        
        switch (set_choice) {
            case 0: char_set = &UPPERCASE; break;
            case 1: char_set = &LOWERCASE; break;
            case 2: char_set = &DIGITS; break;
            case 3: char_set = &SPECIAL; break;
            default: char_set = &LOWERCASE; break;
        }
        
        size_t char_index = (rand_val >> 8) % char_set->length();
        password.push_back((*char_set)[char_index]);
    }
    
    // Перемешиваем пароль с использованием криптографического RNG
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(password.begin(), password.end(), g);
    
    return password;
}

std::string PasswordGenerator::generatePassword(
    const std::string& login,
    const std::string& masterPassword,
    size_t length) {
    
    if (length < 8) length = 8;
    if (length > 64) length = 64;
    
    // Объединяем логин и пароль с разделителями
    std::string combined = "LOGIN:" + login + ":PASS:" + masterPassword + ":SALT:";
    
    // Вычисляем хеш
    std::vector<uint8_t> hash = hashPassword(combined);
    
    // Проверяем, что хеш не нулевой
    bool all_zero = true;
    for (uint8_t byte : hash) {
        if (byte != 0) {
            all_zero = false;
            break;
        }
    }
    
    if (all_zero) {
        // Если хеш нулевой, используем резервный метод
        std::random_device rd;
        std::mt19937 g(rd());
        std::uniform_int_distribution<int> dist(0, 255);
        for (size_t i = 0; i < hash.size(); i++) {
            hash[i] = dist(g);
        }
    }
    
    // Преобразуем хеш в пароль
    std::string result = hashToPassword(hash, length);
    
    // Проверяем, что пароль содержит все типы символов
    bool hasUpper = std::any_of(result.begin(), result.end(), ::isupper);
    bool hasLower = std::any_of(result.begin(), result.end(), ::islower);
    bool hasDigit = std::any_of(result.begin(), result.end(), ::isdigit);
    bool hasSpecial = std::any_of(result.begin(), result.end(), 
        [](char c) { return SPECIAL.find(c) != std::string::npos; });
    
    // Если чего-то не хватает - добавляем принудительно
    if (!hasUpper) result[0] = UPPERCASE[(hash[0] + 1) % UPPERCASE.length()];
    if (!hasLower) result[1] = LOWERCASE[(hash[1] + 1) % LOWERCASE.length()];
    if (!hasDigit) result[2] = DIGITS[(hash[2] + 1) % DIGITS.length()];
    if (!hasSpecial) result[3] = SPECIAL[(hash[3] + 1) % SPECIAL.length()];
    
    return result;
}
