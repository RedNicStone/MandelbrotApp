#pragma once
#ifndef MANDELBROT_APPUTILITY_INCLUDED
#define MANDELBROT_APPUTILITY_INCLUDED

#include <string>

/**
 * Copies a string to a buffer
 * 
 * @param string String to copy from
 * @param buffer `char*` buffer to copy to, will be null-terminated
 * @param size The maximum number of characters to copy (including null-terminating character) (= max size of the buffer)
 */
void copyStringToBuffer(const std::string& string, char* buffer, unsigned int size);

/**
 * Hash algorithm for strings (djb2)
 */
unsigned long hashDjb2(const std::string& str);

#endif
