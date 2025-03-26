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

// ������������ ������������ ����������
enum {
	EE_LCD_LIGHT,
	EE_BAT_MIN_V,
};

// ������������ ������ ������ ��� ���������� ������ � EEPROM
#define MAX_WRITE_BUFFER_SIZE 10  

#include <stdint.h>
#include <avr/io.h>

/**
 * @brief ������ ���� ���� �� EEPROM � ������ ������ ������.
 *
 * ���������� ����� ����������� �������� ��������� � ���������� ���.  
 * ���� ������ ��������� �� ���������� ������� �� ����� 1 �����, ���������� 0.
 *
 * @param index ������ ��������� � EEPROM.
 * @return �������� (1 ����), ��������� �� EEPROM, ��� 0 ��� ������.
 */
uint8_t EEPROM_ReadWearLeveledByte(const uint8_t index);

/**
 * @brief ������ 2 ����� (�����) �� EEPROM � ������ ������ ������.
 *
 * ���������� ����� ����������� �������� ��������� � ���������� ���.  
 * ���� ������ ��������� �� ���������� ������� �� ����� 2 ������, ���������� 0.
 *
 * @param index ������ ��������� � EEPROM.
 * @return �������� (2 �����), ��������� �� EEPROM, ��� 0 ��� ������.
 */
uint16_t EEPROM_ReadWearLeveledWord(const uint8_t index);

/**
 * @brief ������ ������ �� EEPROM � ������ ������ ������.  
 *
 * ���������� ����� ����������� �������� ���������� �� ��������� �������,  
 * ��������� �� �� EEPROM � ���������� � ���������� �����.  
 *
 * @param index ������ ��������� � EEPROM.  
 * @param ptr   ��������� �� �����, ���� ����� �������� ��������� ��������.  
 * @param size  ������ ������ ��� ������ (� ������).  
 */
void EEPROM_ReadWearLeveledBlock(const uint8_t index, void *ptr, uint8_t size);

/**
 * @brief ������ ��� �������� ������ `EEPROM_ReadWearLeveledBlock()`.  
 *
 * ���������� ������ ���������� ���������� � �������� ������� ������ EEPROM.  
 *
 * @param index ������ ��������� � EEPROM.  
 * @param ptr   ����������, � ������� ����� �������� ��������� ��������.  
 */
#define EEPROM_ReadWearLeveled(index, ptr) \
    EEPROM_ReadWearLeveledBlock(index, (void*)(&ptr), sizeof(ptr))

/**
 * @brief ��������� ������ � ����� ������ EEPROM � ������ ������ ������.
 *
 * ������ ������� ������ ������ �� ���������� � ����� ������. ������ � EEPROM 
 * �������������� ���������� ����� ������ `StartWriteBuffer()`.
 *
 * ��������:  
 * - ������������ ������ �������� ���� � �� �� ����������, ����� ��� ��������� 
 *   ��������� � ���� ������ ������, ��� �������� � ����������� ������.  
 * - ������������ ���������� ���������� � ������ ���������� `MAX_WRITE_BUFFER_SIZE`.  
 *
 * @param index ������ ��������� � EEPROM.
 * @param data  ��������� �� ������ ��� ������.
 */
void EEPROM_WriteWearLeveled(const uint8_t index, const void *data);

/**
 * @brief ��������� ������� ����������� ������ � EEPROM.  
 *
 * ������, ����������� � ����� � ������� `EEPROM_WriteWearLeveled()`, ������ 
 * ������������ � EEPROM � ����.  
 * ����� ���� ������� ���������� ����� �������� ������ � �����.  
 */
void StartWriteBuffer(void);




#endif /* EEPROM_H_ */