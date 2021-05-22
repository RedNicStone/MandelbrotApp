#include "app_utility.h"

void copyStringToBuffer(const std::string& string, char* buffer, unsigned int size) {
    int index = string.copy(buffer, size - 1);
    buffer[index] = '\0';
}

unsigned long hashDjb2(const std::string& str) {
    unsigned long hash = 5381;
    for (auto c : str) {
        hash = (hash << 5) + hash + c; /* hash * 33 + c */
    }
    return hash;
}
