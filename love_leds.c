#include <stdio.h>
#include "pico/stdlib.h"
#include <hardware/pio.h>
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "pico/multicore.h"
#include "ws2818b.pio.h"

// Configurações
#define LED_COUNT 25                // Número de LEDs (matriz 5x5)
#define LED_PIN 7                   // Pino GPIO conectado aos LEDs
#define BUZZER_PIN 21               // Pino GPIO conectado ao buzzer

struct pixel_t {
    uint8_t G, R, B;                // Componentes de cor: Verde, Vermelho, Azul
};

typedef struct pixel_t pixel_t;
pixel_t leds[LED_COUNT];
PIO np_pio;
uint sm;

// Definições de notas em Hz
#define NOTE_B4 246.94
#define NOTE_C5 261.63
#define NOTE_D5 293.66
#define NOTE_E5 329.63
#define NOTE_F5 349.23
#define NOTE_G5 392.00
#define NOTE_A5 440.00
#define NOTE_B5 493.88
#define NOTE_C6 523.25
#define NOTE_D6 587.33
#define NOTE_E6 659.26
#define NOTE_F6 698.46
#define NOTE_G6 783.99
#define NOTE_A6 880.00
#define NOTE_B6 987.77
#define NOTE_C7 1046.50
#define NOTE_D7 1174.66
#define NOTE_E7 1318.51
#define NOTE_F7 1396.91
#define NOTE_G7 1567.98
#define NOTE_A7 1760.00
#define NOTE_B7 1975.53
#define NOTE_C8 2093.00

// Notas com sustenido (#)
#define NOTE_FS5 740.00
#define NOTE_GS5 830.61

// Definição para descanso (pausa)
#define REST 0


// Duração das notas (em ms)
#define WHOLE_NOTE    1000
#define HALF_NOTE     500
#define QUARTER_NOTE  250
#define EIGHTH_NOTE   125


int melody[] = {
    NOTE_FS5, NOTE_FS5, NOTE_D5, NOTE_B4, REST, NOTE_B4, REST, NOTE_E5, REST,
    NOTE_E5, REST, NOTE_E5, NOTE_GS5, NOTE_GS5, NOTE_A5, NOTE_B5, NOTE_A5, 
    NOTE_A5, NOTE_A5, NOTE_E5, REST, NOTE_D5, REST, NOTE_FS5, REST, NOTE_FS5,
    REST, NOTE_FS5, NOTE_E5, NOTE_E5, NOTE_FS5, NOTE_E5, REST, REST
};

// Duração das notas
int durations[] = {
    EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, 
    EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, 
    EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, 
    EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, 
    EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE,
    EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE
};

// Função para configurar e tocar uma nota
void set_pwm_pin(uint pin, uint freq, uint duty_c) { // duty_c entre 0 e 10000
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_config config = pwm_get_default_config();
    float div = (float)clock_get_hz(clk_sys) / (freq * 10000); // Obtém a frequência do clock do sistema
    pwm_config_set_clkdiv(&config, div);
    pwm_config_set_wrap(&config, 10000);
    pwm_init(slice_num, &config, true); // Inicia o PWM com a configuração
    pwm_set_gpio_level(pin, duty_c);   // Define o nível do ciclo de trabalho
}

// Função para tocar a música
void tocar_musica() {
    uint buzzer_pin = 21; // Pino conectado ao buzzer

    // Toca a melodia
    for (int i = 0; i < sizeof(melody) / sizeof(int); i++) {
        int freq = melody[i];
        int duration = durations[i];

        if (freq > 0) {
            // Configura e toca a nota com 50% de duty cycle
            set_pwm_pin(buzzer_pin, freq, 5000);
        } else {
            // Pausa (silêncio)
            pwm_set_gpio_level(buzzer_pin, 0);
        }

        // Espera pela duração da nota
        sleep_ms(duration);

        // Pausa entre as notas
        pwm_set_gpio_level(buzzer_pin, 0);
        sleep_ms(50);
    }
}

// Inicialização dos LEDs
void npInit(uint pin) {
    uint offset = pio_add_program(pio0, &ws2818b_program);
    np_pio = pio0;
    sm = pio_claim_unused_sm(np_pio, false);
    if (sm < 0) {
        np_pio = pio1;
        sm = pio_claim_unused_sm(np_pio, true);
    }
    ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);

    for (uint i = 0; i < LED_COUNT; ++i) {
        leds[i].R = 0;
        leds[i].G = 0;
        leds[i].B = 0;
    }
}

// Define um LED com cores específicas
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
    leds[index].R = r * 0.3;
    leds[index].G = g * 0.3;
    leds[index].B = b * 0.3;
}

// Limpa todos os LEDs
void npClear() {
    for (uint i = 0; i < LED_COUNT; ++i)
        npSetLED(i, 0, 0, 0);
}

