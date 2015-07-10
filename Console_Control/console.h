/*******************************************************************************
 
    Project Name:       phook
    Project Version:    1.0.1
    File Name:          console.h
    
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

#ifndef _CONSOLE_H__
#define _CONSOLE_H__


// INCLUDES NECESARIAS
#include <windows.h>
#include <winbase.h>
#include <stdio.h>
#include <winsock2.h>

#include "threads.h"

// MACROS
#define IS_EQUALi( string1, string2 )     ( strcmpi( string1, string2 ) == 0 )
#define IS_NOT_EQUALi( string1, string2 ) ( strcmpi( string1, string2 ) != 0 )

// DEFINICIONES DE BUFFERS Y VERSIONES
#define SIZE_OF_BUFFER          0x1000
#define CONSOLE_VERSION         "1.0.1"
#define CONSOLE_NAME            "console.dll"
#define KERNEL32_NAME           "kernel32.dll"

#define _SUCCESS__               0
#define _ERROR__                -1

// DEFINICIONES FICHERO DE LOG:
#define FILE_LOG "C:\\ph_listen_ports.log"

// DEFINICIONES DE LAS CONEXIONES
#define MAX_CONNECTIONS         1
#define PROMPT                  "\nph >"
#define FIRST_PORT_TO_BIND      1234


// ESTRUCTURAS
typedef struct _COMMAND
{
    char cmd   [SIZE_OF_BUFFER];
    char param1[SIZE_OF_BUFFER];
    char param2[SIZE_OF_BUFFER]; 
    
} COMMAND, *PCOMMAND;


// DEFINICIONES DE LAS FUNCIONES
void Parser                 ( char *, PCOMMAND );
int  ReadSocket             ( int, char *, unsigned long );
int  WaitForConn            ( void );
void LoadConsole            ( void );
void LogPort                ( u_short );

// VARIABLES GLOBALES
HANDLE   MainThread;

#endif          //_CONSOLE_H__
