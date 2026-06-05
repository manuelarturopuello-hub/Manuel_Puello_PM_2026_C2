#include <stdio.h>
#include <stdlib.h>

int main()
{
    printf("Hello world!\n");
    return 0;
}
#include <stdio.h>
/* Promedio curso.
El programa, al recibir como dato el promedio de un alumno en un curso
➥universitario, escribe aprobado si su promedio es mayor o igual a 6.

PRO: variable en tipo real. */

void main(void)
{
    float PRO;
printf(“ingrese el promedio del alumno: “);
scanf(“%f”, &PRO);
if (PRO >= 6)
printf(“\nAprobado”);
}
