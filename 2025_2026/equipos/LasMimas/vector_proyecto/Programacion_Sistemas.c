#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

// Variables globales para el manejo de señales
sigjmp_buf punto_retorno;
int señal_recibida = 0;

// Manejador para SIGFPE (Error de coma flotante)
void manejador_sigfpe(int sig) {
    printf("\n⚠️  ¡Se capturó SIGFPE (Error de coma flotante)!\n");
    señal_recibida = sig;

    // Saltar de vuelta al punto seguro
    siglongjmp(punto_retorno, 1);
}

// Función para calcular pendiente con manejo de errores
double calcular_pendiente(double x1, double y1, double x2, double y2) {
    printf("\n--- Calculando pendiente ---\n");
    printf("Punto 1: (%.2f, %.2f)\n", x1, y1);
    printf("Punto 2: (%.2f, %.2f)\n", x2, y2);

    // Configurar el punto de retorno para siglongjmp
    if (sigsetjmp(punto_retorno, 1) == 0) {
        // Código normal de ejecución
        double delta_x = x2 - x1;
        double delta_y = y2 - y1;

        printf("Δx = %.2f - %.2f = %.2f\n", x2, x1, delta_x);
        printf("Δy = %.2f - %.2f = %.2f\n", y2, y1, delta_y);

        // Esta división puede causar SIGFPE si delta_x es 0
        double pendiente = delta_y / delta_x;

        printf("✅ Pendiente calculada: %.2f / %.2f = %.2f\n", delta_y, delta_x, pendiente);
        return pendiente;
    } else {
        // Se ejecuta después de siglongjmp (cuando ocurre SIGFPE)
        printf("❌ No se puede calcular pendiente (división por cero)\n");

        // Generar pendiente aleatoria
        srand(time(NULL) + getpid());
        double pendiente_aleatoria = ((double)rand() / RAND_MAX) * 20 - 10; // Entre -10 y 10

        printf("🎲 Generando pendiente aleatoria: %.2f\n", pendiente_aleatoria);
        return pendiente_aleatoria;
    }
}

// Función para mostrar información sobre la pendiente
void interpretar_pendiente(double pendiente) {
    printf("📈 Interpretación: ");

    if (isinf(pendiente)) {
        printf("Línea vertical (pendiente infinita)\n");
    } else if (pendiente == 0) {
        printf("Línea horizontal\n");
    } else if (pendiente > 0) {
        printf("Línea creciente (pendiente positiva)\n");
    } else {
        printf("Línea decreciente (pendiente negativa)\n");
    }
}

int main() {
    // Configurar el manejador de señales para SIGFPE
    struct sigaction sa;
    sa.sa_handler = manejador_sigfpe;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGFPE, &sa, NULL) == -1) {
        perror("Error configurando manejador de SIGFPE");
        return 1;
    }

    printf("=========================================\n");
    printf("   CALCULADORA DE PENDIENTES CON FPE\n");
    printf("=========================================\n");
    printf("Este programa maneja errores de división por cero\n");
    printf("generando pendientes aleatorias cuando es necesario.\n\n");

    // Casos de prueba
    double puntos[][4] = {
        {1.0, 2.0, 3.0, 4.0},    // Pendiente normal: 1.0
        {0.0, 0.0, 5.0, 10.0},   // Pendiente normal: 2.0
        {2.0, 3.0, 2.0, 7.0},    // ¡ERROR! División por cero (x1 = x2)
        {4.0, 1.0, 4.0, 8.0},    // ¡ERROR! División por cero (x1 = x2)
        {-1.0, -2.0, 3.0, 4.0},  // Pendiente normal: 1.5
        {0.0, 5.0, 0.0, 10.0}    // ¡ERROR! División por cero
    };

    int num_casos = sizeof(puntos) / sizeof(puntos[0]);

    for (int i = 0; i < num_casos; i++) {
        printf("\n" "🚀 CASO DE PRUEBA %d/%d\n", i + 1, num_casos);
        printf("====================\n");

        double x1 = puntos[i][0];
        double y1 = puntos[i][1];
        double x2 = puntos[i][2];
        double y2 = puntos[i][3];

        double pendiente = calcular_pendiente(x1, y1, x2, y2);
