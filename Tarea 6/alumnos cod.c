#include <stdio.h>
#include <string.h>

#define MAX_ALUMNOS 10

typedef struct {
    char nombre[50];
    char apellido[50];
    float promedio;
    char materia[50];
} Alumno;

void generarXML(Alumno lista[], int cantidad) {
    FILE *archivo = fopen("alumnos.xml", "w");
    if (archivo == NULL) {
        printf("Error al crear el archivo XML\n");
        return;
    }

    fprintf(archivo, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(archivo, "<Alumnos>\n");

    for (int i = 0; i < cantidad; i++) {
        fprintf(archivo, "    <Alumno>\n");
        fprintf(archivo, "        <Nombre>%s</Nombre>\n", lista[i].nombre);
        fprintf(archivo, "        <Apellido>%s</Apellido>\n", lista[i].apellido);
        fprintf(archivo, "        <Promedio>%.1f</Promedio>\n", lista[i].promedio);
        fprintf(archivo, "        <Materia>%s</Materia>\n", lista[i].materia);
        fprintf(archivo, "    </Alumno>\n");
    }

    fprintf(archivo, "</Alumnos>\n");
    fclose(archivo);
    printf("Archivo alumnos.xml generado correctamente.\n");
}

void generarJSON(Alumno lista[], int cantidad) {
    FILE *archivo = fopen("alumnos.json", "w");
    if (archivo == NULL) {
        printf("Error al crear el archivo JSON\n");
        return;
    }

    fprintf(archivo, "{\n    \"Alumnos\": [\n");

    for (int i = 0; i < cantidad; i++) {
        fprintf(archivo, "        {\n");
        fprintf(archivo, "            \"Nombre\": \"%s\",\n", lista[i].nombre);
        fprintf(archivo, "            \"Apellido\": \"%s\",\n", lista[i].apellido);
        fprintf(archivo, "            \"Promedio\": %.1f,\n", lista[i].promedio);
        fprintf(archivo, "            \"Materia\": \"%s\"\n", lista[i].materia);
        fprintf(archivo, "        }%s\n", (i < cantidad - 1) ? "," : "");
    }

    fprintf(archivo, "    ]\n}\n");
    fclose(archivo);
    printf("Archivo alumnos.json generado correctamente.\n");
}

int main() {
    Alumno alumnos[MAX_ALUMNOS] = {
        {"Juan", "Perez", 8.5, "Matematica"},
        {"Maria", "Gonzalez", 9.2, "Fisica"},
        {"Carlos", "Rodriguez", 7.8, "Quimica"},
        {"Ana", "Martinez", 8.9, "Historia"},
        {"Luis", "Fernandez", 6.5, "Biologia"},
        {"Laura", "Lopez", 9.5, "Literatura"},
        {"Diego", "Sanchez", 7.2, "Geografia"},
        {"Sofia", "Diaz", 8.1, "Ingles"},
        {"Pedro", "Torres", 7.9, "Educacion Fisica"},
        {"Valentina", "Ramirez", 9.0, "Computacion"}
    };

    generarXML(alumnos, MAX_ALUMNOS);
    generarJSON(alumnos, MAX_ALUMNOS);

    return 0;
}
