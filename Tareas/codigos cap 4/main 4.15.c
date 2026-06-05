#include <stdio.h>
#include <stdlib.h>

int main()
{
    printf("Hello world!\n");
    return 0;
}
include <stdio.h>

/* Productoria.
El programa calcula la productoria de los N primeros números naturales. */

int productoria(int)    /* prototipo de funcion. */

void main(void)
{
    int NUM;

    /* Se escribe un do-while para verificar que el número del cual se
➥ quiere calcular la productoria sea correcto. */
do
{
    printf(“Ingresa el número del cual quieres calcular la
➥ productoria: “);
scanf(“%d”, &NUM);
}
while (NUM >100 || NUM < 1);
printf(“\nLa productoria de %d es: %d”, NUM, Productoria(NUM));
}

int productoria(int N)
/* La función calcula la productoria de N. */
{
    int I, PRO = 1;
for (I = 1; I <= N; I++)
PRO *= I;
return (PRO);
}
