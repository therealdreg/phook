/*******************************************************************************
 
    Project Name:       phook
    Project Version:    1.0.1
    File Name:          owns.c
    
    Revised:            09/06/08
 
    Description: -
    
    Project URL: http://www.fr33project.org/
    
    License: GPL
 
    Copyright (C) 2007    
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

/**************************************************/
/*  ACORDARSE DE MARCAR LOS PARAMETROS EN EL .DEF */
/**************************************************/

/*
En el .def hay que indicarle el numero de argumentos después del nombre:
        Alias=Nombre@arguments * 4 @ Ordinal

NO SE PUEDE USAR AQUÍ -fomit-frame-pointer
*/

#include "common.h"

DLLEXPORT 
HANDLE _stdcall _CreateFileW
(
    LPCWSTR lpFileName,	
    DWORD dwDesiredAccess, 
    DWORD dwShareMode, 
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,	
    DWORD dwCreationDistribution, 
    DWORD dwFlagsAndAttributes, 
    HANDLE hTemplateFile 
)
{
    char   asc_str[MAX_PATH];
    
    if ( UnicodeToANSI( (WCHAR *) lpFileName, asc_str ) == 0 )
        CreateFileLogger( asc_str );
    
    // Seguimos el curso normal de la API
    return CreateFileW( 
        lpFileName, 
        dwDesiredAccess, 
        dwShareMode, 
        lpSecurityAttributes, 
        dwCreationDistribution, 
        dwFlagsAndAttributes, 
        hTemplateFile );
}

DLLEXPORT 
HANDLE _stdcall _CreateFileA
(
    LPCSTR lpFileName,	
    DWORD dwDesiredAccess, 
    DWORD dwShareMode, 
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,	
    DWORD dwCreationDistribution, 
    DWORD dwFlagsAndAttributes, 
    HANDLE hTemplateFile 
)
{
    char   asc_str[MAX_PATH];
    
    CreateFileLogger( lpFileName );
    
    // Seguimos el curso normal de la API
    return CreateFileA( 
        lpFileName, 
        dwDesiredAccess, 
        dwShareMode, 
        lpSecurityAttributes, 
        dwCreationDistribution, 
        dwFlagsAndAttributes, 
        hTemplateFile );
}

static int
UnicodeToANSI( WCHAR * unicod_string, char * ansi_dst )
{
    int size;
    int i;
    
    size = lstrlenW( unicod_string );
    
    if ( ( size >= MAX_PATH ) || ( size == 0 ) )
        return -1;

    for ( i = 0; i < size; i++ )
        ansi_dst[i] = (char) unicod_string[i];
    
    ansi_dst[i] = 0;
    
    return 0;
}

static void 
CreateFileLogger( const char * file_to_log )
{
    // Escribimos la entrada en un fichero
    HANDLE file;
    DWORD  chars;    
    
    file = \
        CreateFileA
        ( 
            FILE_LOG                            , 
            GENERIC_WRITE    | GENERIC_READ     , 
            0                                   , 
            NULL                                , 
            OPEN_ALWAYS                         , 
            0                                   , 
            NULL 
        );
        
    if ( file != INVALID_HANDLE_VALUE )
    {        
        if ( SetFilePointer( file, 0, NULL, FILE_END ) != -1 )
        {
            WriteFile( file, file_to_log, strlen( file_to_log ), &chars, NULL );
            WriteFile( file, "\x0D\x0A", 2, &chars, NULL );
        }
        CloseHandle( file );         
    }    
}
