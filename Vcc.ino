void setup() {
  Serial.begin(9600); // Инициализация монитора порта
}

void loop() {
  float voltage = readVcc(); // Читаем напряжение
  Serial.print("VCC: ");
  Serial.print(voltage);     // Выводим в вольтах
  Serial.println(" V");
  delay(1000);               // Пауза 1 секунда
}

// Функция измерения питания
float readVcc() {
  // 1. Настройка мультиплексора под конкретный тип микроконтроллера
  #if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega32U4__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2);
  #endif

  delay(2); // 2. Даем источнику опорного напряжения стабилизироваться

  ADCSRA |= _BV(ADSC); // 3. Запуск аналого-цифрового преобразования
  while (bit_is_set(ADCSRA, ADSC)); // 4. Ожидание окончания оцифровки

  // 5. Чтение результатов из регистров
  long result = ADCL;
  result |= (ADCH << 8);

  // 6. Вычисление напряжения по формуле и перевод в вольты
  float voltage = 1125350.0 / (float)result / 1000.0;
  return voltage;
}
