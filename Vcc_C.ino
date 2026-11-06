#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h> // Для функции ltoa (перевод числа в текст)

// 1. Сами настраиваем передачу данных по UART (проводкам TX/RX)
void uart_init(unsigned int ubrr) {
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0); // Включаем прием и передачу
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // Формат: 8 бит данных
}

// 2. Отправка одной буквы в порт
void uart_putchar(char c) {
    while (!(UCSR0A & (1 << UDRE0))); // Ждем, пока освободится буфер передачи
    UDR0 = c;                         // Закидываем символ в железо
}

// 3. Отправка целой строки
void uart_print(const char* str) {
    while (*str) {
        uart_putchar(*str++);
    }
}

// 4. Легкий вывод дробных чисел (без тяжелого printf)
void uart_print_float(float val, unsigned char decimal_digits) {
    long int_part = (long)val;
    char buf[10];
    ltoa(int_part, buf, 10);
    uart_print(buf); // Печатаем целую часть
    
    uart_putchar('.'); // Ставим точку
    
    float frac = val - (float)int_part;
    for (unsigned char i = 0; i < decimal_digits; i++) {
        frac *= 10.0;
        char digit = (char)frac;
        uart_putchar('0' + digit); // Печатаем цифры после точки по одной
        frac -= (float)digit;
    }
}

// 5. Функция измерения напряжения через регистры АЦП
float read_vcc(void) {
    ADMUX = (1 << REFS0) | (1 << MUX3) | (1 << MUX2) | (1 << MUX1); // Включаем опорник 1.1В
    _delay_ms(2); // Пауза на стабилизацию

    ADCSRA |= (1 << ADSC); // Запуск измерения
    while (ADCSRA & (1 << ADSC)); // Ждем окончания

    // ВАЖНО: Читаем результаты строго сначала младший ADCL, потом старший ADCH!
    unsigned int low = ADCL;
    unsigned int high = ADCH;
    long result = (high << 8) | low;

    if (result == 0) return 0.0;

    float voltage = 1125350.0 / (float)result / 1000.0;
    return voltage;
}

// 6. Главная точка входа (вместо setup/loop)
int main(void) {
    uart_init(103); // Настраиваем скорость UART на 9600 бод при 16 МГц

    // Включаем АЦП и ставим идеальный предделитель 128 для максимальной точности
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    while (1) { // Бесконечный цикл
        float vcc = read_vcc();

        uart_print("VCC: ");
        uart_print_float(vcc, 2); // Выводим напряжение с двумя знаками
        uart_print(" V\r\n");

        _delay_ms(1000); // Пауза 1 секунда
    }

    return 0;
}void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
