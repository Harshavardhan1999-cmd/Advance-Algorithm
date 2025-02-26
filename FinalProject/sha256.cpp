#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdint>
#include <algorithm>
#include <memory>
#include <limits>

class SecureHash256 {
private:
    using HashWord = uint32_t;
    
    // Initial hash values (first 32 bits of the fractional parts of the square roots of the first 8 primes 2..19)
    const HashWord initialStates[8] = {
        0x6a09e667u, 0xbb67ae85u, 0x3c6ef372u, 0xa54ff53au,
        0x510e527fu, 0x9b05688cu, 0x1f83d9abu, 0x5be0cd19u
    };

    // Round constants (first 32 bits of the fractional parts of the cube roots of the first 64 primes 2..311)
    const HashWord roundConstants[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    // Core operations
    inline HashWord CHOOSE(HashWord x, HashWord y, HashWord z) const {
        return (x & y) ^ (~x & z);
    }
    
    inline HashWord MAJORITY(HashWord x, HashWord y, HashWord z) const {
        return (x & y) ^ (x & z) ^ (y & z);
    }
    
    inline HashWord ROTATE_RIGHT(HashWord x, unsigned int n) const {
        return (x >> n) | (x << (32 - n));
    }
    
    inline HashWord SIGMA0(HashWord x) const {
        return ROTATE_RIGHT(x, 2) ^ ROTATE_RIGHT(x, 13) ^ ROTATE_RIGHT(x, 22);
    }
    
    inline HashWord SIGMA1(HashWord x) const {
        return ROTATE_RIGHT(x, 6) ^ ROTATE_RIGHT(x, 11) ^ ROTATE_RIGHT(x, 25);
    }
    
    inline HashWord LOWERCASE_SIGMA0(HashWord x) const {
        return ROTATE_RIGHT(x, 7) ^ ROTATE_RIGHT(x, 18) ^ (x >> 3);
    }
    
    inline HashWord LOWERCASE_SIGMA1(HashWord x) const {
        return ROTATE_RIGHT(x, 17) ^ ROTATE_RIGHT(x, 19) ^ (x >> 10);
    }

    // Constants
    static const size_t BLOCK_SEQUENCE_LEN = 16;
    static const size_t HASH_ARRAY_LEN = 8;
    static const size_t SCHEDULE_ARRAY_LEN = 64;
    static const size_t BLOCK_SIZE = 512;
    static const size_t WORD_SIZE = 4; // 32 bits = 4 bytes

    // Prepare message blocks from input data
    std::vector<std::vector<HashWord>> prepareMessage(const unsigned char* input, size_t inputLength) {
        // Calculate required padding
        size_t bitLength = inputLength * 8;
        size_t paddingBits = (448 - 1 - bitLength) % 512;
        if (paddingBits < 0) paddingBits += 512; // Ensure positive value
        
        // Calculate total number of blocks needed
        size_t blockCount = (bitLength + 1 + paddingBits + 64) / 512;
        
        // Create blocks with exact required size
        std::vector<std::vector<HashWord>> blocks(blockCount, std::vector<HashWord>(BLOCK_SEQUENCE_LEN, 0));
        
        // Fill blocks with message data
        size_t byteIndex = 0;
        for (size_t blockIdx = 0; blockIdx < blockCount; blockIdx++) {
            for (size_t wordIdx = 0; wordIdx < BLOCK_SEQUENCE_LEN; wordIdx++) {
                HashWord word = 0;
                for (size_t byteOffset = 0; byteOffset < 4; byteOffset++) {
                    // Current byte position in the overall message
                    size_t pos = blockIdx * 64 + wordIdx * 4 + byteOffset;
                    
                    if (pos < inputLength) {
                        // Normal byte from input
                        word = (word << 8) | input[pos];
                    } else if (pos == inputLength) {
                        // Padding begins with a bit 1
                        word = (word << 8) | 0x80;
                    } else {
                        // Continue with padding bits (zeros)
                        word = (word << 8);
                    }
                }
                blocks[blockIdx][wordIdx] = word;
            }
        }
        
        // Append original message length as 64-bit big-endian integer
        // For messages < 2^32 bits, we only need to set the low-order word
        if (bitLength < 0x100000000) {
            blocks[blockCount-1][14] = 0;
            blocks[blockCount-1][15] = static_cast<HashWord>(bitLength);
        } else {
            // Handle larger messages (rarely needed for typical use)
            uint64_t length64 = bitLength;
            blocks[blockCount-1][14] = static_cast<HashWord>(length64 >> 32);
            blocks[blockCount-1][15] = static_cast<HashWord>(length64 & 0xFFFFFFFF);
        }
        
        return blocks;
    }

    // Process all message blocks and compute the hash value
    void processBlocks(const std::vector<std::vector<HashWord>>& blocks, HashWord* hashValues) {
        // Initialize hash values with initial state
        std::memcpy(hashValues, initialStates, HASH_ARRAY_LEN * sizeof(HashWord));
        
        // Temporary variables for calculations
        HashWord a, b, c, d, e, f, g, h, temp1, temp2;
        HashWord schedule[SCHEDULE_ARRAY_LEN];
        
        // Process each message block
        for (const auto& block : blocks) {
            // Copy the first 16 words from the block to the schedule
            std::copy(block.begin(), block.end(), schedule);
            
            // Extend the schedule to 64 words
            for (size_t t = 16; t < SCHEDULE_ARRAY_LEN; t++) {
                schedule[t] = schedule[t-16] + 
                              LOWERCASE_SIGMA0(schedule[t-15]) + 
                              schedule[t-7] + 
                              LOWERCASE_SIGMA1(schedule[t-2]);
            }
            
            // Initialize working variables with current hash value
            a = hashValues[0];
            b = hashValues[1];
            c = hashValues[2];
            d = hashValues[3];
            e = hashValues[4];
            f = hashValues[5];
            g = hashValues[6];
            h = hashValues[7];
            
            // Main compression loop
            for (size_t t = 0; t < SCHEDULE_ARRAY_LEN; t++) {
                temp1 = h + SIGMA1(e) + CHOOSE(e, f, g) + roundConstants[t] + schedule[t];
                temp2 = SIGMA0(a) + MAJORITY(a, b, c);
                
                h = g;
                g = f;
                f = e;
                e = d + temp1;
                d = c;
                c = b;
                b = a;
                a = temp1 + temp2;
            }
            
            // Update hash values for this block
            hashValues[0] += a;
            hashValues[1] += b;
            hashValues[2] += c;
            hashValues[3] += d;
            hashValues[4] += e;
            hashValues[5] += f;
            hashValues[6] += g;
            hashValues[7] += h;
        }
    }

    // Generate final digest string from hash values
    std::string generateDigest(const HashWord* hashValues) {
        std::stringstream ss;
        for (size_t i = 0; i < HASH_ARRAY_LEN; i++) {
            ss << std::hex << std::setw(8) << std::setfill('0') << hashValues[i];
        }
        return ss.str();
    }

public:
    // Calculate hash for a string input
    std::string calculateHash(const std::string& input) {
        return calculateHash(reinterpret_cast<const unsigned char*>(input.data()), input.length());
    }
    
    // Calculate hash for binary data with specific length
    std::string calculateHash(const unsigned char* input, size_t length) {
        if (!input && length > 0) {
            throw std::invalid_argument("Invalid input: null pointer with non-zero length");
        }
        
        // Prepare message blocks
        auto blocks = prepareMessage(input, length);
        
        // Process blocks and generate hash
        HashWord hashValues[HASH_ARRAY_LEN];
        processBlocks(blocks, hashValues);
        
        // Convert hash to string and return
        return generateDigest(hashValues);
    }
    
    // Calculate hash for file contents
    std::string calculateFileHash(const std::string& filePath) {
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filePath);
        }
        
