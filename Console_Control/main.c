/*******************************************************************************
 
    Project Name:       phook
    Project Version:    1.0.1
    File Name:          main.c
    
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

#include "console.h"


/*
    Función: DllMain
    Descripción: 
        En la carga de la DLL solo lanzamos un hilo para controlar las llamadas
        a la consola.
    Parametros: 
        hInst           - Manejador de la biblioteca
        reason          - Razon por la que llama a esta funcion
        reserved        - Valor usado internamente
    Retorno: 
        FALSE           - Fallo en alguno de los pasos del proceso
        TRUE            - Funcionamiento correcto del proceso
*/
BOOL 
APIENTRY 
DllMain ( HINSTANCE hInst, DWORD reason, LPVOID reserved )
{
    // Variables locales
    WSADATA         wsaData;
    DWORD           tid;  
    
    tid = 0;          
        
    switch (reason)
    {
      case DLL_PROCESS_ATTACH:
            // Obtenemos un handle al hilo principal
            MainThread = GetMainThread();                    
        
            // Inicializamos las conexiones por sockets
            if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)             
                MessageBoxA( NULL, "WSAStartup", "error", MB_OK );            
            else          
                // Creamos el hilo que controlara la consola
                CreateThread
                ( 
                    NULL                                , 
                    0                                   , 
                    (LPTHREAD_START_ROUTINE) LoadConsole, 
                    NULL                                , 
                    0                                   , 
                    & tid 
                );                    
        break;

      case DLL_PROCESS_DETACH:
        break;

      case DLL_THREAD_ATTACH:
        break;

      case DLL_THREAD_DETACH:
        break;
    }

    return TRUE;
}

