/*******************************************************************************
 
    Project Name:       phook
    Project Version:    1.0.1
    File Name:          console_admin.c
    
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

#include "console_admin.h"


/*
    Función: ConsoleAdmin
    Descripción: 
        Funcion de control para dar funcionalidad a la consola
    Parametros: 
        sock        - Socket del cliente conectado
        cmd         - Comando que se va a procesar
    Retorno: 
        _ERROR__    - Fallo en alguno de los pasos del proceso
        _SUCCESS__  - Funcionamiento correcto del proceso
*/
int
ConsoleAdmin( int sock, PCOMMAND cmd )
{
    // Variables locales
    BOOL cmd_ok             = FALSE;
    int  return_function    = 0;
    
    // CONTROL DE COMANDOS
    if ( IS_EQUALi( cmd->cmd, "load" ) )
    {        
        LoadALibrary( sock, cmd );
        cmd_ok = TRUE;
    }

    else if ( IS_EQUALi( cmd->cmd, "unload" ) )
    {
        FreeALibrary( sock, cmd );
        cmd_ok = TRUE;                
    }            
    
    else if ( IS_EQUALi( cmd->cmd, "showmodules" ) )
    {
        ShowModules( sock );
        cmd_ok = TRUE;                
    }    

    else if ( IS_EQUALi( cmd->cmd, "help" ) )                
    {
        ShowConsoleHelp( sock );                
        cmd_ok = TRUE;
    }
    
    else if ( IS_EQUALi( cmd->cmd, "pebhook" ) )                
    {
        if ( ( strlen( cmd->param1 ) > 0 ) && ( strlen( cmd->param2 ) > 0 ) )    
            PebHooking( sock, cmd->param1, cmd->param2);
        else
            RemotePrint( sock, " [INFO]: Necessary parameters.\n" );        
        cmd_ok = TRUE;
    }                      

    else if ( IS_EQUALi( cmd->cmd, "suspend" ) )                
    {
        if ( MainThread == NULL )        
            RemotePrint( sock, " [INFO]: Command not allowed.\n" );
        else
            SuspendThread( MainThread );             
        cmd_ok = TRUE;
    }                                                                             

    else if ( IS_EQUALi( cmd->cmd, "resume" ) )                
    {
        if ( MainThread == NULL )        
            RemotePrint( sock, " [INFO]: Command not allowed.\n" );
        else
            ResumeThread( MainThread );
        
        
        cmd_ok = TRUE;
    }   

    else if ( IS_EQUALi( cmd->cmd, "exit" ) )
    {
        RemotePrint( sock, " [INFO]: Downloading and closing console.\n" );
        Sleep( 100 );
        return_function = 1;
        cmd_ok = TRUE;
    }
    
    if ( cmd_ok == FALSE)
    {
        RemotePrint( sock, " Unkown command.\n" );        
    }
    
    return return_function;
}


/*
    Función: LoadALibrary
    Descripción: 
        Funcion que controla la carga de una biblioteca
    Parametros: 
        sock        - Socket del cliente conectado
        cmd         - Estructura donde esta la biblioteca a cargar
    Retorno: 
        _ERROR__    - Fallo en alguno de los pasos del proceso
        _SUCCESS__  - Funcionamiento correcto del proceso
*/
int 
LoadALibrary( int sock, PCOMMAND cmd )
{
    HINSTANCE library;
    
    if ( cmd->param1[0] == 0 )
    {
        RemotePrint( sock, " Library to load not especified (param1).\n" );
        return _ERROR__;
    }
    
    library = GetModuleHandle( cmd->param1 );
    
    if ( library != NULL )
    {
        RemotePrint( sock, "  Library already loaded in memory.\n" );
        return _ERROR__;
    }    
    
    library = LoadLibrary( cmd->param1 );
    if ( library == NULL )
    {
        RemotePrint( sock, " Error loading the library.\n" );
        return _ERROR__;
    }
    
    RemotePrint( sock, "  Library succesfully loaded.\n" );
    
    return _SUCCESS__;
}


