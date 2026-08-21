#include <stdio.h>
#include <string.h>

int longitud(char cad); /* Prototipo de función. */
void main(void)
{
int i, n, l = -1, p, t;
char cad[50], FRA[20][50];
printf("\nIngrese el número de filas del arreglo: ");
scanf("%d", &n);
for (i=0; i<n; i++)
{
strcpy(cad, FRA[I]);
t = longitud(cad);
if (t > 1)
{
l = t;
p = i;
}
}
printf("\nLa cadena con mayor longitud es: ");
puts(FRA[p]);
printf("\nLongitud: %d", l);
}
int longitud(char *cadena)
/* Esta función calcula la longitud de la cadena. Es idéntica a la función
➥cuenta del programa 7.13. */
{
int cue = 0;
while (! cadena[cue] == '\0')
cue++;
return (cue);
}
