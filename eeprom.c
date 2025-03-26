/*
 * eeprom.c
 *
 * Copy date: 20.03.2025 20:32:28
 *  Author: https://github.com/AntonNeutron
 */ 
/*

  eeprom.c

  Based on AVR101 "High endurance EEPROM storage" by jllassen

  Copyright 2025 AntonNeutron. All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY MATTHEW T. PANDINA "AS IS" AND ANY
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MATTHEW T. PANDINA OR
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
  OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
  SUCH DAMAGE.

*/
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "eeprom.h"

typedef struct {
	uint8_t element_size;  // Размер данных (без учета счетчика)
	uint8_t buffer_count;  // Количество элементов в буфере
	uint16_t addr;         // Начальный адрес в EEPROM
} param_eeprom_t;


// Структура записи в буфере
typedef struct {
	uint16_t adr_eeprom;	// Адрес EEPROM куда записываем данные
	uint8_t newStatus;	// новый статус записи
	const uint8_t *data_ptr; // Указатель на данные
	uint8_t data_size;	// Размер данных
} buffer_record_t;

volatile buffer_record_t eeprom_writebuffer[MAX_WRITE_BUFFER_SIZE];
volatile uint8_t eeprom_writebuffer_head = 0;
volatile uint8_t eeprom_writebuffer_tail = 0;
// Флаг, указывающий, что запись EEPROM в процессе
volatile uint8_t eeprom_busy_flag = 0;
// Статическая переменная для отслеживания позиции в текущем блоке записи
// Значение 0 означает, что для нового блока еще не был записан newStatus.
// После записи newStatus current_byte_index становится 1.
static volatile uint8_t current_byte_index = 0;

/* ************************************ скрипт на питоне генерирует код для переменных  ******************************

# Размер EEPROM
EEPROM_SIZE = 1000
EEPROM_START_ADR = 100  # Начальный адрес

# Таблица данных (name_param, тип, количество элементов)
params = [
{"name_param": "EE_LCD_LIGHT", "type": "uint8_t", "count": 5},
{"name_param": "EE_BAT_MIN_V", "type": "uint16_t", "count": 100},
# Добавьте дополнительные параметры по необходимости
]

# Вывод описания блоков параметров
print("// Наименование используемых параметров. Необходимо перенести в .h файл")
print("enum {")
for param in params:
	print(f"\t{param['name_param']},")
print("};")

# Вывод заголовков
print("\n// Необходимо заполнить и при необходимости добавить следующую структуру, так же для заполнения таблицы можно воспользоваться скриптом питона")
print("enum {")
print(f"\tEEPROM_SIZE = {EEPROM_SIZE},")
print(f"\tEEPROM_START_ADR = {EEPROM_START_ADR},")

	# Подсчитываем параметры и генерируем код для каждого из них
for i, param in enumerate(params):
	# Генерируем код для каждого блока
	print(f"\n\t{param['name_param']}_SIZE = sizeof({param['type']}),")
	print(f"\t{param['name_param']}_COUNT = {param['count']},")
	
	# Проверяем, если i > 0, используем адрес конца предыдущего блока, иначе начальный адрес
	if i > 0:
		print(f"\t{param['name_param']}_ADDR = {params[i-1]['name_param']}_END,")
	else:
		print(f"\t{param['name_param']}_ADDR = EEPROM_START_ADR,")
	
	# Формула для расчёта конца блока
	print(f"\t{param['name_param']}_END = {param['name_param']}_ADDR + ({param['name_param']}_SIZE + 1) * {param['name_param']}_COUNT,")  # Плюс 1 для учета кольцевого буфера

	# Выводим проверку переполнения EEPROM
print("\n};")
print("\n// Проверяем выход за границы EEPROM (если здесь компилятор выдает ошибку значит блоки переменных не помещаются в EEPROM)")
print("// !!!!!!!!!!!!!!            Не забываем указать индекс последней переменной          !!!!!!!!!!!!!!!!!!!!!!")
print(f"extern uint8_t error_eeprom_overflow[{params[-1]['name_param']}_END > EEPROM_SIZE ? -1 : 0];")

# Печатаем описание блока данных
print("\nconst param_eeprom_t param_eeprom[] PROGMEM = {")
for i, param in enumerate(params):
	# Добавляем комментарий с индексом и наименованием параметра
	print(f"\t{{{param['name_param']}_SIZE, {param['name_param']}_COUNT, {param['name_param']}_ADDR}},  //  Индекс {i}, {param['name_param']}, тип {param['type']}, \tколичество элементов {param['count']}")
print("};")


*/

/******************************************************************************************************************************************************************/
/*                                                                  Код полученный из питона                                                                      */
/******************************************************************************************************************************************************************/



