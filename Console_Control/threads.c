/*******************************************************************************
 
    Project Name:       phook
    Project Version:    1.0.1
    File Name:          threads.c
    
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

#include "threads.h"

/*
    Función: GetMainThread
    Descripción: 
        Funcion para obtener un HANDLE al thread principal de un proceso.
        
    Parametros: -
    
    Retorno: 
        HANDLE del thread principal o NULL en caso de error
    
*/
HANDLE 
GetMainThread( void )
{
    // Variabloes locales
    DWORD thread_id;
    
    // Obtenemos el id del thread principal
    if ( GetMainThreadId( & thread_id ) == FALSE )
        return NULL;

    // Devolvemos un HANDLE al thread principal
    return (HANDLE) OpenThread( THREAD_ALL_ACCESS, 0, thread_id );
}


/*
    Función: GetMainThreadId
    Descripción: 
        Funcion para obtener el ThreadId del thread principal de un proceso.
        
    Parametros: 
        thread_id           - Variable de salida
    
    Retorno: 
        TRUE o FALSE en funcion del exito de la funcion y en caso positivo
        devolvemos el id en la variable de entrada
    
*/
BOOL GetMainThreadId( DWORD * thread_id )
{
    // Variables locales
    HANDLE        hThreadSnap; 
    THREADENTRY32 th32; 
    BOOL          return_function;
    DWORD         process_id;
    
    // Inicializacion de las variables
    process_id      = GetCurrentProcessId();
    hThreadSnap     = INVALID_HANDLE_VALUE; 
    return_function = FALSE;

    // Obtenemos un SnapShot de todos los hilos del sistema
    hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, process_id ); 
    if( hThreadSnap == INVALID_HANDLE_VALUE ) 
    { 
        ShowGetLastErrorString
            ( " GetMainThreadId() - CreateToolhelp32Snapshot()" ); 
        return FALSE;
    } 
    
    // Buscamos el primer hilo creado en nuestro proceso
    th32.dwSize = sizeof( THREADENTRY32 );     
    if( !Thread32First( hThreadSnap, & th32 ) ) 
        ShowGetLastErrorString( "GetMainThreadId() - Thread32First()" );  
    
    do
    {
        if ( th32.th32OwnerProcessID == process_id )
        {
            * thread_id     = th32.th32ThreadID;
            return_function = TRUE;
        }
        
    } while ( Thread32Next( hThreadSnap, & th32 ) && return_function != TRUE );
    
    // Cerramos el manejador del Snapshot
    CloseHandle( hThreadSnap );
    
    return return_function;
}
