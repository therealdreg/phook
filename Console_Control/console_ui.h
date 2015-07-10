/*******************************************************************************
 
    Project Name:       phook
    Project Version:    1.0.1
    File Name:          console_ui.h
    
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

#ifndef _CONSOLE_UI_H__
#define _CONSOLE_UI_H__


// INCLUDES NECESARIAS
#include "console.h"


// DEFINICIONES DE LAS FUNCIONES
void ShowConsoleHeader      ( int );
void ShowConsoleHelp        ( int );
void ShowGetLastErrorString ( const char * );
void RemotePrint            ( int, const char * );

#endif  //_CONSOLE_UI_H__
