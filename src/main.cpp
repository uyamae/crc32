#include <iostream>
#include <iomanip>
#include <cstdlib>

/// 8 ビットの反転
constexpr uint32_t reverse8(uint32_t bits) {
    return ((bits >> 7) & 0x01) |
           ((bits >> 5) & 0x02) |
           ((bits >> 3) & 0x04) |
           ((bits >> 1) & 0x08) |
           ((bits << 1) & 0x10) |
           ((bits << 3) & 0x20) |
           ((bits << 5) & 0x40) |
           ((bits << 7) & 0x80);
}
/// 16 ビットの反転
constexpr uint32_t reverse16(uint32_t bits) {
    return reverse8((bits >> 8) & 0xff) |
           (reverse8(bits & 0xff) << 8);
}
/// 32 ビットの反転
constexpr uint32_t reverse32(uint32_t bits) {
    return reverse16(bits >> 16 & 0xffff) |
           (reverse16(bits & 0xffff) << 16);
}

/// 多項式(反転)
constexpr uint32_t kPolynomial = 0x04c11db7;
constexpr uint32_t kPolynomialR = reverse32(kPolynomial);

/// 8 bit に対するmodulo 2 の計算
uint32_t modulo2(uint32_t value) {
    uint32_t crc = 0;
    for (uint32_t i = 0; i < 8; ++i) {
        if (value & (1 << i)) {
            value ^= kPolynomialR << (i + 1);
            crc ^= kPolynomialR >> (7 - i);
        }
    }
    return crc;
}

/// CRC32 テーブル
uint32_t table[256];

/// 手順を細分化したcrc 計算
uint32_t crc32(const uint8_t * data, size_t size, uint32_t crc) {
    crc ^= 0xffffffff;
    for (size_t i = 0; i < size; ++i) {
        auto lower8bit = crc & 0xff;
        auto index = data[i] ^ lower8bit;
        auto upper24bit = crc >> 8;
        auto value = table[index];
        auto xor = value ^ upper24bit;
        crc = xor;
    }
    return crc ^ 0xffffffff;
}
/// 手順をまとめたcrc 計算
uint32_t crc32c(const uint8_t * data, size_t size, uint32_t crc) {
    crc ^= 0xffffffff;
    for (size_t i = 0; i < size; ++i) {
        crc = table[data[i] ^ (crc & 0xff)] ^ (crc >> 8);
    }
    return crc ^ 0xffffffff;
}

int main(int, char**) {
    // テーブル作成
    for (uint32_t i = 0; i < 256; ++i) {
        table[i] = modulo2(i);
        std::cout << std::dec << std::setw(3) << std::setfill(' ') << std::right << i << ":"
                  << std::hex << std::setw(8) << std::setfill('0') << table[i] << std::endl;
    }
    // テーブルを利用したcrc32 計算
    const char * text = "a";
    size_t size = std::strlen(text);
    uint32_t crc = crc32(reinterpret_cast<const uint8_t *>(text), size, 0);
    std::cout << "data:" << text << std::endl;
    std::cout << "crc :" << std::hex << std::setw(8) << std::setfill('0') << crc << std::endl;

    crc = crc32c(reinterpret_cast<const uint8_t *>(text), size, 0);
    std::cout << "data:" << text << std::endl;
    std::cout << "crc :" << std::hex << std::setw(8) << std::setfill('0') << crc << std::endl;

    text = "abcd";
    size = std::strlen(text);
    crc = crc32c(reinterpret_cast<const uint8_t *>(text), size, 0);
    std::cout << "data:" << text << std::endl;
    std::cout << "crc :" << std::hex << std::setw(8) << std::setfill('0') << crc << std::endl;
    // テーブルの出力
    for (uint32_t i = 0; i < 16; ++i) {
        std::cout << "   ";
        for (uint32_t j = 0; j < 16; ++j) {
            std::cout << " 0x" << std::hex << std::setw(8) << std::setfill('0') << table[16 * i + j] << ",";
        }
        std::cout << std::endl;
    }

    return 0;
}
