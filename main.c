
/* RC4 Cracker - Tom D */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define sSIZE 256


int mod(int a, int b) {
    while( (a < 0) || (a >= b) ) {
        if(a < 0)
            a += b;
        if(a >= b)
            a -= b;
    }
    return a;
}


int simResolve(unsigned char * buffer, unsigned char * key, int keyByte) {

    unsigned char tempK;
    unsigned char iv[3];
    unsigned char ivkey[8];
    unsigned char s[sSIZE], tempS;

    int i, j, tempJ, sInverse;
    unsigned char tmp;

    for(int c = 0; c < 4; c++) {
        iv[c] = buffer[c];
    }

    for(int c = 0; c < sSIZE; c++) {
        s[c] = c;
    }

    memset(ivkey, 0, 8);
    ivkey[0] = iv[0];
    ivkey[1] = iv[1];
    ivkey[2] = iv[2];
    j = 0;
    ivkey[3] = key[0];
    ivkey[4] = key[1];
    ivkey[5] = key[2];
    ivkey[6] = key[3];
    ivkey[7] = key[4];

    /*printf("Buffer: %.2x %.2x %.2x %.2x %.2x\n\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);
    printf("\nivkey: ");
    for(int c = 0; c < 8; c++) {
        printf("%x ", ivkey[c]);
    }
    printf("\n\n");
    for(int c = 0; c < sSIZE; c++) {
        printf("%x ", s[c]);
    }
    printf("  i = %x, j = %x", i, j);
    printf("\n\n");                   //*/


    for(i = 0; i < keyByte+3; i++) {
        /*tempJ = j;
        tempS = s[i];
        tempK = (int)ivkey[mod(i, 8)];  //*/

        j = mod(j + s[i] + (int)ivkey[mod((unsigned int)i,8)], sSIZE);

        tmp = s[j];
        s[j] = s[i];
        s[i] = tmp;

        /*for(int c = 0; c < sSIZE; c++) {
            printf("%x ", s[c]);
        }
        printf("  i = %x, j = %x + %x + %x = %x", i, tempJ, tempS, tempK, j);
        printf("\n"); //*/
    }

    sInverse = 0;
    for(int c = 0; c < sSIZE; c++) {
        if(s[c] == (buffer[4] ^ 'a') )
            sInverse = c;
    }
    //printf("\nsInverse = %x\n", sInverse);

    int guess = mod((sInverse - j - s[keyByte + 3]), sSIZE);
    //printf("BYTE: %.2x\n\n", guess);
    return (guess);
    printf("SK[%x] = %x - %x - %x = %x", keyByte, sInverse, j, s[i+1], key[keyByte]);

}


int main(void) {
    printf("\n~ ~ ~ ~ ~ RC4 Key Cracker v1.04 ~ ~ ~ ~ ~\n\n");

    FILE *fp;
    int keyByte = 0;
    int counts[sSIZE];
    unsigned char buffer[5], key[5];
    int bestGuess, maxGuess;

    for(int c = 0; c < 5; c++) {
        key[c] = 0;
    }

    for (keyByte = 0; keyByte < 5; keyByte++) {

        for(int c = 0; c < sSIZE; c++) {
            counts[c] = 0;
        }

        fp = fopen("traffic.rcf", "r");
        if (fp == NULL) {
            printf("\n\nError opening file");
            exit(1);
        }
        while (fread(buffer, sizeof(unsigned char), 5, fp) != 0) {

            //printf("Buffer: %.2x %.2x %.2x %.2x %.2x\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);
            if( (buffer[0] == keyByte+3) && (buffer[1] == 0xff) )
                counts[simResolve(&buffer[0], &key[0], keyByte)]++;
        }

        /*printf("\n\ncounts: ");
        for(int c = 0; c < sSIZE; c++) {
            printf("%d ", counts[c]);
        }
        exit(0);*/

        maxGuess = bestGuess = 0;
        for(int c = 0; c < sSIZE; c++) {
            if(counts[c] > maxGuess) {
                maxGuess = counts[c];
                bestGuess = c;
            }
        }
        key[keyByte] = bestGuess;
        printf("Best guess for key[%d] is 0x%.2x = %d = %c\n", keyByte, bestGuess, bestGuess, bestGuess);
        fclose(fp);
    }
    printf("\nRC4 Key: %c%c%c%c%c\n\n", key[0],key[1],key[2],key[3],key[4]);
}