#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINEA 200
#define MAX_CAMPO 50

typedef struct {
    char nombre[MAX_CAMPO];
    char apellido[MAX_CAMPO];
    float promedio;
    char materia[MAX_CAMPO];
} Alumno;

/* Extrae el contenido de una etiqueta tipo <Tag>valor</Tag> */
void extraerValor(char *linea, char *destino) {
    char *inicio = strchr(linea, '>');
    char *fin = strrchr(linea, '<');

    if (inicio == NULL || fin == NULL || fin <= inicio) {
        destino[0] = '\0';
        return;
    }

    inicio++; /* saltar el '>' */
    int longitud = fin - inicio;
    strncpy(destino, inicio, longitud);
    destino[longitud] = '\0';
}

/* Primera pasada: cuenta cuántos <Alumno> hay en el archivo */
int contarAlumnos(const char *nombreArchivo) {
    FILE *archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL) {
        printf("Error: no se pudo abrir el archivo %s\n", nombreArchivo);
        return -1;
    }

    char linea[MAX_LINEA];
    int contador = 0;

    while (fgets(linea, MAX_LINEA, archivo) != NULL) {
        if (strstr(linea, "<Alumno>") != NULL) {
            contador++;
        }
    }

    fclose(archivo);
    return contador;
}

/* Segunda pasada: carga los datos en el array ya reservado */
int cargarAlumnos(const char *nombreArchivo, Alumno *lista, int n) {
    FILE *archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL) {
        printf("Error: no se pudo abrir el archivo %s\n", nombreArchivo);
        return 0;
    }

    char linea[MAX_LINEA];
    char campo[MAX_CAMPO];
    int indice = -1;

    while (fgets(linea, MAX_LINEA, archivo) != NULL && indice < n) {
        if (strstr(linea, "<Alumno>") != NULL) {
            indice++;
        } else if (strstr(linea, "<Nombre>") != NULL) {
            extraerValor(linea, lista[indice].nombre);
        } else if (strstr(linea, "<Apellido>") != NULL) {
            extraerValor(linea, lista[indice].apellido);
        } else if (strstr(linea, "<Promedio>") != NULL) {
            extraerValor(linea, campo);
            lista[indice].promedio = atof(campo);
        } else if (strstr(linea, "<Materia>") != NULL) {
            extraerValor(linea, lista[indice].materia);
        }
    }

    fclose(archivo);
    return 1;
}

void imprimirAlumnos(Alumno *lista, int n) {
    printf("\n=== Listado de Alumnos (%d) ===\n\n", n);
    for (int i = 0; i < n; i++) {
        printf("Alumno %d:\n", i + 1);
        printf("  Nombre:   %s\n", lista[i].nombre);
        printf("  Apellido: %s\n", lista[i].apellido);
        printf("  Promedio: %.1f\n", lista[i].promedio);
        printf("  Materia:  %s\n", lista[i].materia);
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    const char *nombreArchivo = "alumnos.xml";
    if (argc > 1) {
        nombreArchivo = argv[1];
    }

    /* Paso 1: contar cuántos elementos <Alumno> hay */
    int n = contarAlumnos(nombreArchivo);
    if (n <= 0) {
        printf("No se encontraron alumnos en el archivo, o hubo un error.\n");
        return 1;
    }
    printf("Cantidad de alumnos encontrados: %d\n", n);

    /* Paso 2: reservar memoria dinámica para n elementos */
    Alumno *alumnos = (Alumno *) malloc(n * sizeof(Alumno));
    if (alumnos == NULL) {
        printf("Error: no se pudo reservar memoria.\n");
        return 1;
    }

    /* Paso 3: cargar los datos del XML en el array */
    if (!cargarAlumnos(nombreArchivo, alumnos, n)) {
        free(alumnos);
        return 1;
    }

    /* Paso 4: imprimir los datos cargados */
    imprimirAlumnos(alumnos, n);

    /* Paso 5: liberar la memoria reservada */
    free(alumnos);

    return 0;
}
