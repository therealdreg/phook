/*******************************************************************************
 
    Project Name:       phook
    Project Version:    1.0.1
    File Name:          exp.c
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

#include "exp.h"

/*
    Función: GetPtrToExpDir
    Descripción: 
        Funcion para obtener la direccion del Export Directory a partir de 
        un IMAGE_DOS_HEADER.
    Parametros: 
        image_dos_header - Base de donde se sacara la direccion del 
                           Export Directory 
    Retorno: 
       La direccion del Export Directory, si encontramos algun problema 
       devolvemos NULL.
*/
PIMAGE_EXPORT_DIRECTORY  
GetPtrToExpDir( PIMAGE_DOS_HEADER image_dos_header )
{
    unsigned long           image_data_directoy;
    PIMAGE_EXPORT_DIRECTORY image_export_directory;
    
    image_data_directoy = \
        GetDataDirAddr( image_dos_header, IMAGE_DIRECTORY_ENTRY_EXPORT );
        
    if ( image_data_directoy == 0 )
        return NULL;
        
    image_export_directory = (PIMAGE_EXPORT_DIRECTORY) \
        GetPtrFromRVA( image_dos_header, image_data_directoy );
    
    return image_export_directory;
}

/*
    Función: GetDataDirAddr
    Descripción: 
        Funcion generica encontrar el offset del DATA DIRECTORY
    Parametros: 
        image_dos_header - Cabecera DOS del archivo
        index            - Indice de que directorio buscamos
    Retorno: 
        offset de la posicion del DATA DIRECTORY buscado o 0 en caso de error
*/
unsigned long
GetDataDirAddr( PIMAGE_DOS_HEADER image_dos_header, int index )
{
    PIMAGE_DATA_DIRECTORY  image_directory;
    
    image_directory = GetPtrToImgDir( image_dos_header );

    if ( image_directory == NULL )
        return 0;

    if ( image_directory[index].VirtualAddress == 0 )
        return 0;
    
    return (unsigned long)  \
        ( 
            ((DWORD) image_directory[index].VirtualAddress ) 
        );
}


/*
    Función: GetPtrToImgDir
    Descripción: 
        Funcion generica encontrar el offset del IMAGE DIRECTORY
    Parametros: 
        image_dos_header - Cabecera DOS del archivo
    Retorno: 
        offset de la posicion del IMAGE DIRECTORY o NULL en caso de error
*/
PIMAGE_DATA_DIRECTORY
GetPtrToImgDir( PIMAGE_DOS_HEADER image_dos_header )
{
    PIMAGE_NT_HEADERS       image_nt_headers;
    IMAGE_DATA_DIRECTORY    image_directory;
    PIMAGE_EXPORT_DIRECTORY return_function;
    
    image_nt_headers = GetPtrToImgNTHdrs( image_dos_header );
    
    if ( image_nt_headers == NULL )
        return NULL;
    
    return & image_nt_headers->OptionalHeader.DataDirectory[0];
}

PIMAGE_NT_HEADERS 
GetPtrToImgNTHdrs( PIMAGE_DOS_HEADER image_dos_header )
{
    if ( image_dos_header->e_lfanew == ((DWORD) NULL) )
        return NULL;
    
    return ( PIMAGE_NT_HEADERS ) \
        (  ((DWORD)image_dos_header) + ((DWORD) image_dos_header->e_lfanew) );
}

PIMAGE_SECTION_HEADER
GetPtrToImgSect( PIMAGE_DOS_HEADER image_dos_header )
{
    PIMAGE_DATA_DIRECTORY image_data_directory;
    
    image_data_directory = GetPtrToImgDir( image_dos_header );
    
    if ( image_data_directory == NULL )
        return NULL;
    
    return (PIMAGE_SECTION_HEADER) \
        ( image_data_directory + IMAGE_NUMBEROF_DIRECTORY_ENTRIES );
}

PIMAGE_SECTION_HEADER 
GetPtrToImgSectRVA( PIMAGE_DOS_HEADER image_dos_header, DWORD rva )
{
    PIMAGE_SECTION_HEADER image_section_header;
    unsigned long         i;
    DWORD                 number_of_sections;
    PIMAGE_NT_HEADERS     image_nt_headers;
    
    image_nt_headers = GetPtrToImgNTHdrs( image_dos_header );
    if ( image_nt_headers == NULL )
        return NULL;
        
    image_section_header = \
        GetPtrToImgSect( (PIMAGE_DOS_HEADER) image_dos_header ); 
        
    if ( image_section_header == NULL )
        return NULL;
        
    number_of_sections = image_nt_headers->FileHeader.NumberOfSections;
    
    for ( i = 0; i < number_of_sections; i++, image_section_header++ )
    {
		DWORD size = image_section_header->Misc.VirtualSize;
		if ( 0 == size )
			size = image_section_header->SizeOfRawData;
			
        if ( (rva >= image_section_header->VirtualAddress) && 
             (rva < (image_section_header->VirtualAddress + size)))
            return image_section_header;
    }
    
    return NULL;
}

DWORD GetPtrFromRVA( PIMAGE_DOS_HEADER image_dos_header, DWORD rva )
{
    PIMAGE_SECTION_HEADER image_section_header;
    unsigned long         delta;

    image_section_header = GetPtrToImgSectRVA( image_dos_header, rva );
    if ( image_section_header == NULL )
        return 0;
    
    delta = (unsigned long) \
    (
        image_section_header->VirtualAddress 
        - 
        image_section_header->PointerToRawData
    );
	
    return (DWORD) ( (DWORD) image_dos_header + (rva - delta) );
}
