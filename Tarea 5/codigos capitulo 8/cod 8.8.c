#include <stdio.h>
#include <string.h>

/* Hospital.
El programa genera información acerca de los pacientes de un hospital. */

typedef struct /* Declaración de la estructura domicilio. */
{
    char cal[20]; /* Calle. */
    int num;      /* Número. */
    char col[20]; /* Colonia. */
    char cp[5];   /* Código Postal. */
    char ciu[20]; /* Ciudad. */
} domicilio;

typedef struct /* Declaración de la estructura paciente. */
{
    char nom[20]; /* Nombre y apellido. */
    int edad;
    char sexo;
    int con;       /* Condición. */
    domicilio dom; /* Observa que el campo dom es de tipo estructura domicilio. */
    char tel[10];  /* Teléfono. */
} paciente;

void Lectura(paciente *, int);
void F1(paciente *, int);
void F2(paciente *, int); /* Prototipos de funciones. */
void F3(paciente *, int);

void main(void)
{
    paciente HOSPITAL[100]; /* Arreglo unidimensional de tipo estructura paciente. */
    int TAM;
    do
    {
        printf("Ingrese el numero de pacientes: ");
        scanf("%d", &TAM);
    }
    while (TAM > 50 || TAM < 1); /* Se verifica que el tamaño del arreglo sea correcto. */

    Lectura(HOSPITAL, TAM);
    F1(HOSPITAL, TAM);
    F2(HOSPITAL, TAM);
    F3(HOSPITAL, TAM);
}

void Lectura(paciente A[], int T)
/* Esta función se utiliza para leer un arreglo unidimensional de tipo
estructura paciente de T elementos. */
{
    int I;
    for (I=0; I<T; I++)
    {
        printf("\n\t\tPaciente %d", I+1);
        fflush(stdin);
        printf("\nNombre: ");
        gets(A[I].nom);
        printf("Edad: ");
        scanf("%d", &A[I].edad);
        printf("Sexo (F-M): ");
        fflush(stdin);
        scanf("%c", &A[I].sexo);
        printf("Condicion (1..5): ");
        scanf("%d", &A[I].con);
        fflush(stdin);
        printf("\tCalle: ");
        gets(A[I].dom.cal);
        printf("\tNumero: ");
        scanf("%d", &A[I].dom.num);
        fflush(stdin);
        printf("\tColonia: ");
        gets(A[I].dom.col);
        fflush(stdin);
        printf("\tCodigo Postal: ");
        gets(A[I].dom.cp);
        fflush(stdin);
        printf("\tCiudad: ");
        gets(A[I].dom.ciu);
        fflush(stdin);
        printf("Telefono: ");
        gets(A[I].tel);
    }
}

void F1(paciente A[], int T)
/* Esta función se utiliza para obtener el porcentaje tanto de hombres como de
mujeres registrados en el hospital. */
{
    int I, FEM = 0, MAS = 0, TOT;
    for (I=0; I<T; I++)
    {
        switch (A[I].sexo)
        {
            case 'F': FEM++;
                      break;
            case 'M': MAS++;
                      break;
        }
    }
    TOT = FEM + MAS;
    if (TOT > 0)
    {
        printf("\nPorcentaje de Hombres: %.2f%%", (float)MAS / TOT * 100);
        printf("\nPorcentaje de Mujeres: %.2f%%", (float)FEM / TOT * 100);
    }
    else
    {
        printf("\nNo se registraron pacientes con sexo valido.");
    }
}

void F2(paciente A[], int T)
/* Esta función se utiliza para obtener el número de pacientes que ingresaron al
hospital en cada una de las categorías de condición. */
{
    int I, C1 = 0, C2 = 0, C3 = 0, C4 = 0, C5 = 0;
    for (I=0; I<T; I++)
    {
        switch (A[I].con)
        {
            case 1: C1++;
                    break;
            case 2: C2++;
                    break;
            case 3: C3++;
                    break;
            case 4: C4++;
                    break;
            case 5: C5++;
                    break;
        }
    }
    printf("\nNumero pacientes en condicion 1: %d", C1);
    printf("\nNumero pacientes en condicion 2: %d", C2);
    printf("\nNumero pacientes en condicion 3: %d", C3);
    printf("\nNumero pacientes en condicion 4: %d", C4);
    printf("\nNumero pacientes en condicion 5: %d", C5);
}

void F3(paciente A[], int T)
/* La función F3 se utiliza para generar el nombre y teléfono de todos los
pacientes que tuvieron una condición de ingreso de máxima gravedad (5). */
{
    int I;
    printf("\nPacientes ingresados en estado de gravedad");
    for (I=0; I<T; I++)
    {
        if (A[I].con == 5)
        {
            printf("\nNombre: %s\tTeléfono: %s", A[I].nom, A[I].tel);
        }
    }
}
