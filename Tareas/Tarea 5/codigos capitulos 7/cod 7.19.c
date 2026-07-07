#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Cuenta palabras.
El programa calcula el número de palabras que hay en la cadena de caracteres. */
int cuentap(char *); /* Prototipo de función. */
void main(void)
{
int i;
char fra[50];
printf("\nIngrese la linea de texto: ");
gets(fra);
strcat(fra," "); /* Se agrega un espacio en blanco al final de la
cadena. */
i = cuentap(fra);
printf("\nLa linea de texto tiene %d palabras", i);
}
int cuentap(char *cad)
{
/* La función cuenta el número de palabras que hay en la cadena de
caracteres. */
char *cad0 = "";
int i = 0;
cad0 = strstr(cad," "); /* Se localiza el primer espacio en blanco en la
cadena. */
while (strcmp(cad, " "))
{
strcpy(cad, cad0);
i++;
cad0 = strstr (cad + 1," ");
/* Se busca un espacio en blanco a partir de la siguiente posición. */
}
return (i);
}
