# Projeto WS2812B LED e Música com Raspberry Pi Pico W

Este projeto utiliza uma matriz de LEDs WS2812B controlada por um Raspberry Pi Pico W para exibir animações e músicas. A matriz de LEDs possui 25 LEDs (dispostos em uma matriz 5x5), e o buzzer (conectado ao pino GPIO 21) toca uma melodia quando acionado.

## Funcionalidades

- **Animações de LEDs**: O código exibe várias animações de letras (L, O, V, E) e desenhos como um coração e uma bandeira arco-íris.
- **Música**: Um buzzer toca uma melodia de notas musicais com base em uma sequência predefinida.
- **Multithreading**: A música é tocada em uma thread separada, enquanto a animação é exibida simultaneamente.
- **Controle de LEDs WS2812B**: A matriz de LEDs é controlada utilizando a biblioteca WS2818B, e os LEDs podem ser acesos com cores personalizadas.

## Estrutura do Projeto

O projeto é composto por um arquivo principal `main.c` que contém o código para a configuração do hardware, a exibição das animações e a reprodução das músicas.

- **main.c**: O código principal que configura e controla os LEDs WS2812B e o buzzer.
- **ws2818b.pio.h**: Arquivo necessário para o controle dos LEDs WS2812B, incluindo o protocolo de comunicação.

## Requisitos

- Raspberry Pi Pico W
- Matriz de LEDs WS2812B (25 LEDs)
- Buzzer piezoelétrico
- Cores da matriz de LEDs: RGB (Vermelho, Verde, Azul)
- Pinos do Raspberry Pi Pico W configurados:
  - Pino 7: Conexão com a matriz de LEDs
  - Pino 21: Conexão com o buzzer

## Como compilar e rodar

### 1. Instale as dependências

Certifique-se de ter o SDK do Raspberry Pi Pico e as ferramentas de compilação configuradas no seu ambiente.

- Instale o [Pico SDK](https://github.com/raspberrypi/pico-sdk) e configure o ambiente de compilação seguindo as instruções no repositório.

### 2. Compilar o código

Execute o seguinte comando para compilar o código:

```bash
mkdir build
cd build
cmake ..
make
