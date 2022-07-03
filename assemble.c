#include "assemble.h"

void main(int argc, char **argv)
{
    FILE *assp, *machp, *fopen();
    struct symbolTable *pSymTab;
    int symTabLen;
    int i = 0, j = 0, found, noInsts;
    struct instruction *currInst = (struct instruction *)malloc(sizeof(struct instruction));
    int lineSize = 100;
    char line[50];
    char *token, *token2;
    char *instructions[]={"add", "sub", "slt", "or", "nand"
                        , "addi" , "slti", "ori", "lui", "lw"
                        , "sw" , "beq", "jalr", "j", "halt"};

    int instCnt = 0;
    char hexTable[16] = {'0', '1', '2', '3', '4', '5', '6', '7'
                        , '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    char lower[5];

    if(argc<3)
    {
        printf("***** Please run this program as follows: \n"); 
        printf("***** %s assprog.as machprog.m\n",argv[0]); 
        printf("***** where assprog.as is your assembly program\n"); 
        printf("***** and machprog.m will be your machine code.\n"); 
        exit(1);
    }

    if((assp=fopen(argv[1],"r")) == NULL) 
    {
        printf("%s cannot be opened\n", argv[1]); 
        exit(1);
    }
    if((machp=fopen(argv[2],"w+")) == NULL) 
    {
        printf("%s cannot be opened\n", argv[2]); 
        exit(1);
    }

    symTabLen = findSymTabLen(assp);
    pSymTab = (struct symbolTable *)malloc(symTabLen*sizeof(struct symbolTable));
    for(i=0; i <symTabLen; i++)
        pSymTab[i].symbol = (char*)malloc(7);
    noInsts=fillSymTab(pSymTab, assp);
    for ( i = 0; i < symTabLen - 1; i++)
        {
            for ( j = i+1; j < symTabLen; j++)
            {
                 if(strcmp(pSymTab[i].symbol, pSymTab[j].symbol) == 0)
                {
                    error("Multiple use of single lable");
                }
            }

        }
    currInst->mnemonic =(char *)malloc(7);
    long result;
    char *temp = (char *)malloc(9);
    while(fgets(line, lineSize, assp))
    {
        currInst->PC = instCnt;
        for ( i = 0; i < 8; i++)
        {
            currInst->inst[i] = '0';
        }

        instCnt++;
        found = 0;
        token = strtok(line, "\t, \n");
        for(i=0;i<symTabLen; i++)
        {
            if(strcmp(pSymTab[i].symbol, token) == 0)
                token = strtok(NULL,"\t, \n");
        }

        strcpy(currInst->mnemonic,token);
        currInst->instType = -1;

        if(strcmp(currInst->mnemonic, ".fill") == 0)
        {
            token = strtok(NULL, "\t, \n");
            if(isDecimal(token))
            {
                fprintf(machp, "%i\n", atoi(token));
            }
            else
            {
                for ( i = 0; i < symTabLen; i++)
                {
                    if(strcmp(pSymTab[i].symbol, token) == 0)
                    {
                        currInst->imm = pSymTab[i].value;
                        break;
                    }
                }
                if(symTabLen == i)
                {
                    error("Invalid Lable");
                }
                int2hex16(temp, currInst->imm);
                currInst->inst[0] = '0';
                currInst->inst[1] = '0';
                currInst->inst[2] = '0';
                currInst->inst[3] = '0';
                currInst->inst[4] = *(temp+0);
                currInst->inst[5] = *(temp+1);
                currInst->inst[6] = *(temp+2);
                currInst->inst[7] = *(temp+3);
                result = hex2int(currInst->inst);
                fprintf(machp, "%i\n", result);
            }
            continue;
        }

        for (i = 0; i < 15; i++)
        {
            if (strcmp(currInst->mnemonic, instructions[i]) == 0)
            {
                currInst->intInst = i;
                break;
            }
        }
        if(i == 15)
        {
            error("Invalid opcode");
        }

        if(i < 5) //R-Type
        {
            currInst->rd = atoi(strtok(NULL, "\t, \n"));
            currInst->rs = atoi(strtok(NULL, "\t, \n"));
            currInst->rt = atoi(strtok(NULL, "\t, \n"));
            currInst->instType = RTYPE;
        }
        else if (i < 13) //I-Type
        {
            currInst->rd = 0;
            currInst->rt = atoi(strtok(NULL, "\t, \n"));
            currInst->instType = ITYPE;

            if(strcmp(token, "lui") == 0 )
            {
                currInst->rs = 0;
            }
            else
                currInst->rs = atoi(strtok(NULL, "\t, \n"));

            token2 = strtok(NULL, "\t, \n");

            if(strcmp(token, "jalr") == 0 )
            {
                currInst->imm = 0;
            }
            else if (isDecimal(token2))
            {
                currInst->imm = atoi(token2);
            }
            else
            {
                for ( i = 0; i < symTabLen; i++)
                {
                    if(strcmp(pSymTab[i].symbol, token2) == 0)
                    {
                        currInst->imm = pSymTab[i].value;
                        break;
                    }
                }
                if(symTabLen == i)
                {
                    error("Invalid Lable");
                }
            }


            if (strcmp(token,"beq") == 0)
            {
                int temp;
                currInst->imm = currInst->imm - currInst->PC - 1;
                temp = currInst->rs;
                currInst->rs = currInst->rt;
                currInst->rt = temp;
            }
        }
        else //J-Type
        {
            currInst->instType = JTYPE;
            currInst->rd = 0;
            currInst->rt = 0;
            currInst->rs = 0;
            if(strcmp(token, "j") == 0 )
            {
                token = strtok(NULL, "\t, \n");
                for ( i = 0; i < symTabLen; i++)
                {
                    if(strcmp(pSymTab[i].symbol, token) == 0)
                    {
                        currInst->imm = pSymTab[i].value;
                        break;
                    }
                }
                if(symTabLen == i)
                {
                    error("Invalid Lable");
                }
            }
            else
                currInst->imm = 0;
        }

        currInst->inst[1] = hexTable[currInst->intInst];

        if(currInst->imm > 32767 || currInst->imm < -32768)
        {
            error("Invalid offset");
        }
        switch (currInst->instType)
        {
        case RTYPE:
            currInst->inst[2] = hexTable[currInst->rs];
            currInst->inst[3] = hexTable[currInst->rt];
            currInst->inst[4] = hexTable[currInst->rd];
            break;
        case ITYPE:
            currInst->inst[2] = hexTable[currInst->rs];
            currInst->inst[3] = hexTable[currInst->rt];
            int2hex16(temp, currInst->imm);
            currInst->inst[4] = *temp;
            currInst->inst[5] = *(temp+1);
            currInst->inst[6] = *(temp+2);
            currInst->inst[7] = *(temp+3);
            break;
        default:
            int2hex16(temp, currInst->imm);
            currInst->inst[5] = *(temp+1);
            currInst->inst[6] = *(temp+2);
            currInst->inst[7] = *(temp+3);
            break;
        }
        currInst->inst[8] = '\0';
        result = hex2int(currInst->inst);
        fprintf(machp, "%i\n", result);
    }
    free(temp);
    fclose(machp);
    fclose(assp);
    free(pSymTab);
    free(currInst);
}

void error(char *str)
{
    printf("%s",str);
    exit(1);
}
bool isDecimal(char* token)
{
    char c= token[0];
    return c>='0' && c<= '9' || c=='-';
}

int findSymTabLen (FILE *inputFile)
{
    int count = 0;
    int lineSize = 100;
    char line[50];
    while(fgets(line, lineSize, inputFile))
    {
        if((line[0] == ' ') || (line[0] == '\t'));
        else
            count++;
    }
    rewind(inputFile);
    return count;
}

int fillSymTab (struct symbolTable *symT,FILE *inputFile)
{
    int lineNo = 0;
    int lineSize = 100;
    char line[50];
    int i=0;
    char *token;
    while(fgets(line, lineSize, inputFile))
    {
        if(( line[0] == ' ') || (line[0] == '\t'));
        else
        {
            token = strtok(line,"\t, ");
            strcpy(symT[i].symbol, token);
            symT[i++].value = lineNo;
        }
        lineNo++;
    }
    rewind(inputFile);
    return lineNo;
}

int hex2int(char *hex)
{
    int result = 0;
    while ((*hex) != '\0')
    {
        if(('0' <= (*hex)) && ((*hex) <= '9'))
            result = result*16 + (*hex) - '0';
        else if (('a' <= (*hex)) && ((*hex) <='f'))
            result = result*16 + (*hex) - 'a' + 10;
        else if (('A' <= (*hex)) && ((*hex) <='F'))
            result = result*16 + (*hex) - 'A' + 10;
        hex++;
    }
    return result;
}

void int2hex16(char *lower, int a) // function got some changes for negative numbers
{
    sprintf(lower, "%x", a);
    if(a < 0x0)
    {
        lower[3] = lower[7];
        lower[2] = lower[6];
        lower[1] = lower[5];
        lower[0] = lower[4];
    }
    else if(a < 0x10)
    {
        lower[3] = lower[0];
        lower[2] = '0';
        lower[1] = '0';
        lower[0] = '0';

    }
    else if(a < 0x100)
    {
        lower[3] = lower[1];
        lower[2] = lower[0];
        lower[1] = '0';
        lower[0] = '0';
    }
    else if(a < 0x1000)
    {
        lower[3] = lower[2];
        lower[2] = lower[1];
        lower[1] = lower[0];
        lower[0] = '0';
    }
    lower[4] = '\0';
}
