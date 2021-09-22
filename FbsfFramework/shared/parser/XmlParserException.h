/*
 * Copyright QTronic GmbH. All rights reserved.
 */

/* ---------------------------------------------------------------------------*
 * XmlParserException.h
 * Exception used in parsing model description of a FMI 2.0 model.
 *
 * Author: Adrian Tirea
 * ---------------------------------------------------------------------------*/

#ifndef XML_PARSER_EXCEPTION_H
#define XML_PARSER_EXCEPTION_H

#include <exception>
#include <stdarg.h>
#include <stdio.h>

#define ERROR_INFO  "[Info]"
#define ERROR_ERROR "[Error]"
#define ERROR_FATAL "[Fatal]"

static char* strallocprintf(const char *format, va_list argp);

// message passed in constructor is freed in destructor.
class XmlParserException : public std::exception {
 public:
    char *message;

 public:
    XmlParserException(const char *format, ...) {
    va_list argp;
    va_start(argp, format);
    message = strallocprintf(format, argp);
    va_end(argp);
    }
    ~XmlParserException() {
        if (message) delete[]message;
    }

    virtual const char *what() const noexcept {
        return message;
    }
};

static char *strallocprintf(const char *format, va_list argp) {
    int n;
    char *result;
    // HLA n = _vscprintf(format, argp);
    va_list argcopy;
    va_copy(argcopy, argp);
    n = vsnprintf(NULL, 0, format, argcopy);
    va_end(argcopy);

    result = new char[n + 1];
    vsprintf(result, format, argp);
    return result;
}

#endif  // XML_PARSER_EXCEPTION_H
