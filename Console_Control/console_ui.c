/*******************************************************************************
 
    Project Name:       phook
    Project Version:    1.0.1
    File Name:          console_ui.c
    
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

#include "console_ui.h"


/*
    Función: ShowConsoleHeader
    Descripción: 
        Funcion que envia la cabecera de la consola al cliente conectado.
    Parametros: 
        sock           - Socket conectado al cliente
    Retorno: 
        -
*/
void 
ShowConsoleHeader( int sock )
{
    // Preparamos loas variables locales
    char * header;
    header = calloc( SIZE_OF_BUFFER, sizeof(char) );
    
    // Enviamos el buffer formateado a nuestra variable
    sprintf
    (
        header, 
        "  ________________________________________________________ \n"
        " |                   Phook Prompt v%s                  |\n"
        " |       Juan Carlos Montes   eunimedes@hotmail.com       |\n"
        " |       David Reguera Garcia Dreg@fr33project.org        |\n"
        " | ------------------------------------------------------ |\n"
        " |            http://www.fr33project.org                  |\n"
        " |________________________________________________________|\n"
        "\n"
        ,
        
        CONSOLE_VERSION
    );
    
    // Enviamos la pantalla 
    RemotePrint( sock, header );
    
    // Liberamos la memoria pedida
    free( header );
}


/*
    Función: ShowConsoleHelp
    Descripción: 
        Funcion que envia la ayuda de la consola al cliente conectado.
    Parametros: 
        sock           - Socket conectado al cliente
    Retorno: 
        -
*/
void 
ShowConsoleHelp( int sock )
{
    // Preparamos loas variables locales
    char * header;
    header = calloc( SIZE_OF_BUFFER, sizeof(char) );
    
    if ( header == NULL )
    {
        RemotePrint
        ( 
            sock, 
            "-> Error: asignating memory dinamically for help buffer.\n"
        );
        
        return;
    }
    
    // Enviamos el buffer formateado a nuestra variable
    sprintf
    (
        header, 
        
        "  _________________________________________________________________ \n"
        " |                       Phook Prompt v%s                       |\n"
        " |                                                                 |\n"
        " | Command list:                                                   |\n"
        " | --------------------------------------------------------------- |\n"
        " | help                      - Shows this screen                   |\n"
        " | exit                      - Closes and unloads the console      |\n"
        " | suspend                   - Pauses the programs execution       |\n"
        " | resume                    - Resumes the programs execution      |\n"                
        " | showmodules               - Shows the modules list              |\n"                        
        " | load [param1]             - Loads in memory the library         |\n"
        " |                             especified in [param1]              |\n"
        " | unload [param1]           - Unloads a librery in memory         |\n"                        
        " |                             especified in [param1]              |\n"        
        " | pebhook [param1] [param2] - Performs PEB Hook over a dll        |\n"        
        " |                             [param1]: Name of the original dll  |\n"                                        
        " |                             [param2]: Path to the DLL hook      |\n"                                                
        " |_________________________________________________________________|\n"
        "\n",
        
        CONSOLE_VERSION
    );
    
    // Enviamos la pantalla 
    RemotePrint( sock, header );
    
    // Liberamos la memoria pedida
    free( header );
}

/*
    Función: ShowGetLastErrorString
    Descripción: 
        Funcion que muestra el ultimo error en GetLastError
    Parametros: 
        function      - Nombre de la funcion desde la que se llama 
                        a esta funcion.
    Retorno: 
        -
*/
void 
ShowGetLastErrorString( const char * function )
{
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD  NRLastError;
    
    NRLastError = GetLastError();

    printf( " Error in the function: %s\n", function );
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

/*
    Función: RemotePrint
    Descripción: 
        Funcion de envio de cadenas por sockets.
    Parametros: 
        sock          - Socket del destinatario
        buff          - Buffer a enviar
    Retorno: 
        -
*/
void
RemotePrint( int sock, const char * buff)
{
    send( sock, buff, strlen( buff ) + 1, 0 );
}


