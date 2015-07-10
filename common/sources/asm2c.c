/*******************************************************************************
 
    Project Name:       phook
    Project Version:    1.0.1
    File Name:          asm2c.c
    Revised:            09/06/08
 
    Description: -
    
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


#include "../includes/asm2c.h"

/*
    Función: CreateJMP
    Descripción: Carga de los datos de la estructura JMP_ASM_t
    Parametros: 
        jmp_asm             - Estructura a rellenar         
        address_to_jmp      - Direccion de salto 
    Retorno: -           
*/
void 
CreateJMP( JMP_ASM_t * jmp_asm, unsigned long address_to_jmp )
{
    jmp_asm->push_address.push_opcode = PUSH_OPCODE;
    jmp_asm->push_address.data_push   = address_to_jmp;
    jmp_asm->ret_opcode               = RET_OPCODE;
}


/*
    Función: CreatePush
    Descripción: Carga de los datos de la estructura PUSH_ASM_t
    Parametros: 
        push                - Estructura a rellenar         
        value               - Valor que se desea insertar en la pila 
    Retorno: -           
*/
void 
CreatePush( PUSH_ASM_t * push, unsigned long value )
{
    push->push_opcode = PUSH_OPCODE;
    push->data_push   = value;
}


/*
    Función: CreateCALL
    Descripción: Carga de los datos de la estructura CALL_ASM_t
    Parametros: 
        call                - Estructura a rellenar         
        address_to_return   - Direccion de retorno despues de la funcion 
        address_to_call     - Direccion de la funcion a llamar
    Retorno: -           
*/
void 
CreateCALL
( 
    CALL_ASM_t    * call               , 
    unsigned long   address_to_return  , 
    unsigned long   address_to_call  
)
{
    call->address_to_return.push_opcode            = PUSH_OPCODE       ;
    call->address_to_return.data_push              = address_to_return ;
    call->address_to_call.push_address.push_opcode = PUSH_OPCODE       ;
    call->address_to_call.push_address.data_push   = address_to_call   ;
    call->address_to_call.ret_opcode               = RET_OPCODE   ;
}


/*
    Función: CreateJMPEAX
    Descripción: Carga de los datos de la estructura JMP_EAX_ASM_t
    Parametros: 
        jmp_eax                - Estructura a rellenar         
    Retorno: -           
*/
void 
CreateJMPEAX( JMP_EAX_ASM_t * jmp_eax )
{
    jmp_eax->first_byte  = JMP_EAX_FIRST_BYTE;
    jmp_eax->second_byte = JMP_EAX_SECOND_BYTE;
}

void CreateINCMEM( INC_BYTE_MEM_t * inc_byte_mem, unsigned long address )
{
    inc_byte_mem->first_byte  = INC_MEM_FIRST_BYTE;
    inc_byte_mem->second_byte = INC_MEM_SECOND_BYTE;
    inc_byte_mem->address     = address;
}
////////////////////////////////////////////////////////////////////////////////
