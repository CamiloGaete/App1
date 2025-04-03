#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definiciones de constantes
#define MAX_LINE 1024       // Máximo largo de una línea de texto
#define MAX_PIZZAS 1000     // Máximo número de ventas registradas
#define MAX_NAME 100        // Máximo largo de nombre de pizza, categoría, etc.

// Estructura para representar una venta de pizza
typedef struct {
    char pizza_id[MAX_NAME];
    char pizza_name[MAX_NAME];
    char order_date[20];
    int quantity;
    float total_price;
    char pizza_category[MAX_NAME];
    char pizza_ingredients[MAX_LINE];
} PizzaSale;

// Estructura para agrupar por tipo de pizza
typedef struct {
    char nombre[MAX_NAME];
    int cantidad_total;
} PizzaResumen;

// Arreglos para guardar ventas y el resumen por pizza
PizzaSale ventas[MAX_PIZZAS];
PizzaResumen resumen[100];
int total_ventas = 0;            // Contador de ventas leídas
int total_pizzas_unicas = 0;     // Contador de pizzas distintas

// Reinicia el resumen de pizzas agrupadas
void reiniciar_resumen() {
    total_pizzas_unicas = 0;
    for (int i = 0; i < 100; i++) {
        resumen[i].nombre[0] = '\0';
        resumen[i].cantidad_total = 0;
    }
}

// Agrupa ventas por nombre de pizza y suma cantidades
void agrupar_por_pizza() {
    reiniciar_resumen();
    for (int i = 0; i < total_ventas; i++) {
        int encontrado = 0;
        for (int j = 0; j < total_pizzas_unicas; j++) {
            if (strcmp(resumen[j].nombre, ventas[i].pizza_name) == 0) {
                resumen[j].cantidad_total += ventas[i].quantity;
                encontrado = 1;
                break;
            }
        }
        if (!encontrado) {
            strcpy(resumen[total_pizzas_unicas].nombre, ventas[i].pizza_name);
            resumen[total_pizzas_unicas].cantidad_total = ventas[i].quantity;
            total_pizzas_unicas++;
        }
    }
}

// Imprime el resumen agrupado (para debug)
void debug_resumen() {
    printf("\n--- DEBUG RESUMEN ---\n");
    for (int i = 0; i < total_pizzas_unicas; i++) {
        printf("%s: %d\n", resumen[i].nombre, resumen[i].cantidad_total);
    }
    printf("---------------------\n");
}

// Lee archivo CSV y carga las ventas en el arreglo
void leer_csv(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("No se pudo abrir el archivo");
        exit(1);
    }

    char line[MAX_LINE];
    fgets(line, MAX_LINE, file); // Lee encabezado

    while (fgets(line, MAX_LINE, file) && total_ventas < MAX_PIZZAS) {
        char* campos[12];
        int columna = 0;
        char* ptr = line;

        // Separación de campos con manejo de comillas
        while (*ptr && columna < 12) {
            char* start;
            if (*ptr == '"') {
                ptr++;
                start = ptr;
                while (*ptr && !(*ptr == '"' && *(ptr + 1) == ',')) ptr++;
                if (*ptr == '"') *ptr = '\0';
                ptr += 2;
            } else {
                start = ptr;
                while (*ptr && *ptr != ',') ptr++;
                if (*ptr == ',') *ptr = '\0';
                ptr++;
            }
            campos[columna++] = start;
        }

        // Asignación de campos si están completos
        if (columna == 12) {
            strcpy(ventas[total_ventas].pizza_id, campos[2]);
            ventas[total_ventas].quantity = atoi(campos[3]);
            strcpy(ventas[total_ventas].order_date, campos[4]);
            ventas[total_ventas].total_price = atof(campos[7]);
            strcpy(ventas[total_ventas].pizza_category, campos[9]);
            strcpy(ventas[total_ventas].pizza_ingredients, campos[10]);
            strcpy(ventas[total_ventas].pizza_name, campos[11]);

            // Quitar salto de línea final
            ventas[total_ventas].pizza_name[strcspn(ventas[total_ventas].pizza_name, "\r\n")] = 0;

            total_ventas++;
        }
    }

    fclose(file);
}

// Encuentra e imprime la pizza más vendida
void pizza_mas_vendida() {
    int max = 0;
    for (int i = 1; i < total_pizzas_unicas; i++) {
        if (resumen[i].cantidad_total > resumen[max].cantidad_total) {
            max = i;
        }
    }
    printf("Pizza mas vendida: %s (%d ventas)\n", resumen[max].nombre, resumen[max].cantidad_total);
}

// Encuentra e imprime la pizza menos vendida
void pizza_menos_vendida() {
    int min = 0;
    for (int i = 1; i < total_pizzas_unicas; i++) {
        if (resumen[i].cantidad_total < resumen[min].cantidad_total) {
            min = i;
        }
    }
    printf("Pizza menos vendida: %s (%d ventas)\n", resumen[min].nombre, resumen[min].cantidad_total);
}

// Encuentra fechas con más o menos ventas (por dinero o por cantidad)
void fecha_mas_menos(const char* tipo, int por_dinero) {
    float total_por_dia[1000] = {0};
    int cantidad_por_dia[1000] = {0};
    char fechas[1000][20];
    int total_fechas = 0;

    // Agrupa por fecha
    for (int i = 0; i < total_ventas; i++) {
        int index = -1;
