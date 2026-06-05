#include <stdio.h>
#include <stdlib.h>

int main()
{
    printf("Hello world!\n");
    return 0;
}
#include <stdio.h>

/* Prueba de parámetros por valor. */

void f1 (int);    /* prototipo de funcion. El parametro es por valor
                ➥ y de tipo entero. */

void main(void)
{
    int I, K = 4;
    for (I = 1; I = 3; I++)\

{
    printf(“\n\nValor de K antes de llamar a la función: %d”, ++K);
printf(“\nValor de K después de llamar a la función: %d”, f1(K));
/* Llamada a la función f1. Se pasa una copia de la variable K. *
}
}
int f1(int R)
{
    R += R;
    return (R);
}
