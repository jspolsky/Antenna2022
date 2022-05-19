#include <Arduino.h>
#include "Util.h"

// better debugging. Inspired from https://gist.github.com/asheeshr/9004783 with some modifications

namespace Util
{

    void setup(void)
    {
#ifdef DEBUG_SC
        uint32_t tmEnd = millis() + 5000;
        Serial.begin(250000);
        while (!Serial && millis() < tmEnd)
            ;
#endif
    }

    uint32_t FreeMem()
    {
        uint32_t stackTop;
        uint32_t heapTop;

        // current position of the stack.
        stackTop = (uint32_t)&stackTop;

        // current position of heap.
        void *hTop = malloc(1);
        heapTop = (uint32_t)hTop;
        free(hTop);

        // The difference is (approximately) the free, available ram.
        return stackTop - heapTop;
    }
} // namespace Util

void binaryPrint( char* rgchDest, uint32_t i )
{
    for (uint32_t mask = 0x8000; mask; mask >>= 1)
    {
        *rgchDest++ = i & mask ? '1' : '0';
    }
    *rgchDest = '\0';
}

void dbgprintf(char const *pszFmt UNUSED_IN_RELEASE, ...)
{

#ifdef DEBUG_SC
    char const *pszTmp;
    char rgchTmp[33];

    va_list argv;
    va_start(argv, pszFmt);

    pszTmp = pszFmt;
    while (*pszTmp)
    {
        if (*pszTmp == '%')
        {
            pszTmp++;
            switch (*pszTmp)
            {
            case 'd':
                Serial.print(va_arg(argv, int));
                break;

            case 'x':
            case 'X':
                Serial.print(va_arg(argv, uint32_t), HEX);
                break;

            case 'b':
                binaryPrint(rgchTmp, va_arg(argv, uint32_t));
                Serial.print(rgchTmp);
                break;

            case 'l':
                Serial.print(va_arg(argv, long));
                break;

            case 'u':
                Serial.print(va_arg(argv, unsigned long));
                break;

            case 'f':
                Serial.print(va_arg(argv, double));
                break;

            case 'F':
                Serial.print(va_arg(argv, double), 8);
                break;

            case 'c':
                Serial.print((char)va_arg(argv, int));
                break;

            case 's':
                Serial.print(va_arg(argv, char *));
                break;

            case '%':
                Serial.print('%');
                break;

            default:
                break;
            }
        }
        else if (*pszTmp == '\n')
        {
            Serial.println();
        }
        else
        {
            Serial.print(*pszTmp);
        }

        pszTmp++;
    }
#endif
}
