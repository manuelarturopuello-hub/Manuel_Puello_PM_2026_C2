#include <stdio.h>
#include <string.h>

/* Bienes raíces.
El programa maneja información sobre las propiedades que tiene una empresa
de bienes raíces de la ciudad de Lima, Perú, tanto para venta como para
renta. */

typedef struct /* Declaración de la estructura ubicación.*/
{
    char zona[20];
    char calle[20];
    char colo[20]; /* Colonia. */
} ubicacion;

typedef struct /* Declaración de la estructura propiedades.*/
{
    char clave[5];
    float scu;     /* Superficie cubierta. */
    float ste;     /* Superficie terreno. */
    char car[50];  /* Características. */
    ubicacion ubi; /* Observa que este campo es de tipo estructura ubicación. */
    float precio;
    char dispo;    /* Disponibilidad. */
} propiedades;

void Lectura(propiedades *, int); /* Prototipos de funciones. */
void F1(propiedades *, int);
void F2(propiedades *, int);

void main(void)
{
    propiedades PROPIE[100];
    /* Se declara un arreglo unidimensional de tipo estructura propiedades. */
    int TAM;
    do
    {
        printf("Ingrese el numero de propiedades: ");
        scanf("%d", &TAM);
    }
    while (TAM > 100 || TAM < 1);
    /* Se verifica que el tamaño del arreglo sea correcto. */

    Lectura(PROPIE, TAM);
    F1(PROPIE, TAM);
    F2(PROPIE, TAM);
}

void Lectura(propiedades A[], int T)
/* Esta función se utiliza para leer un arreglo unidimensional de tipo estructura
propiedades de T elementos. */
{
    int I;
    for (I = 0; I < T; I++)
    {
        printf("\nIngrese los datos de la propiedad %d", I + 1);
        printf("\nClave: ");
        fflush(stdin);
        gets(A[I].clave);
        printf("Superficie cubierta: ");
        scanf("%f", &A[I].scu);
        printf("Superficie terreno: ");
        scanf("%f", &A[I].ste);
        printf("Caracteristicas: ");
        fflush(stdin);
        gets(A[I].car);
        printf("Zona: ");
        gets(A[I].ubi.zona);
        printf("Calle: ");
        gets(A[I].ubi.calle);
        printf("Colonia: ");
        gets(A[I].ubi.colo);
        printf("Precio: ");
        scanf("%f", &A[I].precio);
        printf("Disponibilidad (V-Venta R-Renta): ");
        fflush(stdin);
        scanf("%c", &A[I].dispo);
    }
}

void F1(propiedades A[], int T)
/* Esta función genera un listado de las propiedades disponibles para la venta
cuyo precio se encuentra entre 450,000 y 650,000. */
{
    int I;
    printf("\n\t\tListado de Propiedades para Venta (Entre 450,000 y 650,000)");
    for (I = 0; I < T; I++)
    {
        if ((A[I].dispo == 'V') && (A[I].precio >= 450000) && (A[I].precio <= 650000))
        {
            printf("\nClave de la propiedad: ");
            puts(A[I].clave);
            printf("Superficie cubierta: %f", A[I].scu);
            printf("\nSuperficie terreno: %f", A[I].ste);
            printf("\nCaracteristicas: ");
            puts(A[I].car);
            printf("Calle: ");
            puts(A[I].ubi.calle);
            printf("Colonia: ");
            puts(A[I].ubi.colo);
            printf("Precio: %.2f\n", A[I].precio);
        }
    }
}

void F2(propiedades A[], int T)
/* Al recibir como datos una zona geográfica de Lima, Perú, y un cierto rango
respecto al monto, esta función genera un listado de todas las propiedades
disponibles para renta. */
{
    int I;
    float li, ls;
    char zon[20];
    printf("\n\t\tListado de Propiedades para Renta");
    printf("\nIngrese zona geográfica: ");
    fflush(stdin);
    gets(zon);
    printf("Ingrese el límite inferior del precio: ");
    scanf("%f", &li);
    printf("Ingrese el límite superior del precio: ");
    scanf("%f", &ls);

    for (I = 0; I < T; I++)
    {
        if ((A[I].dispo == 'R') && (strcmp(A[I].ubi.zona, zon) == 0) && (A[I].precio >= li) && (A[I].precio <= ls))
        {
            printf("\nClave de la propiedad: ");
            puts(A[I].clave);
            printf("Superficie cubierta: %.2f", A[I].scu);
            printf("\nSuperficie terreno: %.2f", A[I].ste);
            printf("\nCaracteristicas: ");
            puts(A[I].car);
            printf("Calle: ");
            puts(A[I].ubi.calle);
            printf("Colonia: ");
            puts(A[I].ubi.colo);
            printf("Precio: %.2f\n", A[I].precio);
        }
    }
}
