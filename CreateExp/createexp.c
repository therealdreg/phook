/*******************************************************************************
 
    Project Name:       phook
    Project Version:    1.0.1
    File Name:          createexp.c
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

#include "createexp.h"

int FileCreatExpInit( FILE_CREATEXP_t * file, const char * name_dst )
{
    char   buff[300];
    char * debug;
    
    debug = "File openned:";
    
    sprintf( buff, "%.260s.def", name_dst );
    file->dst_def = fopen( buff, "w" );
    if ( file->dst_def == NULL )
    {
        printf( " [FAIL] - %s %s\n", debug, buff );
        perror( "" );
        return -1;
    }
    printf( " [OK] - %s %s\n", debug, buff );
    
    sprintf( buff, "%.260s.c", name_dst );
    file->dst_c = fopen( buff, "w" );
    if ( file->dst_c == NULL )
    {
        printf( " [FAIL] - %s %s\n", debug, buff );
        perror( "" );
        fclose( file->dst_def );
        
        return -1;
    }
    printf( " [OK] - %s %s\n", debug, buff );
    
    return 0;
}

void FileExpDestroy( FILE_CREATEXP_t * file )
{
    fclose( file->dst_def );
    fclose( file->dst_c );
}

int  CreateExp( int argc, char * argv[] )
{
    int               return_function;
    VIEW_FILE_t       view_file;
    FILE_CREATEXP_t   file;
    char            * debug;
    
    return_function = 0;

    ShowProgramHeader( VERSION );

    if ( argc != MIN_ARGS )
    {
        puts( " Syntax: CreateExp.exe library.dll library\n" );
        return -1;    
    }
    
    if ( FileCreatExpInit( & file, argv[ARG_NR_PATH_DST] ) == -1 )
        return -1;
    else
    {
        if ( GetPtrViewFile( argv[ARG_NR_LIB_SRC], & view_file ) == -1 )
            return_function = -1;
        else
        {
            return_function = \
                _CreateExp
                    ( (PIMAGE_DOS_HEADER) view_file.lib_view_file, & file );    
            
            FreePtrViewFile( & view_file );
        }
        
        FileExpDestroy( & file );
    }
    
    debug = "EXPORTING FUNCTIONS!!!";
    if ( return_function == 0 )
        printf( "\n [OK] - %s", debug );
    else
        printf( "\n [FAIL] - %s", debug );
    
    return return_function;
}

int _CreateExp( PIMAGE_DOS_HEADER image_dos_header, FILE_CREATEXP_t * file )
{
    int                         i;
    PIMAGE_EXPORT_DIRECTORY     ptr_exp_dir;
    char                        buff[0x400];
    
    ptr_exp_dir = GetPtrToExpDir( image_dos_header );
    if ( ptr_exp_dir == NULL )
    {
        puts( " Error GetPtrToExpDir" );
        return -1;
    }

    sprintf
    (
        buff,
        "LIBRARY    default\n"        
        "EXPORTS\n"
    );
    if ( fwrite( buff, strlen( buff ), 1, file->dst_def ) != 1 )   
    {
        perror( "" );
        return -1;
    }
    
    sprintf
    (
        buff,
        "\n#define DLLEXPORT __declspec (dllexport)\n"
        "\n#define FAKE_LIB \"BIBLIOTECA.dll\"\n"        
        "\n"        
        "\nunsigned long tmp;\n"        
        "\n"                        
        "#define JMP( lib, func )        \\\n"
        "   asm ( \"pushad\" );           \\\n"        
        "   asm                           \\\n"
        "   (                             \\\n"
        "       \" push edx             \\n\"      \\\n"
        "       \" push %s              \\n\"      \\\n"
        "       \" call eax             \\n\"      \\\n"
        "       \" pop edx              \\n\"      \\\n"
        "       \" push %s              \\n\"      \\\n"
        "       \" push eax             \\n\"      \\\n"
        "       \" call edx             \\n\"      \\\n"
        "       \" mov %s, eax          \\n\"      \\\n"        
        "       \" popad                \\n\"      \\\n"
        "                                          \\\n"   
        "       : :                                \\\n"
        "       \"a\" (GetModuleHandle) ,         \\\n"
        "       \"g\" (lib)             ,         \\\n"
        "       \"g\" (func)            ,         \\\n"
        "       \"d\" (GetProcAddress)  ,         \\\n"
        "       \"g\" (tmp)                       \\\n"        
        "    );                                   \\\n"
        "   asm ( \"jmp %s\" : : \"g\" (tmp) );   \n\n" ,          
        
        "\%1", "\%2", "\%4", "\%0"
    );
    
    if ( fwrite( buff, strlen( buff ), 1, file->dst_c ) != 1 )
    {
        perror( "" );
        return -1;
    }
    
    for ( i = 0; i < ptr_exp_dir->NumberOfFunctions; i++ )
        if 
        ( 
            GetPtrToNameFromOrd( image_dos_header, i, ptr_exp_dir, file ) == -1 
        )
            return -1;
    
    return 0;
} 

int
GetPtrToNameFromOrd
( 
    PIMAGE_DOS_HEADER image_dos_header   , 
    WORD ordinal                         , 
    PIMAGE_EXPORT_DIRECTORY ptr_exp_dir  ,  
    FILE_CREATEXP_t * file 
)
{
    int                         i;  
    char                     ** table_names;
    WORD                      * table_ordinals;
    BOOL                        ord_found;
    char                      * name;
    char                        unkown[MAX_PATH];
    char                        buff[0x400];
    WORD                        real_ordinal;
    
    real_ordinal = ptr_exp_dir->Base + ordinal;
        
    sprintf( unkown, "_UNKOWN__%d", real_ordinal );
    
    table_names  = (char **) \
        GetPtrFromRVA( image_dos_header, (DWORD) ptr_exp_dir->AddressOfNames ); 
    table_ordinals = (WORD *) \
        GetPtrFromRVA
            ( image_dos_header, (DWORD) ptr_exp_dir->AddressOfNameOrdinals );
    
    ord_found = FALSE;
    i = 0;
    while ( i < ptr_exp_dir->NumberOfNames && ord_found == FALSE )
    {
        if ( table_ordinals[i] == ordinal )
            ord_found = TRUE;
        else
            i++;
    }
    
    if ( ord_found == TRUE )
        name = (char *) \
            GetPtrFromRVA( image_dos_header, (DWORD) table_names[i] );
    else
        name = unkown;
    
    sprintf
    ( 
        buff                                       ,
         
        "%s=_%s              @ %d\n"               , 
        
        name                                       , 
        name                                       , 
        real_ordinal
    );     
    if ( fwrite( buff, strlen( buff ), 1, file->dst_def ) != 1 )
    {
        perror( "" );
        return -1;
    }
    
    sprintf
    ( 
        buff, 
        "\nDLLEXPORT void _%s ( void )\n"
        "{\n"                             
        "    JMP( FAKE_LIB, %d );\n"
        "}\n", 
    
        name                                      , 
        real_ordinal
    );  
    if ( fwrite( buff, strlen( buff ), 1, file->dst_c ) != 1 )
    {
        perror( "" );
        return -1;
    }
}

