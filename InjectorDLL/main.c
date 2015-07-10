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

#include "injectordll.h"

int 
main( int argc, char * argv[] )
{
    // Variables locales
    int return_function;
 
    // Inicializacion de las variables locales   
    return_function = _SUCCESS__;
    
    // Mostramos la cabecera del programa
    ShowProgramHeader( VERSION );
    
    // Controlamos los argumentos
    if ( argc < MIN_ARGUMENTS )
    {
        ShowSyntax( argv[0] );
        return_function = _ERROR__;
    }
    
    // Lanzamos la funcion principal 
    if ( return_function == _SUCCESS__ )
    {
        if ( !strcmp( argv[2], "-p" ) )
            return_function = InjectorDll
            ( 
                argv[ARG_NR_DLL_TO_INJ], 
                argv[ARG_NR_TYPE_APP], 
                NULL, 
                (DWORD) atoi( argv[ARG_NR_PROC_PID] )
            );
        else
            return_function = InjectorDll
            ( 
                argv[ARG_NR_DLL_TO_INJ], 
                argv[ARG_NR_TYPE_APP], 
                argv[ARG_NR_PROG_NAME], 
                0 
            );
        
    }

    // Espera de pulsacion de ENTER
    getchar();

    return return_function;
}
