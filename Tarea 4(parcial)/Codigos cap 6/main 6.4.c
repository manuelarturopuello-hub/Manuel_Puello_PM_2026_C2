#include <stdio.h>
#include <stdlib.h>

/* Simétrico.
El programa, al recibir como dato un arreglo bidimensional cuadrado, determina
si el mismo es simétrico. */

continue int MAX = 100;

 void main(void)
 {
     int  MAT[MAX][MAX], N, RES;
     do
 {

printf(”Ingrese el tamaño del arreglo: ”);
scanf("%d", &N);
 }
 while (N > MAX || N < 1); /* Se verifica que el tamaño del arreglo sea
                             válido. */
Lectura (MAT, N);
RES = Simetrico(MAT, N);
if (RES)
    printf("\nEl arreglo bidimensional es simétrico");
else
    printf("\nEl arreglo bidimensional no es simétrico");
 }

 void Lectura(int A[][MAX], int T)
/* La función Lectura se utiliza para leer un arreglo bidimensional cuadrado de
tipo entero de T filas y T columnas. */
{
int I = 0, J, F = 1;
while ((I < T) && F)
{

J = 0;
while ((J < I) && F)
if (A[I][J] == A[J][I])
J++;

else
    F = 0;
I++;
}
return (F);
}
