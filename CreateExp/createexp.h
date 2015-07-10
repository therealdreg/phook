/*******************************************************************************
 
    Project Name:       phook
    Project Version:    1.0.1
    File Name:          createexp.h
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

#ifndef _CREATE_EXP_H__
#define _CREATE_EXP_H__

#include <stdio.h>
#include <windows.h>
#include "exp.h"
#include "file_map.h"
#include "ui.h"

#define VERSION "1.0.1"

#define MIN_ARGS 3

#define ARG_NR_LIB_SRC   1
#define ARG_NR_PATH_DST  2

typedef struct FILE_CREATEXP_s
{
    FILE * dst_def;
    FILE * dst_c;
    
} FILE_CREATEXP_t;


int  CreateExp( int, char ** );
int  _CreateExp( PIMAGE_DOS_HEADER, FILE_CREATEXP_t * );
int  GetPtrToNameFromOrd
    ( PIMAGE_DOS_HEADER, WORD, PIMAGE_EXPORT_DIRECTORY, FILE_CREATEXP_t * );


#endif /* _CREATE_EXP_H__ */
