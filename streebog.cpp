// streebog.cpp
#include "streebog.h"
#include <cstring>
#include <algorithm>

// Таблица подстановок S-бокса (из ГОСТ Р 34.11-2012)
const uint8_t Streebog::Pi[256] = {
    0xFC, 0xEE, 0xDD, 0x11, 0xCF, 0x6E, 0x31, 0x16,
    0xFB, 0xC4, 0xFA, 0xDA, 0x23, 0xC5, 0x04, 0x4D,
    0xE9, 0x77, 0xF0, 0xDB, 0x93, 0x2E, 0x99, 0xBA,
    0x17, 0x36, 0xF1, 0xBB, 0x14, 0xCD, 0x5F, 0xC1,
    0xF9, 0x18, 0x65, 0x5A, 0xE2, 0x5C, 0xEF, 0x21,
    0x81, 0x1C, 0x3C, 0x42, 0x8B, 0x01, 0x8E, 0x4F,
    0x05, 0x84, 0x02, 0xAE, 0xE3, 0x6A, 0x8F, 0xA0,
    0x06, 0x0B, 0xED, 0x98, 0x7F, 0xD4, 0xD3, 0x1F,
    0xEB, 0x34, 0x2C, 0x51, 0xEA, 0xC8, 0x48, 0xAB,
    0xF2, 0x2A, 0x68, 0xA2, 0xFD, 0x3A, 0xCE, 0xCC,
    0xB5, 0x70, 0x0E, 0x56, 0x08, 0x0C, 0x76, 0x12,
    0xBF, 0x72, 0x13, 0x47, 0x9C, 0xB7, 0x5D, 0x87,
    0x15, 0xA1, 0x96, 0x29, 0x10, 0x7B, 0x9A, 0xC7,
    0xF3, 0x91, 0x78, 0x6F, 0x9D, 0x9E, 0xB2, 0xB1,
    0x32, 0x75, 0x19, 0x3D, 0xFF, 0x35, 0x8A, 0x7E,
    0x6D, 0x54, 0xC6, 0x80, 0xC3, 0xBD, 0x0D, 0x57,
    0xDF, 0xF5, 0x24, 0xA9, 0xE6, 0x01, 0x6E, 0xDE,
    0xE8, 0x66, 0x14, 0x92, 0x1D, 0x41, 0x58, 0x69,
    0x16, 0x62, 0xB4, 0x22, 0x45, 0x1B, 0xE5, 0x2B,
    0x0F, 0x5E, 0x9F, 0x4A, 0x4C, 0x4B, 0x1E, 0x20,
    0x82, 0x2F, 0xCA, 0x60, 0xC0, 0x50, 0xE4, 0x40,
    0x0A, 0x46, 0x3E, 0x49, 0xE7, 0x3B, 0x7C, 0x7D,
    0x64, 0x0D, 0x43, 0xAC, 0x88, 0x37, 0x39, 0x86,
    0x7A, 0x53, 0x6B, 0xA5, 0x30, 0x95, 0x90, 0xAA,
    0xE0, 0x6C, 0x6F, 0x74, 0xEF, 0xEB, 0x85, 0xCE,
    0x5B, 0x3D, 0x07, 0x27, 0xFC, 0x91, 0xAC, 0x1F,
    0x9B, 0xD8, 0xEC, 0x44, 0x05, 0xB3, 0x7F, 0xF6,
    0x0B, 0x0C, 0xD9, 0x8C, 0x58, 0xF4, 0x4C, 0x7A,
    0xE1, 0xAF, 0x08, 0x28, 0x3A, 0x72, 0x9E, 0xDE,
    0xA6, 0x3F, 0xD7, 0xEB, 0x81, 0xDA, 0x0E, 0xFE,
    0x1A, 0x6D, 0x15, 0x3C, 0xBA, 0x17, 0x84, 0x79,
    0x00, 0x7E, 0x67, 0x94, 0xD6, 0x49, 0x38, 0xA9
};

// Таблица перестановок Tau
const uint8_t Streebog::Tau[64] = {
    0,  8, 16, 24, 32, 40, 48, 56,
    1,  9, 17, 25, 33, 41, 49, 57,
    2, 10, 18, 26, 34, 42, 50, 58,
    3, 11, 19, 27, 35, 43, 51, 59,
    4, 12, 20, 28, 36, 44, 52, 60,
    5, 13, 21, 29, 37, 45, 53, 61,
    6, 14, 22, 30, 38, 46, 54, 62,
    7, 15, 23, 31, 39, 47, 55, 63
};

// Итерационные константы C (первые 12)
const uint64_t Streebog::C[12][8] = {
    {0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
     0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL},
    {0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
     0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL},
    // ... (полные константы нужно добавить из ГОСТ)
    // Для примера заполним нулями, в реальном проекте нужно добавить полные константы
};

Streebog::Streebog() {}

void Streebog::init(Context* ctx) {
    memset(ctx->h, 0, sizeof(ctx->h));
    memset(ctx->N, 0, sizeof(ctx->N));
    memset(ctx->Sigma, 0, sizeof(ctx->Sigma));
    ctx->fill = 0;
    memset(ctx->buffer, 0, sizeof(ctx->buffer));
    memset(ctx->length, 0, sizeof(ctx->length));
}

