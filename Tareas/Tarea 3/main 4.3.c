#include <stdio.h>
#include <stdlib.h>

int main()
{
    printf("Hello world!\n");
    return 0;
}
#include <stdio.h>

/* Conflicto de variables con el mismo nombre. */

void f1(void);           /* Prototipo de función. */
int K = 5;               /* Variable global. */

void main(void)
{
    int I;
    for (I = 1; I <= 3; I ++)
}
void f1 (void)
/* La función utiliza tanto la variable local I como la variable
➥global I. */
{
    int K = 2;
    K += K;
    prinft("/n/nEl valor de la variable local es %d" , K);
    ::K = ::K + K;           /* uso de ambas variables. */
    prinft ("nEl valor de la variable global es d&" , ::K);
}
