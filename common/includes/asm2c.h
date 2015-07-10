/*******************************************************************************
 
    Project Name:       phook
    Project Version:    1.0.1
    File Name:          asm2c.h
    Revised:            09/06/08
 
    Description: 
        Cabecera donde se almacenan todos los datos necesarios para poder
        construir estructuras de ejecucion de codigo en asm directamente
        para poder realizar escrituras de memoria en procesos remotos.
    
    Project URL: http://www.fr33project.org/
 
    License: GPL
 
    Copyright (C) 2008    
        David Reguera Garcia      -  Dreg@fr33project.org 
        Juan Carlos Montes Senra  -  eunimedes@hotmail.com
 
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 
    You should have received a copy of the GNU General Public License
    along with this program (gpl.txt); if not, write to the Free Software
 
*******************************************************************************/

#ifndef _ASM2C_H__
#define _ASM2C_H__

////////////////////////////////////////////////////////////////////////////////
//
//      DEFINICIONES DE LOS OPCODEs USADOS
//
#define PUSH_OPCODE 0x68
#define RET_OPCODE  0xC3
#define CALL_OPCODE 0xE8

#define PUSH_EAX_OPCODE 0x50

// JMP EAX
#define JMP_EAX_FIRST_BYTE  0xFF
#define JMP_EAX_SECOND_BYTE 0xE0

// INC MEM
#define INC_MEM_FIRST_BYTE  0xFE
#define INC_MEM_SECOND_BYTE 0x05

////////////////////////////////////////////////////////////////////////////////
//
//      LISTA DE ESTRUCTURAS CODIFICADAS PARA CODIGO EN ASM
//
#pragma pack( 1 )

// PUSH 0xXXXXXXXX
typedef struct PUSH_ASM_s
{
    char             push_opcode ;
    unsigned long    data_push   ;
    
} PUSH_ASM_t;

// JMP 0xXXXXXXXX
typedef struct JMP_ASM_s
{
    PUSH_ASM_t push_address;
    char       ret_opcode;
    
} JMP_ASM_t;

// CALL 0xXXXXXXXX
typedef struct CALL_ASM_s
{
    PUSH_ASM_t address_to_return ; 
    JMP_ASM_t  address_to_call   ;
    
} CALL_ASM_t;

// JMP EAX
typedef struct JMP_EAX_ASM_s
{
    char first_byte;
    char second_byte;
    
} JMP_EAX_ASM_t;

// INC BYTE PTR [MEM]
typedef struct INC_BYTE_MEM_s
{
    char          first_byte;
    char          second_byte;
    unsigned long address;
    
} INC_BYTE_MEM_t;

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//      LISTA DE FUNCIONES DE CARGA DE LOS DATOS EN LAS ESTRUCTURAS DE ASM
//

void CreateJMP    ( JMP_ASM_t *, unsigned long );
void CreatePush   ( PUSH_ASM_t *, unsigned long );
void CreateCALL   ( CALL_ASM_t *, unsigned long, unsigned long );
void CreateJMPEAX ( JMP_EAX_ASM_t * );
void CreateINCMEM ( INC_BYTE_MEM_t *, unsigned long );
////////////////////////////////////////////////////////////////////////////////


#endif      // _ASM2C_H__
