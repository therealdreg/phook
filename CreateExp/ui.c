/*******************************************************************************
 
    Project Name:       phook
    Project Version:    1.0.1
    File Name:          ui.c
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

#include "ui.h"

/*
    Función: ShowGetLastErrorString
    Descripción: Muestra de forma detallada la información acerca del último
                 error devuelto por GetLastError().
    Parametros: -
    Retorno: -
*/
void 
ShowGetLastErrorString( const char * function )
{
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD  NRLastError;
    
    NRLastError = GetLastError();

    printf( " Error in funcion: %s\n", function );
    printf( " GetLastError: %d\n", NRLastError );
    
    if 
    ( 
        FormatMessage
        (
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM ,
            NULL                                                        ,
            NRLastError                                                 ,
            MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT )                 ,
            (LPTSTR) & lpMsgBuf                                         ,
            0                                                           , 
            NULL 
        )
        !=
        0
    )
    {
        printf( " %s", lpMsgBuf );
        LocalFree( lpMsgBuf );
    }
}

void 
ShowProgramHeader( const char * version )
{
    printf
    (
        "  ____________________________________________ \n"
        " |                 CreateExp v%s           |\n"
        " |                                            |\n"
        " | Juan Carlos Montes   eunimedes@hotmail.com |\n"
        " | David Reguera Garcia Dreg@fr33project.org  |\n"
        " | ------------------------------------------ |\n"
        " |       http://www.fr33project.org           |\n"
        " |____________________________________________|\n"
        "\n"
        ,
        
        version
    );
}