        // Read file contents
        std::vector<unsigned char> buffer(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>()
        );
        
        file.close();
        
        // Calculate hash for file contents
        return calculateHash(buffer.data(), buffer.size());
    }
    
    // Utility function for direct string hashing
    static std::string hash(const std::string& input) {
        SecureHash256 hasher;
        return hasher.calculateHash(input);
    }
    
    // Utility function for direct file hashing
    static std::string hashFile(const std::string& filePath) {
        SecureHash256 hasher;
        return hasher.calculateFileHash(filePath);
    }
};

// Application class to separate business logic from UI
class HashApplication {
private:
    SecureHash256 hasher;
    
    // Helper method to handle string input
    void processStringInput() {
        std::cout << "Enter string to hash: ";
        std::string input;
        std::cin.ignore(); // Clear newline character
        std::getline(std::cin, input);
        
        try {
            std::string hash = hasher.calculateHash(input);
            std::cout << "Hash: " << hash << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
    
    // Helper method to handle file input
    void processFileInput() {
        std::cout << "Enter file path: ";
        std::string filePath;
        std::cin >> filePath;
        
        try {
            std::string hash = hasher.calculateFileHash(filePath);
            std::cout << "Hash: " << hash << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
    
public:
    void run() {
        int option;
        do {
            std::cout << "\n=== SHA-256 Hash Generator ===\n";
            std::cout << "1. Hash a string\n";
            std::cout << "2. Hash a file\n";
            std::cout << "3. Exit\n";
            std::cout << "Choose an option: ";
            
            // Validate input
            if (!(std::cin >> option)) {
                std::cin.clear(); // Clear error flag
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
                std::cerr << "Invalid input. Please enter a number.\n";
                continue;
            }
            
            switch (option) {
                case 1:
                    processStringInput();
                    break;
                case 2:
                    processFileInput();
                    break;
                case 3:
                    std::cout << "Exiting...\n";
                    break;
                default:
                    std::cout << "Invalid option. Please try again.\n";
                    break;
            }
        } while (option != 3);
    }
};

int main() {
    try {
        HashApplication app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
