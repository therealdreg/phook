/*******************************************************************************
 
    Project Name:       phook
    Project Version:    1.0.1
    File Name:          peb_hook.h
    
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

#ifndef _PEB_HOOK_H__
#define _PEB_HOOK_H__


////////////////////////////////////////////////////////////////////////////////
//
//      INCLUDES
//
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "PEB.h"
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//      DEFINICIONES
//
#define _SUCCESS__   0
#define _ERROR__    -1
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//      MACROS
//
#define IS_EQUALi    ( string1, string2 ) ( strcmpi( string1, string2 ) == 0 )
#define IS_NOT_EQUALi( string1, string2 ) ( strcmpi( string1, string2 ) != 0 )
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//      ESTRUCTURAS
//
typedef struct _IAT_TABLE
{
    DWORD       base_orig;
    DWORD       base_owner;
    
} IAT_TABLE, *PIAT_TABLE;
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//      DECLARACION DE LAS FUNCIONES 
//
int                      
PebHooking       ( int, const char * orig, const char * hook );

PIMAGE_DATA_DIRECTORY    
GetPtrToImgDir   ( PIMAGE_DOS_HEADER );

PIMAGE_EXPORT_DIRECTORY  
GetPtrToExpDir   ( PIMAGE_DOS_HEADER );

PIMAGE_IMPORT_DESCRIPTOR 
GetPtrToImpDir   ( PIMAGE_DOS_HEADER );

unsigned long            
GetDataDirAddr   ( PIMAGE_DOS_HEADER, int );

int                      
LoadAddressTable ( int, PIAT_TABLE *, const char *, const char *, DWORD * );

int                      
IATConstructor   ( int, PIAT_TABLE, const char *, const char *, DWORD );

void                     
ChangeIAT        ( int, PIAT_TABLE, DWORD *, DWORD );

int                      
PebHook          ( int, const char *, const char * );

int                      
CheckKernel32    ( int sock, const char * orig, const char * hook );

DWORD                    
GetImageSize     ( UCHAR * base );
////////////////////////////////////////////////////////////////////////////////


#endif /* _PEB_HOOK_H__ */
