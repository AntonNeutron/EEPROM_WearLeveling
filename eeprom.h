/*
 * eeprom.h
 *
 * Created: 20.03.2025 20:33:38
 *  Author: AntonNeutron
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

/*

  eeprom.h

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

// Наименование используемых параметров
enum {
	EE_LCD_LIGHT,
	EE_BAT_MIN_V,
};

// Максимальный размер буфера для отложенной записи в EEPROM
#define MAX_WRITE_BUFFER_SIZE 10  

#include <stdint.h>
#include <avr/io.h>

/**
 * @brief Читает один байт из EEPROM с учетом износа памяти.
 *
 * Производит поиск актуального значения параметра и возвращает его.  
 * Если размер параметра по указанному индексу не равен 1 байту, возвращает 0.
 *
 * @param index Индекс параметра в EEPROM.
 * @return Значение (1 байт), считанное из EEPROM, или 0 при ошибке.
 */
uint8_t EEPROM_ReadWearLeveledByte(const uint8_t index);

/**
 * @brief Читает 2 байта (слово) из EEPROM с учетом износа памяти.
 *
 * Производит поиск актуального значения параметра и возвращает его.  
 * Если размер параметра по указанному индексу не равен 2 байтам, возвращает 0.
 *
 * @param index Индекс параметра в EEPROM.
 * @return Значение (2 байта), считанное из EEPROM, или 0 при ошибке.
 */
uint16_t EEPROM_ReadWearLeveledWord(const uint8_t index);

/**
 * @brief Читает данные из EEPROM с учетом износа памяти.  
 *
 * Производит поиск актуального значения переменной по заданному индексу,  
 * считывает ее из EEPROM и записывает в переданный буфер.  
 *
 * @param index Индекс параметра в EEPROM.  
 * @param ptr   Указатель на буфер, куда будет записано считанное значение.  
 * @param size  Размер данных для чтения (в байтах).  
 */
void EEPROM_ReadWearLeveledBlock(const uint8_t index, void *ptr, uint8_t size);

/**
 * @brief Макрос для удобного вызова `EEPROM_ReadWearLeveledBlock()`.  
 *
 * Определяет размер переданной переменной и вызывает функцию чтения EEPROM.  
 *
 * @param index Индекс параметра в EEPROM.  
 * @param ptr   Переменная, в которую будет записано считанное значение.  
 */
#define EEPROM_ReadWearLeveled(index, ptr) \
    EEPROM_ReadWearLeveledBlock(index, (void*)(&ptr), sizeof(ptr))

/**
 * @brief Добавляет данные в буфер записи EEPROM с учетом износа памяти.
 *
 * Данная функция вносит ссылку на переменную в буфер записи. Запись в EEPROM 
 * осуществляется асинхронно после вызова `StartWriteBuffer()`.
 *
 * ВНИМАНИЕ:  
 * - Одновременно нельзя изменять одну и ту же переменную, иначе все изменения 
 *   запишутся в одну ячейку памяти, что приведет к ускоренному износу.  
 * - Максимальное количество переменных в буфере ограничено `MAX_WRITE_BUFFER_SIZE`.  
 *
 * @param index Индекс параметра в EEPROM.
 * @param data  Указатель на данные для записи.
 */
void EEPROM_WriteWearLeveled(const uint8_t index, const void *data);

/**
 * @brief Запускает процесс асинхронной записи в EEPROM.  
 *
 * Данные, добавленные в буфер с помощью `EEPROM_WriteWearLeveled()`, начнут 
 * записываться в EEPROM в фоне.  
 * Вызов этой функции обязателен после внесения данных в буфер.  
 */
void StartWriteBuffer(void);

#endif /* EEPROM_H_ */
