#include <stdio.h>
#include <stdlib.h>

int main()
{
    printf("Hello world!\n");
    return 0;
}
#include <stdio.h>

/* Cubo-3.
El programa calcula el cubo de los 10 primeros números naturales con la
➥ ayuda de una función y utilizando parámetros por valor.

int cubo(int)          /* prototipo de funcion. El parametro es de ➥ tipo entero. */

void main(void)
{
    int I;
    for (I = 1; I <= 10; I++)
printf(“\nEl cubo de I es:%d”, cubo(I));
/* Llamada a la función cubo. El paso del parámetro es por valor. */
}
int cubo(int K)     /* K es un parametro por valor de tipo entero. */
/* la funcion calcula el cubo de parametro K. */
{
    return (K*K*K);
}
