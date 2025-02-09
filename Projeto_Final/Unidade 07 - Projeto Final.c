#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

// Definição de pinos
#define TRIG_PIN 15
#define ECHO_PIN 14
#define BUZZER_PIN 6
#define VIBRATION_PIN 7
#define PIR_PIN 8 // Pino para o sensor PIR

// Limites de distância para diferentes zonas
#define SAFE_ZONE 100
#define ALERT_ZONE 50
#define DANGER_ZONE 20

void init_hardware()
{
    stdio_init_all();
    gpio_init(TRIG_PIN);
    gpio_set_dir(TRIG_PIN, GPIO_OUT);
    gpio_init(ECHO_PIN);
    gpio_set_dir(ECHO_PIN, GPIO_IN);
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_init(VIBRATION_PIN);
    gpio_set_dir(VIBRATION_PIN, GPIO_OUT);
    gpio_init(PIR_PIN);
    gpio_set_dir(PIR_PIN, GPIO_IN); // Sensor PIR como entrada
}

// Função para medir a distância
float measure_distance()
{
    gpio_put(TRIG_PIN, 1);
    sleep_us(10);
    gpio_put(TRIG_PIN, 0);

    while (gpio_get(ECHO_PIN) == 0)
        ;
    absolute_time_t start_time = get_absolute_time();
    while (gpio_get(ECHO_PIN) == 1)
        ;
    absolute_time_t end_time = get_absolute_time();

    int64_t pulse_duration = absolute_time_diff_us(start_time, end_time);
    float distance = (pulse_duration * 0.0343f) / 2;
    return distance;
}

// Função para emitir feedback sonoro com base na distância
void sound_alert(float distance)
{
    if (distance < DANGER_ZONE)
    {
        // Alerta sonoro contínuo e rápido
        gpio_put(BUZZER_PIN, 1);
        sleep_ms(100);
        gpio_put(BUZZER_PIN, 0);
        sleep_ms(100);
    }
    else if (distance < ALERT_ZONE)
    {
        // Alerta sonoro com frequência média
        gpio_put(BUZZER_PIN, 1);
        sleep_ms(300);
        gpio_put(BUZZER_PIN, 0);
        sleep_ms(300);
    }
    else if (distance < SAFE_ZONE)
    {
        // Alerta sonoro intermitente
        gpio_put(BUZZER_PIN, 1);
        sleep_ms(500);
        gpio_put(BUZZER_PIN, 0);
        sleep_ms(500);
    }
    else
    {
        // Sem alerta sonoro
        gpio_put(BUZZER_PIN, 0);
    }
}

// Função para emitir feedback vibratório com base na distância
void vibration_alert(float distance)
{
    if (distance < DANGER_ZONE)
    {
        // Vibração contínua e rápida
        gpio_put(VIBRATION_PIN, 1);
        sleep_ms(100);
        gpio_put(VIBRATION_PIN, 0);
        sleep_ms(100);
    }
    else if (distance < ALERT_ZONE)
    {
        // Vibração moderada
        gpio_put(VIBRATION_PIN, 1);
        sleep_ms(300);
        gpio_put(VIBRATION_PIN, 0);
        sleep_ms(300);
    }
    else if (distance < SAFE_ZONE)
    {
        // Vibração suave
        gpio_put(VIBRATION_PIN, 1);
        sleep_ms(500);
        gpio_put(VIBRATION_PIN, 0);
        sleep_ms(500);
    }
    else
    {
        // Sem vibração
        gpio_put(VIBRATION_PIN, 0);
    }
}

// Função para alertar sobre movimento detectado
void motion_alert()
{
    gpio_put(BUZZER_PIN, 1);
    gpio_put(VIBRATION_PIN, 1);
    sleep_ms(500); // Sinal mais longo para movimento
    gpio_put(BUZZER_PIN, 0);
    gpio_put(VIBRATION_PIN, 0);
}

int main()
{
    init_hardware();

    while (1)
    {
        bool is_moving = gpio_get(PIR_PIN); // Detecta se há movimento com o PIR
        float distance = measure_distance();

        // Exibe a distância medida no console para depuração
        printf("Distancia: %.2f cm, Movimento: %d\n", distance, is_moving);

        // Alerta por movimento detectado
        if (is_moving)
        {
            motion_alert();
        }

        // Alerta sonoro e vibratório baseado na distância
        sound_alert(distance);
        vibration_alert(distance);

        sleep_ms(500); // Aguarda antes de verificar novamente
    }
}