// Atualiza o hardware dos LEDs
void npWrite() {
    for (uint i = 0; i < LED_COUNT; ++i) {
        pio_sm_put_blocking(np_pio, sm, leds[i].G);
        pio_sm_put_blocking(np_pio, sm, leds[i].R);
        pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
}

// Mostra letras para formar "LOVE"
void drawLetterL() {
    npClear();
    npSetLED(1, 255, 0, 0); // Parte vertical
    npSetLED(2, 255, 0, 0);
    npSetLED(3, 255, 0, 0);
    npSetLED(6, 255, 0, 0);
    npSetLED(13, 255, 0, 0); // Parte horizontal
    npSetLED(16, 255, 0, 0); // Parte horizontal
    npSetLED(23, 255, 0, 0); // Parte horizontal
    npWrite();
    sleep_ms(1000);
}

void drawLetterO() {
    npClear();
    npSetLED(1, 0, 255, 0); // Parte superior
    npSetLED(2, 0, 255, 0);
    npSetLED(3, 0, 255, 0); // Laterais
    npSetLED(6, 0, 255, 0);
    npSetLED(13, 0, 255, 0); // Parte inferior
    npSetLED(16, 0, 255, 0);
    npSetLED(23, 0, 255, 0);
    npSetLED(22, 0, 255, 0);
    npSetLED(21, 0, 255, 0);
    npSetLED(18, 0, 255, 0);
    npSetLED(11, 0, 255, 0);
    npSetLED(8, 0, 255, 0);
    npWrite();
    sleep_ms(1000);
}

void drawLetterV() {
    npClear();
    npSetLED(24, 0, 0, 255); // Parte superior
    npSetLED(16, 0, 0, 255);
    npSetLED(13, 0, 0, 255); // Parte central
    npSetLED(7, 0, 0, 255);
    npSetLED(2, 0, 0, 255); // Inferior
    npSetLED(11, 0, 0, 255);
    npSetLED(18, 0, 0, 255);
    npSetLED(20, 0, 0, 255);
    npWrite();
    sleep_ms(1000);
}

void drawLetterE() {
    npClear();
    npSetLED(1, 255, 255, 0); // Vertical
    npSetLED(2, 255, 255, 0);
    npSetLED(3, 255, 255, 0);
    npSetLED(6, 255, 255, 0);
    npSetLED(16, 255, 255, 0);
    npSetLED(13, 255, 255, 0); // Superior
    npSetLED(12, 255, 255, 0);
    npSetLED(11, 255, 255, 0); // Central
    npSetLED(23, 255, 255, 0);
    npSetLED(22, 255, 255, 0); // Inferior
    npSetLED(21, 255, 255, 0);
    npWrite();
    sleep_ms(1000);
}

void drawCoracao() {
    for (int i = 0; i < 3; i++) {  // Piscar 3 vezes
        // Desenha o coração
        npClear();
        npSetLED(23, 255, 0, 0); // Vermelho
        npSetLED(21, 255, 0, 0);
        npSetLED(15, 255, 0, 0);
        npSetLED(17, 255, 0, 0);
        npSetLED(19, 255, 0, 0);
        npSetLED(14, 255, 0, 0);
        npSetLED(10, 255, 0, 0);
        npSetLED(6, 255, 0, 0);
        npSetLED(8, 255, 0, 0);
        npSetLED(2, 255, 0, 0);
        npSetLED(16, 255, 102, 204); // Rosa
        npSetLED(18, 255, 102, 204);
        npSetLED(13, 255, 102, 204);
        npSetLED(12, 255, 102, 204);
        npSetLED(11, 255, 102, 204);
        npSetLED(7, 255, 102, 204);
        npWrite();
        sleep_ms(500);  // Coração visível por 500 ms

        // Apaga todos os LEDs
        npClear();
        npWrite();
        sleep_ms(200);  // Coração apagado por 500 ms
    }
}

// Exibe bandeira arco-íris, linha por linha
void rainbowFlag() {
    
    uint8_t colors[5][3] = {
        {255, 0, 0},   // Vermelho
        {255, 165, 0}, // Laranja
        {255, 255, 0}, // Amarelo
        {0, 255, 0},   // Verde
        {0, 0, 255}    // Azul
    };

    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 5; col++) {
            // Calcula o índice e acende os LEDs da linha atual
            npSetLED(row * 5 + col, colors[row][0], colors[row][1], colors[row][2]);
        }
        npWrite();         // Atualiza os LEDs da linha atual
        sleep_ms(500);     // Espera 500 ms antes de passar para a próxima linha
    }

    sleep_ms(1000);         // Mantém a bandeira acesa por 2 segundos
}


// Função para gerar uma onda de arco-íris
void rainbowWave() {
    for (int t = 0; t < LED_COUNT; t++) {
        for (int i = 0; i < LED_COUNT; i++) {
            uint8_t r = (i + t) % 3 == 0 ? 255 : 0; // Vermelho em padrão cíclico
            uint8_t g = (i + t) % 3 == 1 ? 255 : 0; // Verde em padrão cíclico
            uint8_t b = (i + t) % 3 == 2 ? 255 : 0; // Azul em padrão cíclico
            npSetLED(i, r, g, b);
        }
        npWrite();
        sleep_ms(100);
    }
}

// Animação principal
void animate() {
    rainbowFlag();
    drawLetterL();
    drawLetterO();
    drawLetterV();
    drawLetterE();
    drawCoracao();
    rainbowWave();
    npClear();
}

void music_thread() {
    while (true) {
        tocar_musica();
        //sleep_ms(2000); // Pausa entre as músicas
    }
}

int main() {
    stdio_init_all();
    npInit(LED_PIN);
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);

    // Cria uma thread para executar a música em segundo plano
    multicore_launch_core1(music_thread);

    while (true) {
        animate(); // Exibir LOVE seguido da bandeira
    }
}