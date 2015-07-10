/*******************************************************************************
 
    Project Name:       phook
    Project Version:    1.0.1
    File Name:          injectordll.h
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

#ifndef _INJECTOR_DLL_H__
#define _INJECTOR_DLL_H__

// Definiciones estandard
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

// Definiciones comunes
#include "ui.h"
#include "..\common\includes\asm2c.h"


////////////////////////////////////////////////////////////////////////////////
// 
//      DEFINICIONES GENERICAS DEL PROYECTO
//
#define VERSION       "1.0.1"
#define MIN_ARGUMENTS 4

#define _ERROR__      -1
#define _SUCCESS__     0

#define _GUI_APP__   0 // GUI     APP
#define _CONS_APP__  1 // Console APP
#define _UNK_APP__   2 // Unkown  APP

#define BUFFER_SIZE     0x400

#define TIME_OUT_CHECK_SUSPEND_THREAD 1000
#define TIME_OUT_SUSPEND              10

#define ARG_NR_DLL_TO_INJ  1
#define ARG_NR_TYPE_APP    2
#define ARG_NR_PROG_NAME   3
#define ARG_NR_PROC_PID    3

#define MAX_TRYS_CHECK_SUSP 10
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//      DEFINICIONES PARA EL CODIGO EN ASM
//

// Desplazamientos desde LOADER_DLL hasta los campos de DATA:
#define DISTANCE_TO_FLAG       ( sizeof( LOADER_DLL_t ) - sizeof( char ) )
#define DISTANCE_TO_NAME_DLL   ( DISTANCE_TO_FLAG - MAX_PATH ) 

// Desplazamientos desde el LOADER_DLL hasta los saltos necesitados
#define DISTANCE_TO_FIRST_RETURN \
    ( sizeof( PUSH_ASM_t ) + sizeof( CALL_ASM_t ) ) 

#define DISTANCE_TO_SECOND_RETURN \
    ( DISTANCE_TO_FIRST_RETURN + sizeof( CALL_ASM_t ) )

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//      LISTA DE ESTRUCTURAS CODIFICADAS PARA CODIGO EN ASM
//

// Estructura principal de carga de DLLs
typedef struct LOADER_DLL_s
{
    /* CODE */
    PUSH_ASM_t      push_name_dll;           /* PUSH "DLL_INJECT.DLL"  */
    CALL_ASM_t      call_load_library;       /* CALL  LoadLibraryA     */
    /* first_return: */
    CALL_ASM_t      call_get_current_thread; /* CALL  GetCurrentThread */
    /* second_return: */
    INC_BYTE_MEM_t  inc_flag;                /* INC [FLAG]             */
    char            PUSH_EAX;                /* PUSH EAX               */
    CALL_ASM_t      call_suspendthread;      /* CALL SuspendThread     */
    
    /* DATA */
    char            name_dll[MAX_PATH];      /* DLL_INJECT.DLL'\0'     */
    char            flag;                    /* FLAG DE CONTROL        */ 
      
} LOADER_DLL_t;
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//      DEFINICION DE LAS FUNCIONES DEL PROYECTO
//
int  InjectorDll            ( const char *, const char *, const char *, DWORD );
int  InjectDll              ( HANDLE      , const char * );
int  CreateLoaderDLLStruct  ( LOADER_DLL_t *, unsigned long, const char * );
int  GetTypeOfApp           ( LPTSTR );
////////////////////////////////////////////////////////////////////////////////


#endif /* _INJECTOR_DLL_H__ */