/*
    Función: FreeALibrary
    Descripción: 
        Funcion que controla la descarga de una biblioteca
    Parametros: 
        sock        - Socket del cliente conectado
        cmd         - Estructura donde esta la biblioteca a descargar
    Retorno: 
        _ERROR__    - Fallo en alguno de los pasos del proceso
        _SUCCESS__  - Funcionamiento correcto del proceso
*/
int 
FreeALibrary( int sock, PCOMMAND cmd )
{
    HINSTANCE library;
    
    if ( cmd->param1[0] == 0 )
    {
        RemotePrint
            ( sock, " Library to unload not especified (param1).\n" );
        return _ERROR__;
    }
    
    library = GetModuleHandle( cmd->param1 );
     if ( library == NULL )
    {
        RemotePrint( sock, " Library not loaded in memory.\n" );
        return _ERROR__;
    }   
    
    if ( FreeLibrary( library ) == 0)
    {
        RemotePrint( sock, " Error unloading library.\n" );
        return _ERROR__;
    }
    
    RemotePrint( sock, " Library succesfully unloaded.\n" );
    
    return _SUCCESS__;
}


/*
    Función: UnicodeToANSI
    Descripción: 
        Funcion generica que convierte una cadena en UNICODE a una cadena en
        ANSI.
    Parametros: 
        unicode_src - Cadena en unicode
        ansi_dst    - Cadena en ANSI, para salida
    Retorno: 
        _ERROR__    - Fallo en alguno de los pasos del proceso
        _SUCCESS__  - Funcionamiento correcto del proceso
*/
int 
UnicodeToANSI( UNICODE_STRING * unicode_src, char ** ansi_dst )
{
    int i;
    
    * ansi_dst = calloc( 1, unicode_src->Length );
    if ( (* ansi_dst ) == NULL )
        return _ERROR__;
    
    for ( i = 0; i < unicode_src->Length; i++ )
        (* ansi_dst)[i] = unicode_src->Buffer[i];
    
    return _SUCCESS__;
}


/*
    Función: ShowModules
    Descripción: 
        Funcion que envia al cliente la lista de modulos cargados en memoria
        usando la lista del PEB
    Parametros: 
        sock        - Socket del cliente conectado
    Retorno: 
        _ERROR__    - Fallo en alguno de los pasos del proceso
        _SUCCESS__  - Funcionamiento correcto del proceso
*/
int  
ShowModules( int sock )
{
    // Variables locales
    PPEB            peb;
    PLDR_MODULE     ldr_module;
    PLDR_MODULE     ldr_init;    
    PPEB_LDR_DATA   peb_ldr;
    char          * base_name; 
    
    // Obtenemos un puntero al PEB   
    peb = GetPtrToPEB();       
    if ( peb == NULL )    
    {
        RemotePrint( sock, "  Error: obtaint PEB.\n" );
        return _ERROR__;
    }
    
    // Obtenemos el LDR a partir del PEB
    peb_ldr = peb->LoaderData;    
    
    // Obtenemos la lista de modulos a partir del LDR
    ldr_module = ( PLDR_MODULE ) peb_ldr->InLoadOrderModuleList.Flink;
    ldr_init = ldr_module;       
    
    // Checkeamos cada modulo
    do
    {                
        // Controlamos que no sea el inicio
        if 
        ( 
            ( DWORD ) ldr_module->InLoadOrderModuleList.Flink 
            != 
            ( DWORD ) ldr_init 
        )        
        {        
            // Enviamos los datos de ese modulo al cliente
            if 
            ( 
                UnicodeToANSI( & ldr_module->FullDllName, & base_name ) 
                == 
                _SUCCESS__ 
            )
            {
                RemotePrint( sock, base_name );
                RemotePrint( sock, "\n" ); 

                free( base_name );
            }
            else
                RemotePrint
                    ( sock, " Error: converting BaseDLLName to ASCII.\n" );        
        }
                              
        // Cargamos el siguiente modulo
        ldr_module = ( PLDR_MODULE ) ldr_module->InLoadOrderModuleList.Flink;        
            
    } while ( ( DWORD )ldr_module != ( DWORD )ldr_init );   
    
    // Si no hay problemas salimos con _SUCCESS__
    return _SUCCESS__;
}
