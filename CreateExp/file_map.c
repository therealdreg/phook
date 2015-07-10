/*******************************************************************************
 
    Project Name:       phook
    Project Version:    1.0.1
    File Name:          file_map.c
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

#include "file_map.h"

int GetPtrViewFile( const char * file_name, VIEW_FILE_t * view_file )
{
    view_file->lib_handle = \
        CreateFile
        (
            file_name             ,
            GENERIC_READ          ,
            FILE_SHARE_READ       ,
            NULL                  ,
            OPEN_EXISTING         ,
            FILE_ATTRIBUTE_NORMAL ,
            NULL
        );
        
    if ( view_file->lib_handle == INVALID_HANDLE_VALUE )
    {
        ShowGetLastErrorString( "CreateFile" );
        return -1;
    }
    
    view_file->lib_mapp = \
        CreateFileMapping
        (
            view_file->lib_handle , 
            NULL                  ,
            PAGE_READONLY         ,
            0                     ,
            0                     ,
            0
        );
            
    if ( view_file->lib_mapp == NULL )
    {
        CloseHandle( view_file->lib_handle );
        ShowGetLastErrorString( "CreateFileMapping" );
        
        return -1;
    }

    view_file->lib_view_file = \
    MapViewOfFile
    (
        view_file->lib_mapp ,
        FILE_MAP_READ       ,
        0                   ,
        0                   ,
        0
    );
    if ( view_file->lib_view_file == NULL )
    {
        CloseHandle( view_file->lib_mapp );
        CloseHandle( view_file->lib_handle );
        ShowGetLastErrorString( "MapViewOfFile" );
        
        return -1;
    }

    return 0;
}

    
void FreePtrViewFile( VIEW_FILE_t * view_file )
{
    UnmapViewOfFile( view_file->lib_view_file );
    CloseHandle( view_file->lib_mapp );   
    CloseHandle( view_file->lib_handle );
}
    
