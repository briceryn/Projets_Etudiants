#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int isDec(const char* str)
{
    int etat = 0;
    int curseur = 0;
    char c = str[curseur];

    while(1)
    {
        if(c == 0)
        {
            return ((etat == 1) || (etat == 2));
        }
        else if(etat == 0)
        {
            if(c == '0') etat = 2;
            else if ((c >= '1') && (c <= '9')) etat = 1;
            else return 0;
        }
        else if(etat == 1)
        {
            if((c >= '0') && (c <= '9')) etat = 1;
            else return 0;
        }
        else if(etat == 2)
        {
            return 0;
        }
        else
        {
            printf("ERREUR : état inconnu %d\n", etat);
            exit(1);
        }
        curseur ++; c = str[curseur];
    }
}

void test_isDec()
{
    char* data[] = {"0", "00", "075", "71", "45A2", "120x", "9900"};
    int expect[] = {1, 0, 0, 1, 0, 0, 1};
    int ndata = 7;
    int errors = 0;

    for (int i = 0; i < ndata; i++)
    {
        int result = isDec(data[i]);
        if (result != expect[i])
        {
            if (result)
            {
                printf("Erreur : %s acceptee a tort\n", data[i]);
            }
        else
        {
            printf("Erreur : %s refusee a tort\n", data[i]);
        }
        errors++;
        }
    }
    if (errors == 0)
    {
        printf("Test OK\n");
    }
    else
    {
        printf("%d erreurs\n", errors);
    }
}

int isBinHex(const char* str)
{
    int etat = 0;
    int curseur = 0;
    char c = str[curseur];

    while (1)
    {
        if (c == '\0')
        {
            return (etat == 3 || etat == 5);
        }
        else if (etat == 0)
        {
            if (c == '0')
            {
                etat = 1;
            }
            else
            {
                return 0;
            }
        }
        else if (etat == 1)
        {
            if (c == 'b')
            {
               etat = 2;
            }
            else if (c == 'x')
            {
                etat = 4;
            }
            else
            {
                return 0;
            }
        }
        else if (etat == 2)
        {
            if (c == '0' || c == '1')
            {
                etat = 3;
            }
            else
            {
                return 0;
            }
        }
        else if (etat == 3)
        {
            if (c == '0' || c == '1')
            {
              etat = 3;
            }
            else
            {
               return 0;
            }
        }
        else if (etat == 4)
        {
            if (isxdigit(c)) etat = 5;
            else
            {
               return 0;
            }
        }
        else if (etat == 5)
        {
            if (isxdigit(c))
            {
               etat = 5;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            printf("ERREUR : état inconnu %d\n", etat);
            exit(1);
        }
        curseur++;
        c = str[curseur];
    }
}

void test_isBinHex()
{
    char* data[] = {"0", "17", "0z75", "0b1110001", "0b1", "0b11111", "0b000", "0b002011", "0b10011tt", "0x123", "0xABD", "0xAB47", "0x04AA", "0x11G1", "0xH110", "1x67AB"};
    int expect[] = {0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0};
    int ndata = 16;
    int errors = 0;

    for (int i = 0; i < ndata; i++)
    {
        int result = isBinHex(data[i]);
        if (result != expect[i])
        {
                if(result)
                {
                    printf("Erreur : %s acceptee a tort\n", data[i]);
                }
                else
                {
                   printf("Erreur : %s refusee a tort\n", data[i]);
                }
            errors++;
        }
    }
    if (errors == 0)
    {
       printf("Test OK\n");
    }
    else
    {
        printf("%d erreurs\n", errors);
    }
}

int main()
{
    printf("Test isDec:\n");
    test_isDec();
    printf("\nTest isBinHex:\n");
    test_isBinHex();
    return 0;
}
