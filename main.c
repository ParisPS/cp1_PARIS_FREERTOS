/*
 * CP1 - Semáforos com FreeRTOS (ESP32 / ESP-IDF)
 * Três tarefas executando intercaladamente com 1s entre elas.
 * Saída:
 * [Tarefa 1] Executou - Paris
 * [Tarefa 2] Executou - Paris
 * [Tarefa 3] Executou - Paris
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// Semáforos binários para encadear as tarefas
static SemaphoreHandle_t sem1 = NULL;
static SemaphoreHandle_t sem2 = NULL;
static SemaphoreHandle_t sem3 = NULL;

static const TickType_t ONE_SEC = pdMS_TO_TICKS(1000);

static void task1(void *pvParameters)
{
    (void) pvParameters;
    for (;;)
    {
        // Espera liberação do semáforo 1
        xSemaphoreTake(sem1, portMAX_DELAY);

        // Ação da Tarefa 1
        printf("[Tarefa 1] Executou - Paris\n");

        // Intervalo de 1 segundo antes de liberar a próxima
        vTaskDelay(ONE_SEC);

        // Libera a Tarefa 2
        xSemaphoreGive(sem2);
    }
}

static void task2(void *pvParameters)
{
    (void) pvParameters;
    for (;;)
    {
        xSemaphoreTake(sem2, portMAX_DELAY);

        printf("[Tarefa 2] Executou - Paris\n");

        vTaskDelay(ONE_SEC);

        xSemaphoreGive(sem3);
    }
}

static void task3(void *pvParameters)
{
    (void) pvParameters;
    for (;;)
    {
        xSemaphoreTake(sem3, portMAX_DELAY);

        printf("[Tarefa 3] Executou - Paris\n");

        vTaskDelay(ONE_SEC);

        // Fecha o ciclo liberando a Tarefa 1
        xSemaphoreGive(sem1);
    }
}

void app_main(void)
{
    // Cria os semáforos binários
    sem1 = xSemaphoreCreateBinary();
    sem2 = xSemaphoreCreateBinary();
    sem3 = xSemaphoreCreateBinary();

    if (sem1 == NULL || sem2 == NULL || sem3 == NULL)
    {
        printf("Falha ao criar semáforos.\n");
        return;
    }

    // Cria as tarefas (mesma prioridade para evitar preempção "preferencial")
    BaseType_t ok = pdPASS;
    ok &= xTaskCreate(task1, "task1", 2048, NULL, 5, NULL);
    ok &= xTaskCreate(task2, "task2", 2048, NULL, 5, NULL);
    ok &= xTaskCreate(task3, "task3", 2048, NULL, 5, NULL);

    if (ok != pdPASS)
    {
        printf("Falha ao criar tarefas.\n");
        return;
    }

    // Dispara o ciclo liberando o semáforo da Tarefa 1
    xSemaphoreGive(sem1);
}