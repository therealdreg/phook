/*******************************************************************************
 
    Project Name:       phook
    Project Version:    1.0.1
    File Name:          common.h
    
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

#ifndef _COMMON_H____
#define _COMMON_H____

#include <windows.h>
#define DLLEXPORT __declspec (dllexport)

#define FILE_LOG "C:\\CreateFile.log"

static int  UnicodeToANSI   ( WCHAR *, char * );
static void CreateFileLogger( const char * );

#endif /* _COMMON_H____ */
