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

