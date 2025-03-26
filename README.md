# EEPROM Wear Leveling
Библиотека для записи данных в EEPROM с учетом износа (wear leveling) на AVR.

Библиотека для управления данными в EEPROM микроконтроллеров AVR с учетом износа памяти.  
Позволяет равномерно распределять циклы записи, увеличивая срок службы EEPROM,  
а также предоставляет удобный буфер для асинхронной записи.

## Основные возможности:
- Запись данных в EEPROM с учетом износа (wear leveling)
- Буферизация записей для асинхронного выполнения
- Поддержка чтения и записи переменных разного размера
- Минимизация количества перезаписываемых ячеек

Подходит для микроконтроллеров AVR (Atmega, Attiny)
Требует стандартную библиотеку AVR-GCC

## Использование:

### **1. Определение таблицы параметров**
Перед использованием необходимо создать **таблицу параметров**, в которой указываются:
- **Размер переменной** (в байтах)
- **Уровень wear leveling** (количество копий переменной в EEPROM)
- **Начальный адрес блока хранения**  

Пример таблицы:

typedef struct {
	uint8_t element_size;  // Размер данных (без учета счетчика)
	uint8_t buffer_count;  // Количество элементов в буфере
	uint16_t addr;         // Начальный адрес в EEPROM
} param_eeprom_t;

const param_eeprom_t param_eeprom[] PROGMEM = {
	{EE_LCD_LIGHT_SIZE, EE_LCD_LIGHT_COUNT, EE_LCD_LIGHT_ADDR},  //  Индекс 0, EE_LCD_LIGHT, тип uint8_t, 	количество элементов 5
	{EE_BAT_MIN_V_SIZE, EE_BAT_MIN_V_COUNT, EE_BAT_MIN_V_ADDR},  //  Индекс 1, EE_BAT_MIN_V, тип uint16_t, 	количество элементов 100
};

"Для удобства таблицу параметров можно сформировать с помощью скрипта на Python.

### **2. Запись переменной с учетом износа**
EEPROM_WriteWearLeveled(index, &value);

### **3. Асинхронное начало записи из буфера**
StartWriteBuffer();

### **4. Чтение переменной из EEPROM**
uint8_t val = EEPROM_ReadWearLeveledByte(index);
uint16_t word = EEPROM_ReadWearLeveledWord(index);
EEPROM_WriteWearLeveled(index, &data);

#######################################################################################################################

EEPROM Wear Leveling
A library for writing data to EEPROM with wear leveling on AVR microcontrollers.

This library manages EEPROM data on AVR microcontrollers while considering memory wear.
It evenly distributes write cycles, extending EEPROM lifespan,
and provides a convenient buffer for asynchronous writing.

Features:
Wear leveling for EEPROM data storage

Buffered writes for asynchronous execution

Support for reading and writing variables of different sizes

Minimization of overwritten cells

Compatible with AVR microcontrollers (Atmega, Attiny)
Requires the standard AVR-GCC library

Usage:
## **1. Defining the Parameter Table**
Before use, you need to create a parameter table, which specifies:

Variable size (in bytes)
Wear leveling level (number of stored copies in EEPROM)
Starting address of the storage block

Example table:

typedef struct {  
    uint8_t element_size;  // Data size (excluding the counter)  
    uint8_t buffer_count;  // Number of buffer elements  
    uint16_t addr;         // Starting address in EEPROM  
} param_eeprom_t;  

const param_eeprom_t param_eeprom[] PROGMEM = {  
    {EE_LCD_LIGHT_SIZE, EE_LCD_LIGHT_COUNT, EE_LCD_LIGHT_ADDR},  // Index 0, EE_LCD_LIGHT, type uint8_t, 5 copies  
    {EE_BAT_MIN_V_SIZE, EE_BAT_MIN_V_COUNT, EE_BAT_MIN_V_ADDR},  // Index 1, EE_BAT_MIN_V, type uint16_t, 100 copies  
};  
For convenience, the parameter table can be generated using a Python script.

### **2. Writing a Variable with Wear Leveling**

EEPROM_WriteWearLeveled(index, &value);

### **3. Starting Asynchronous Write from the Buffer**

StartWriteBuffer();

### **4. Reading a Variable from EEPROM**

uint8_t val = EEPROM_ReadWearLeveledByte(index);
uint16_t word = EEPROM_ReadWearLeveledWord(index);
EEPROM_WriteWearLeveled(index, &data);