// Необходимо заполнить и при необходимости добавить следующую структуру, так же для заполнения таблицы можно воспользоваться скриптом питона
enum {
	EEPROM_SIZE = 1000,
	EEPROM_START_ADR = 00,

	EE_LCD_LIGHT_SIZE = sizeof(uint8_t),
	EE_LCD_LIGHT_COUNT = 5,
	EE_LCD_LIGHT_ADDR = EEPROM_START_ADR,
	EE_LCD_LIGHT_END = EE_LCD_LIGHT_ADDR + (EE_LCD_LIGHT_SIZE + 1) * EE_LCD_LIGHT_COUNT,

	EE_BAT_MIN_V_SIZE = sizeof(uint16_t),
	EE_BAT_MIN_V_COUNT = 3,
	EE_BAT_MIN_V_ADDR = EE_LCD_LIGHT_END,
	EE_BAT_MIN_V_END = EE_BAT_MIN_V_ADDR + (EE_BAT_MIN_V_SIZE + 1) * EE_BAT_MIN_V_COUNT,

};

// Проверяем выход за границы EEPROM (если здесь компилятор выдает ошибку значит блоки переменных не помещаются в EEPROM)
// !!!!!!!!!!!!!!            Не забываем указать индекс последней переменной          !!!!!!!!!!!!!!!!!!!!!!
extern uint8_t error_eeprom_overflow[EE_BAT_MIN_V_END > EEPROM_SIZE ? -1 : 0];

const param_eeprom_t param_eeprom[] PROGMEM = {
	{EE_LCD_LIGHT_SIZE, EE_LCD_LIGHT_COUNT, EE_LCD_LIGHT_ADDR},  //  Индекс 0, EE_LCD_LIGHT, тип uint8_t, 	количество элементов 5
	{EE_BAT_MIN_V_SIZE, EE_BAT_MIN_V_COUNT, EE_BAT_MIN_V_ADDR},  //  Индекс 1, EE_BAT_MIN_V, тип uint16_t, 	количество элементов 100
};


/******************************************************************************************************************************************************************/



#include <avr/eeprom.h>
#define EEPROM_Read(address) eeprom_read_byte((uint8_t *)(uint16_t)(address))
#define EEPROM_Write(address, data) eeprom_update_byte((uint8_t *)(uint16_t)(address), (data))
#define EEPROM_Read_Block(address,size) eeprom_read_byte((uint8_t *)(uint16_t)(address))
#define EEPROM_Write_Block(address, data) eeprom_update_byte((uint8_t *)(uint16_t)(address), (data))


// Чтение параметров переменной из флеш-памяти по индексу или имени переменной (3us)
void EEPROM_ReadParam(uint8_t index, param_eeprom_t *param) {
	param->element_size = pgm_read_byte(&(param_eeprom[index].element_size));
	param->buffer_count = pgm_read_byte(&(param_eeprom[index].buffer_count));
	param->addr = pgm_read_word(&(param_eeprom[index].addr));
}

static uint16_t EEPROM_FindCurrentAddress(const uint8_t index, param_eeprom_t *param) {
    uint8_t status, pred_status;
    // Заводим отдельную переменную для текущего адреса, чтобы не менять param.addr
    uint16_t current_addr = param->addr;
    uint16_t EeBufEnd = param->addr + (param->buffer_count - 1) * (param->element_size + 1);
    
    pred_status = EEPROM_Read(current_addr);
    while (current_addr != EeBufEnd) {
        current_addr += param->element_size + 1;
        status = EEPROM_Read(current_addr);
        
        if (status != (uint8_t)(pred_status + 1)) {
            current_addr -= (param->element_size + 1);
            break;
        }
        pred_status = status;
    }
    
    // Возвращаем адрес следующего байта после статуса последнего корректного элемента
    return current_addr + 1;
}

volatile uint8_t Aaaaa;

uint8_t EEPROM_ReadWearLeveledByte(const uint8_t index) {
	param_eeprom_t param;
	 // Чтение данных о параметре из flash
	EEPROM_ReadParam(index, &param);  // index — это индекс в массиве param_eeprom
	if (param.element_size != 1) return 0;
	uint16_t address = EEPROM_FindCurrentAddress(index, &param);
	return eeprom_read_byte((const uint8_t*)(uint16_t)address);
}

uint16_t EEPROM_ReadWearLeveledWord(const uint8_t index) {
	param_eeprom_t param;
	// Чтение данных о параметре из flash
	EEPROM_ReadParam(index, &param);  // index — это индекс в массиве param_eeprom
	if (param.element_size != 2) return 0;
	uint16_t address = EEPROM_FindCurrentAddress(index, &param);
	return eeprom_read_word((const uint16_t*)address);
}

