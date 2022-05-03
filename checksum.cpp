#include <stdio.h>
#include <iostream>

using namespace std; 

int checksum(const char *s) {
    int c = 0;

    while (*s) {
        c ^= *s++;
    }

    return c;
}

int main()
{
    char s[] = "BNALF,1,1,0,,B,W,V,,220003,,0,0,VISUAL ALARM";
    int c = checksum(s);
    cout <<  hex << uppercase << c << endl;
    return 0;
}