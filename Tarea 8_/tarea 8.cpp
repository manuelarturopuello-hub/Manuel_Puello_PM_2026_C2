#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define MAX_ARCHIVOS 100
#define MAX_NOMBRE 256

// Verifica si la extensión es válida
int extensionValida(const char *nombre) {
    const char *ext = strrchr(nombre, '.');

    if (ext == NULL)
        return 0;

    ext++;

    if (strcasecmp(ext, "png") == 0 ||
        strcasecmp(ext, "jpg") == 0 ||
        strcasecmp(ext, "jpeg") == 0 ||
        strcasecmp(ext, "bmp") == 0) {
        return 1;
    }

    return 0;
}

// Verifica que el contenido realmente corresponda al formato
int verificarArchivo(SDL_Surface *imagen, const char *nombre) {
    if (imagen == NULL) {
        printf("\nERROR: El archivo '%s' no es una imagen valida.\n",
               nombre);
        return 0;
    }

    printf("El archivo '%s' es una imagen valida.\n", nombre);
    return 1;
}

// Convierte la imagen a blanco y negro
void convertirBlancoNegro(SDL_Surface *imagen) {

    SDL_LockSurface(imagen);

    Uint32 *pixeles = (Uint32 *)imagen->pixels;

    for (int y = 0; y < imagen->h; y++) {

        for (int x = 0; x < imagen->w; x++) {

            Uint32 pixel = pixeles[y * (imagen->pitch / 4) + x];

            Uint8 r, g, b, a;

            SDL_GetRGBA(pixel, imagen->format,
                        &r, &g, &b, &a);

            // Formula de luminancia
            Uint8 gris = (Uint8)(
                0.299 * r +
                0.587 * g +
                0.114 * b
            );

            pixeles[y * (imagen->pitch / 4) + x] =
                SDL_MapRGBA(imagen->format,
                            gris, gris, gris, a);
        }
    }

    SDL_UnlockSurface(imagen);
}

// Guarda la imagen convertida
int guardarImagen(SDL_Surface *imagen, const char *nombre) {

    char salida[MAX_NOMBRE];

    char *punto = strrchr(nombre, '.');

    if (punto != NULL) {

        int posicion = punto - nombre;

        strncpy(salida, nombre, posicion);
        salida[posicion] = '\0';

        strcat(salida, "_BN.bmp");

    } else {

        strcpy(salida, "imagen_BN.bmp");
    }

    if (SDL_SaveBMP(imagen, salida) == 0) {

        printf("\nImagen convertida correctamente.\n");
        printf("Archivo creado: %s\n", salida);

        return 1;
    }

    printf("\nError al guardar la imagen.\n");

    return 0;
}

int main() {

    DIR *directorio;

    struct dirent *entrada;

    char archivos[MAX_ARCHIVOS][MAX_NOMBRE];

    int cantidad = 0;
    int opcion;

    // Inicializar SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {

        printf("Error al iniciar SDL: %s\n",
               SDL_GetError());

        return 1;
    }

    // Inicializar PNG, JPG y otros formatos
    int flags = IMG_INIT_PNG | IMG_INIT_JPG;

    if ((IMG_Init(flags) & flags) != flags) {

        printf("Error al iniciar SDL_image: %s\n",
               IMG_GetError());

        SDL_Quit();

        return 1;
    }

    // Abrir carpeta actual
    directorio = opendir(".");

    if (directorio == NULL) {

        printf("No se pudo abrir la carpeta.\n");

        IMG_Quit();
        SDL_Quit();

        return 1;
    }

    printf("\n====================================\n");
    printf("   CONVERSOR DE IMAGENES A B/N\n");
    printf("====================================\n");

    printf("\nArchivos de imagen encontrados:\n\n");

    // Leer archivos de la carpeta
    while ((entrada = readdir(directorio)) != NULL) {

        if (extensionValida(entrada->d_name)) {

            if (cantidad < MAX_ARCHIVOS) {

                strcpy(archivos[cantidad],
                       entrada->d_name);

                printf("%d. %s\n",
                       cantidad + 1,
                       entrada->d_name);

                cantidad++;
            }
        }
    }

    closedir(directorio);

    if (cantidad == 0) {

        printf("\nNo se encontraron imagenes.\n");

        IMG_Quit();
        SDL_Quit();

        return 0;
    }

    // Menú
    printf("\n====================================\n");
    printf("Seleccione una imagen: ");

    scanf("%d", &opcion);

    if (opcion < 1 || opcion > cantidad) {

        printf("\nOpcion no valida.\n");

        IMG_Quit();
        SDL_Quit();

        return 1;
    }

    // Nombre del archivo seleccionado
    char *archivoSeleccionado =
        archivos[opcion - 1];

    printf("\nArchivo seleccionado: %s\n",
           archivoSeleccionado);

    // Cargar imagen
    SDL_Surface *imagen =
        IMG_Load(archivoSeleccionado);

    // Verificar contenido
    if (!verificarArchivo(imagen,
                          archivoSeleccionado)) {

        IMG_Quit();
        SDL_Quit();

        return 1;
    }

    printf("Formato detectado: %s\n",
           SDL_GetPixelFormatName(
               imagen->format->format));

    printf("Tamano: %d x %d pixeles\n",
           imagen->w,
           imagen->h);

    // Convertir
    convertirBlancoNegro(imagen);

    // Guardar
    guardarImagen(imagen,
                  archivoSeleccionado);

    SDL_FreeSurface(imagen);

    IMG_Quit();
    SDL_Quit();

    return 0;
}