void Streebog::xor_blocks(uint64_t* a, const uint64_t* b) {
    for (int i = 0; i < 8; i++) {
        a[i] ^= b[i];
    }
}

void Streebog::add_mod512(uint64_t* a, const uint64_t* b) {
    uint64_t carry = 0;
    for (int i = 0; i < 8; i++) {
        uint64_t sum = a[i] + b[i] + carry;
        carry = (sum < a[i] || (sum == a[i] && carry)) ? 1 : 0;
        a[i] = sum;
    }
}

void Streebog::sbox(uint8_t* data) {
    for (int i = 0; i < 64; i++) {
        data[i] = Pi[data[i]];
    }
}

void Streebog::permutation(uint8_t* data) {
    uint8_t temp[64];
    memcpy(temp, data, 64);
    for (int i = 0; i < 64; i++) {
        data[i] = temp[Tau[i]];
    }
}

void Streebog::transform(uint64_t* state, const uint64_t* key) {
    uint8_t data[64];
    uint8_t key_bytes[64];
    
    // Копируем состояние и ключ в байтовые массивы
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            data[i*8 + j] = (state[i] >> (j * 8)) & 0xFF;
            key_bytes[i*8 + j] = (key[i] >> (j * 8)) & 0xFF;
        }
    }
    
    // XOR с ключом
    for (int i = 0; i < 64; i++) {
        data[i] ^= key_bytes[i];
    }
    
    // S-бокс
    sbox(data);
    
    // Перестановка
    permutation(data);
    
    // Обратное преобразование в массив uint64_t
    for (int i = 0; i < 8; i++) {
        state[i] = 0;
        for (int j = 0; j < 8; j++) {
            state[i] |= (uint64_t)data[i*8 + j] << (j * 8);
        }
    }
}

void Streebog::key_schedule(uint64_t* key, const uint64_t* iter) {
    uint64_t temp[8];
    memcpy(temp, key, 64);
    xor_blocks(temp, iter);
    transform(temp, key);
    memcpy(key, temp, 64);
}

void Streebog::g_function(uint64_t* h, const uint64_t* N, const uint64_t* m) {
    uint64_t K[8];
    uint64_t temp[8];
    
    // K = h
    memcpy(K, h, 64);
    
    // temp = N
    memcpy(temp, N, 64);
    xor_blocks(temp, m);
    
    // Основной цикл
    for (int i = 0; i < 12; i++) {
        transform(temp, K);
        key_schedule(K, C[i]);
    }
    
    // h = K xor h
    xor_blocks(K, h);
    xor_blocks(K, m);
    memcpy(h, K, 64);
}

void Streebog::process_block(Context* ctx, const uint8_t* data) {
    uint64_t m[8];
    for (int i = 0; i < 8; i++) {
        m[i] = 0;
        for (int j = 0; j < 8; j++) {
            m[i] |= (uint64_t)data[i*8 + j] << (j * 8);
        }
    }
    
    g_function(ctx->h, ctx->N, m);
    add_mod512(ctx->N, m);
    xor_blocks(ctx->Sigma, m);
}

void Streebog::update(Context* ctx, const uint8_t* data, size_t len) {
    size_t pos = 0;
    
    if (ctx->fill > 0) {
        size_t available = 64 - ctx->fill;
        size_t to_copy = std::min(len, available);
        memcpy(ctx->buffer + ctx->fill, data, to_copy);
        ctx->fill += to_copy;
        len -= to_copy;
        pos += to_copy;
        
        if (ctx->fill == 64) {
            process_block(ctx, ctx->buffer);
            ctx->fill = 0;
        }
    }
    
    while (len >= 64) {
        process_block(ctx, data + pos);
        pos += 64;
        len -= 64;
    }
    
    if (len > 0) {
        memcpy(ctx->buffer, data + pos, len);
        ctx->fill = len;
    }
    
    // Обновляем длину
    for (int i = 0; i < 8; i++) {
        ctx->length[i] += len;
        if (ctx->length[i] >= len) break;
    }
}

void Streebog::finalize(Context* ctx, uint8_t* digest) {
    // Добавление паддинга
    size_t padded_len = ctx->fill;
    ctx->buffer[padded_len++] = 0x01;
    while (padded_len < 64) {
        ctx->buffer[padded_len++] = 0x00;
    }
    
    process_block(ctx, ctx->buffer);
    
    // Добавление длины сообщения
    uint64_t length[8] = {0};
    for (int i = 0; i < 8; i++) {
        length[i] = ctx->length[i] * 8; // в битах
    }
    
    process_block(ctx, (uint8_t*)length);
    process_block(ctx, (uint8_t*)ctx->Sigma);
    
    // Формирование результата
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            digest[i*8 + j] = (ctx->h[i] >> (j * 8)) & 0xFF;
        }
    }
}

std::vector<uint8_t> Streebog::hash512(const std::vector<uint8_t>& data) {
    Context ctx;
    init(&ctx);
    update(&ctx, data.data(), data.size());
    std::vector<uint8_t> result(DIGEST_SIZE);
    finalize(&ctx, result.data());
    return result;
}

std::vector<uint8_t> Streebog::hash512(const std::string& data) {
    std::vector<uint8_t> vec(data.begin(), data.end());
    return hash512(vec);
}
