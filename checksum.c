#include <stdio.h>

int checksum(const char *s) {
    int c = 0;

    while (*s) {
        c ^= *s++;
    }

    return c;
}

int main()
{
    char mystring[] = "BNALF,1,1,0,,B,W,V,,220003,,0,0,VISUAL ALARM";

    printf("String: %s\nChecksum: 0x%02X\n Checksum nor: %d", mystring, checksum(mystring), checksum(mystring) & 0xFF);

    return 0;
}