// streebog.h
#pragma once
#include <cstdint>
#include <vector>
#include <string>

class Streebog {
public:
    static constexpr size_t DIGEST_SIZE = 64; // 512 бит
    
    Streebog();
    
    static std::vector<uint8_t> hash512(const std::vector<uint8_t>& data);
    static std::vector<uint8_t> hash512(const std::string& data);
    
private:
    struct Context {
        uint64_t h[8];
        uint64_t N[8];
        uint64_t Sigma[8];
        size_t fill;
        uint8_t buffer[64];
        uint64_t length[8];
    };
    
    static const uint64_t C[12][8];
    static const uint8_t Pi[256];
    static const uint8_t Tau[64];
    
    static void init(Context* ctx);
    static void update(Context* ctx, const uint8_t* data, size_t len);
    static void finalize(Context* ctx, uint8_t* digest);
    static void process_block(Context* ctx, const uint8_t* data);
    
    static void transform(uint64_t* state, const uint64_t* key);
    static void key_schedule(uint64_t* key, const uint64_t* iter);
    static void g_function(uint64_t* h, const uint64_t* N, const uint64_t* m);
    static void add_mod512(uint64_t* a, const uint64_t* b);
    static void xor_blocks(uint64_t* a, const uint64_t* b);
    static void sbox(uint8_t* data);
    static void permutation(uint8_t* data);
};
