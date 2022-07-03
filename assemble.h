#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define NORTYPE 5
#define NOITYPE 8
#define NOJTYPE 3
#define RTYPE 0
#define ITYPE 1
#define JTYPE 2
struct symbolTable
{
    int value;
    char *symbol;
};
struct instruction
{
    size_t instType;
    size_t intInst;
    char *mnemonic;
    char inst[9];
    int rs;
    int rt;
    int rd;
    int imm;
    int PC;
};
void error(char *);
bool isDecimal(char* );
int findSymTabLen(FILE *);
int fillSymTab(struct symbolTable *, FILE *);
void formInst(struct instruction *, FILE *);
int hex2int(char *);
void int2hex16(char *, int);