// Читаем парамерт из EEPROM количество считанных байт выбирается минимальным
void EEPROM_ReadWearLeveledBlock(const uint8_t index, void * ptr, uint8_t size) {
	param_eeprom_t param;
	
	// Чтение данных о параметре из flash
	EEPROM_ReadParam(index, &param);  // index — это индекс в массиве param_eeprom
	uint16_t address = EEPROM_FindCurrentAddress(index, &param);
	if ( size > param.element_size )
	size = param.element_size;
	eeprom_read_block(ptr,(void*)address,size);
}


uint8_t EEPROM_CompareData(uint16_t eeprom_addr, const void *data, uint8_t size) {
	for (uint8_t i = 0; i < size; i++) {
		if (eeprom_read_byte((const uint8_t *)(eeprom_addr + i)) != ((const uint8_t *)data)[i]) {
			return 0; // есть различия
		}
	}
	return 1; // данные идентичны
}

void eeprom_writebuffer_add(const uint16_t *addr, const uint8_t *newStatus, const uint8_t *data, const uint8_t *data_size);

void EEPROM_WriteWearLeveled(const uint8_t index, const void * data) {
  param_eeprom_t param;
  
  // Чтение данных о параметре из flash
  EEPROM_ReadParam(index, &param);  // index — это индекс в массиве param_eeprom
  uint16_t address = EEPROM_FindCurrentAddress(index, &param) -1; // -1 мы получаем адрес начало блока, а не записанных данных

  // Only perform the write if the new value is different from what's currently stored
  if (EEPROM_CompareData(address+1,data,param.element_size))  return;


  // Store the old status value
  uint8_t newStatusValue = EEPROM_Read(address) + 1;

  // Move pointer to the next element in the buffer, wrapping around if necessary
  address += param.element_size + 1;
  // получаем адрес следующего за пределами блока
  uint16_t EeBufOffBlock = param.addr + param.buffer_count * (param.element_size + 1); 
  if (address == EeBufOffBlock) // если полученный адрес равен адресу за пределами блока, то какого быть не может и мы меняем его на начальный
    address = param.addr;

  // If self-programming is used in the application, insert code here
  // to wait for any self-programming operations to finish before
  // writing to the EEPROM.

  // Update the param buffer in the EEPROM
 // EEPROM_Write(address, data);

  // Update the status buffer in the EEPROM
 // EEPROM_Write(address + EE_PARAM_BUFFER_SIZE, oldStatusValue + 1);
 eeprom_writebuffer_add(&address,&newStatusValue, data, &param.element_size);
}

// Функция для добавления записи в буфер
void eeprom_writebuffer_add(const uint16_t *addr, const uint8_t *newStatus, const uint8_t *data, const uint8_t *data_size) {
	// Проверка заполненности буфера
	
	if (eeprom_busy_flag) return;
	
	uint8_t next_head = (eeprom_writebuffer_head + 1) % MAX_WRITE_BUFFER_SIZE;
	if (next_head == eeprom_writebuffer_tail) {
		// Буфер переполнен, можно выполнить обработку ошибки
		return;
	}

	// Заполняем запись в буфере
	eeprom_writebuffer[eeprom_writebuffer_head].adr_eeprom = *addr;
	eeprom_writebuffer[eeprom_writebuffer_head].newStatus = *newStatus;
	eeprom_writebuffer[eeprom_writebuffer_head].data_ptr = data;
	eeprom_writebuffer[eeprom_writebuffer_head].data_size = *data_size;

	// Сдвигаем указатель головы буфера
	eeprom_writebuffer_head = next_head;

	// Если запись не занята и буфер не пуст (первый элемент)

}

void StartWriteBuffer (void){
		// Запускаем запись 
	if (!eeprom_busy_flag) {
		eeprom_busy_flag = 1;
		//current_byte_index = 1;  // После newStatus переходим к данным
		EECR |= (1 << EERIE);
	}
}

// Вектор прерывания "EEPROM Ready" для Atmega128

ISR(EE_READY_vect){
    static volatile const buffer_record_t *record = NULL;
	
    if (record == NULL || current_byte_index >= (record->data_size + 1)) {
	    if (eeprom_writebuffer_tail == eeprom_writebuffer_head) {
		    EECR &= ~(1 << EERIE); // Отключаем прерывание
			record = NULL;
		    eeprom_busy_flag = 0;
		    return;
	    }

	    record = &eeprom_writebuffer[eeprom_writebuffer_tail];
	    current_byte_index = 0;
	    eeprom_writebuffer_tail = (eeprom_writebuffer_tail + 1) % MAX_WRITE_BUFFER_SIZE;
    }

    EEAR = record->adr_eeprom + current_byte_index;
    EEDR = (current_byte_index == 0) ? record->newStatus : record->data_ptr[current_byte_index - 1];

    EECR |= (1 << EEMWE);
    EECR |= (1 << EEWE);
    
    current_byte_index++;
    }
