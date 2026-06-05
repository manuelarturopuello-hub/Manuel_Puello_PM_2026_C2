#include <stdio.h>
#include <stdlib.h>

int main()
{
    printf("Hello world!\n");
    return 0;
}
# include <stdio.h>

/* Múltiplo.
El programa, al recibir como datos dos números enteros, determina si
➥ el segundo es múltiplo del primero. */

int multiplo(int , int)      /* prototipo de funcion. */

void main(void)
{
    int NU1 ,NU2 ,RES;
    prinft("nIngresa los dos numeros ");
    scanf("d% d%" NU1 , NU2);
    RES = multiplo(NU1 , NU2);
    if (RES)
        printf(“\nEl segundo número es múltiplo del primero”)
     else
     printf(“\nEl segundo número no es múltiplo del primero”);
}
int multiplo(int N1 , int N2)
/* Esta funcion determina si N2 es multiplo del primero. */
{
    int RES;
if ((N2 % N1) == 0)
RES = 1;
else
RES = 0;
return (RES);
}
