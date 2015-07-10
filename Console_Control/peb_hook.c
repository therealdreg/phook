/*******************************************************************************
 
    Project Name:       phook
    Project Version:    1.0.1
    File Name:          peb_hook.c
    
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

#include "peb_hook.h"


/*
    Función: PebHooking
    Descripción: 
        Funcion generica para realizar peb hook sobre una biblioteca.
    Parametros: 
        sock            - Socket de comunicacion con la consola remota
        orig            - Nombre de la biblioteca que deseamos hookear
        hook            - Nombre de la biblioteca propia
    Retorno: 
        _ERROR__        - Fallo en alguno de los pasos del proceso
        _SUCCESS__      - Funcionamiento correcto del proceso
*/
int
PebHooking ( int sock, const char * orig, const char * hook )
{
    // Variables Locales
    DWORD                   libHook                 = 0;
    DWORD                   libOrig                 = 0;
    DWORD                   NumberOfFuntions        = 0;
    PIMAGE_DOS_HEADER       pOrig                   = NULL;
    PIMAGE_EXPORT_DIRECTORY image_export_directory  = NULL;
    PIAT_TABLE              iat_table               = NULL;
                
    // Checkeamos que existen los archivos
    libOrig = ( DWORD ) LoadLibraryA( orig );
    if ( libOrig == 0 )
    {
        RemotePrint( sock, " [INFO]: Parameter 1 invalid.\n" );
        return _ERROR__;
    }

    libHook = ( DWORD ) LoadLibraryA( hook );
    if ( libHook == 0 )
    {
        RemotePrint( sock, " [INFO]: Parameter 2 invalid.\n" );
        return _ERROR__;
    }    
                
    // Cargamos la lista de exportaciones de las 2 librerias    
    if 
    ( 
        LoadAddressTable( sock, & iat_table, orig, hook, & NumberOfFuntions ) 
        != 
        0 
    )
    {
        RemotePrint
        ( 
            sock, 
            " Error loading data from EXPORT TABLE.\n" 
        );
        return _ERROR__;
    }      
    
    // Realizamos PEB HOOK cambiando: base, EP, size
    if ( PebHook( sock, orig, hook ) != 0)
    {
        RemotePrint( sock, " Error inserting hook over PEB.\n" );
        return _ERROR__;
    }            
    
    /*
    // En caso de que estemos hookeando el kernel32.dll, nos aseguramos de que 
    // los nuevos modulos nos usen como referencia a nosotros y no al kernel32
    // real.
    if ( CheckKernel32( sock, orig, hook ) != 0 )
    {
        RemotePrint
        ( 
            sock, 
            " Error replacing kernel32.dll on Loader.\n"
        );
        
        return _ERROR__;
    } 
    */    
    
    // Reconstruimos la IAT con la nueva DLL
    if ( IATConstructor( sock, iat_table, orig, hook, NumberOfFuntions ) != 0 )
    {
        RemotePrint( sock, " Error replacing IAT.\n" );
        return _ERROR__;
    }    

    // Si no encontramos problemas salimos correctamente
    return _SUCCESS__;
}

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
    return (PIMAGE_EXPORT_DIRECTORY) \
        GetDataDirAddr( image_dos_header, IMAGE_DIRECTORY_ENTRY_EXPORT );
}

/*
    Función: GetPtrToImpDir
    Descripción: 
        Funcion para obtener la direccion del Import Descriptor a partir de 
        un IMAGE_DOS_HEADER.
    Parametros: 
        image_dos_header - Base de donde se sacara la direccion del 
                           Import Descriptor
    Retorno: 
       La direccion del Import Descriptor, si encontramos algun problema 
       devolvemos NULL.
*/
PIMAGE_IMPORT_DESCRIPTOR  
GetPtrToImpDir( PIMAGE_DOS_HEADER image_dos_header )
{ 
    PIMAGE_IMPORT_DESCRIPTOR image_import_descriptor;
    
    image_import_descriptor = ( PIMAGE_IMPORT_DESCRIPTOR ) \
        GetDataDirAddr( image_dos_header, IMAGE_DIRECTORY_ENTRY_IMPORT );
    
    return image_import_descriptor;
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
            + 
            ((DWORD) image_dos_header) 
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
    
    if ( image_dos_header->e_lfanew == ((DWORD) NULL) )
        return NULL;
    
    image_nt_headers = ( PIMAGE_NT_HEADERS ) \
        (  ((DWORD)image_dos_header) + ((DWORD) image_dos_header->e_lfanew) );
    
    if ( image_nt_headers == NULL )
        return NULL;
    
    return & image_nt_headers->OptionalHeader.DataDirectory[0];
}

/*
    Función: LoadAddressTable
    Descripción: 
        Funcion que carga una tabla con las direcciones de las funciones que 
        exporta la biblioteca original y su correspondiente en la biblioteca
        que usamos como hook
    Parametros: 
        sock                > Socket de comunicacion con la consola remota
        iat_table           > Tabla donde se almacenara la lista de funciones 
                              exportadas        
        orig                > Nombre de la biblioteca que deseamos hookear
        owner               > Nombre de la biblioteca propia
        NumberOfFuntions    < Numero de funciones de la tabla
    Retorno: 
        _ERROR__            - Fallo en alguno de los pasos del proceso
        _SUCCESS__          - Funcionamiento correcto del proceso
*/
int
LoadAddressTable
( 
    int          sock      , 
    PIAT_TABLE * iat_table , 
    const char * orig      , 
    const char * owner     , 
    DWORD      * NumberOfFuntions 
)
{
    // Variables locales
    PIMAGE_EXPORT_DIRECTORY     pOrig;
    PIMAGE_EXPORT_DIRECTORY     pOwner;
    DWORD                       libOrig;
    DWORD                       libOwner;    
    int                         i;
    
    // Cargamos las 2 bibliotecas y obtenemos sus manejadores    
    libOrig     = (DWORD) LoadLibraryA( orig );
    libOwner    = (DWORD) LoadLibraryA( owner );    
        
    // Obtenemos los punteros al Export Directory
    pOrig = GetPtrToExpDir( ( PIMAGE_DOS_HEADER ) libOrig );
    pOwner = GetPtrToExpDir( ( PIMAGE_DOS_HEADER ) libOwner );
    
    // Controlamos el nº de funciones que exporta cada una de ellas
    if ( pOrig->NumberOfFunctions != pOwner->NumberOfFunctions )
        RemotePrint
        ( 
            sock, 
            " [WARNING] The list of exported functions doesn't match.\n" 
        );
        
    // Controlamos que exporten por lo menos una funcion
    if ( pOrig->NumberOfFunctions == 0 )
        return _ERROR__;
     
    // Preparamos la tabla en funcion del nº de funciones que exporten
    * iat_table = (PIAT_TABLE) \
        calloc( pOrig->NumberOfFunctions, sizeof( IAT_TABLE ) );
    if ( iat_table == NULL )
        return _ERROR__;               
    
    // Montamos la tabla de exportaciones
    for (i = 0; i< pOrig->NumberOfFunctions; i++)
    {
        (* iat_table)[i].base_orig  = (DWORD) \
            GetProcAddress( (HINSTANCE) libOrig , (LPCSTR) pOrig->Base + i );
        
        (* iat_table)[i].base_owner = (DWORD) \
            GetProcAddress( (HINSTANCE) libOwner, (LPCSTR) pOwner->Base + i );    
    }       
    
    // Devolvemos el numero de funciones exportadas
    * NumberOfFuntions = pOrig->NumberOfFunctions;
    
    // Si no encontramos problemas devolvemos 0
    return _SUCCESS__;
}

/*
    Función: PebHook
    Descripción: 
        Funcion intercambia los valores del PEB entre la biblioteca original
        y la biblioteca que usamos como hook
    Parametros: 
        sock                > Socket de comunicacion con la consola remota
        orig                > Nombre de la biblioteca que deseamos hookear
        owner               > Nombre de la biblioteca propia
    Retorno: 
        _ERROR__            - Fallo en alguno de los pasos del proceso
        _SUCCESS__          - Funcionamiento correcto del proceso
*/
int
PebHook( int sock, const char * orig, const char * owner)
{
    // Variables locales
    DWORD                       libOrig;
    DWORD                       libOwner; 
    PPEB                        peb;
    PPEB_LDR_DATA               peb_ldr;
    PLDR_MODULE                 ldr_module;
    PLDR_MODULE                 ldr_orig;
    PLDR_MODULE                 ldr_owner;
    PLDR_MODULE                 ldr_init;
    DWORD                       xchange;
    
    // Obtenemos los manejadores de las 2 bibliotecas   
    libOrig     = (DWORD) LoadLibrary( orig );
    libOwner    = (DWORD) LoadLibrary( owner );
    
    if ( libOrig == 0 )
    {
        RemotePrint( sock, "-> Error loading original library.\n" );
        return _ERROR__;
    }
    
    if ( libOwner == 0 )
    {
        RemotePrint( sock, "-> Error loading our own library.\n" );
        return _ERROR__;
    }    
    
    // Obtenemos la direccion del PEB
    peb = GetPtrToPEB();
    if ( peb == NULL )
    {
        RemotePrint( sock, "-> Error obtaining PEB's Address.\n" );
        return _ERROR__;
    }
           
    // Obtenemos el LDR a partir del PEB
    peb_ldr = peb->LoaderData;
    
    // Obtenemos la lista de modulos a partir del LDR
    ldr_module = ( PLDR_MODULE ) peb_ldr->InLoadOrderModuleList.Flink;
    ldr_init = ldr_module;       
    
    // Por cada modulo checkeamos que sea lo que buscamos
    do
    {
        // Buscamos el original
        if ( ( DWORD ) ldr_module->BaseAddress == libOrig)
            ldr_orig = ldr_module;

        // Buscamos nuestra biblioteca
        if ( ( DWORD ) ldr_module->BaseAddress == libOwner)
            ldr_owner = ldr_module;            
        
        // Cargamos el siguiente modulo
        ldr_module = ( PLDR_MODULE ) ldr_module->InLoadOrderModuleList.Flink;        
            
    } while ( ( DWORD )ldr_module != ( DWORD )ldr_init );
    
    // Checkeamos que las encontramos
    if ( ( ldr_orig != NULL ) && ( ldr_owner != NULL ) )
    {
        xchange                = ( DWORD ) ldr_orig->EntryPoint;
        ldr_orig->EntryPoint   = ldr_owner->EntryPoint;
        ldr_owner->EntryPoint  = ( LPVOID ) xchange;
        xchange                = ( DWORD ) ldr_orig->BaseAddress;
        ldr_orig->BaseAddress  = ldr_owner->BaseAddress;
        ldr_owner->BaseAddress = ( LPVOID ) xchange;
        xchange                = ( ULONG ) ldr_orig->SizeOfImage;
        ldr_orig->SizeOfImage  = ldr_owner->SizeOfImage;
        ldr_owner->SizeOfImage = ( ULONG ) xchange;                                     
                
        return _SUCCESS__;
    } 
    else 
    {
        RemotePrint
            ( sock, "-> Error searching libraries on modules." );
    
        return _ERROR__;       
    }
        
    // Si no las encontramos devolvemos ERROR
    return _ERROR__;    
}


/*
    Función: IATConstructor
    Descripción: 
        Funcion que modifica los valores de la IAT de las bibliotecas ya
        cargadas en memoria sustituyendo las llamadas a la biblioteca original
        por llamadas a nuestra biblioteca
    Parametros: 
        sock                > Socket de comunicacion con la consola remota
        iat_table           > Tabla donde estan almacenadas la lista de 
                              funciones exportadas        
        orig                > Nombre de la biblioteca que deseamos hookear
        owner               > Nombre de la biblioteca propia
        NumberOfFuntions    > Numero de funciones de la tabla
    Retorno: 
        _ERROR__            - Fallo en alguno de los pasos del proceso
        _SUCCESS__          - Funcionamiento correcto del proceso
*/
int
IATConstructor
( 
    int          sock              , 
    PIAT_TABLE   iat_table         , 
    const char * orig              ,  
    const char * owner             , 
    DWORD        NumberOfFunctions 
)
{
    // Variables locales
    DWORD                        libOrig;
    DWORD                        libOwner;     
    DWORD                      * pIAT;     
    DWORD                        base_exe;
    DWORD                        base_module; 
    PPEB                         peb;
    WCHAR                      * uOrig;
    PLDR_MODULE                  ldr_module;    
    PLDR_MODULE                  ldr_init;    
    PPEB_LDR_DATA                peb_ldr;
    BOOL                         check;
    PIMAGE_IMPORT_DESCRIPTOR     image_import_descriptor;
    IMAGE_IMPORT_DESCRIPTOR      image_import_descriptor_null;
    
    // Inicializamos datos
    memset
    ( 
        &image_import_descriptor_null , 
        0                             , 
        sizeof( image_import_descriptor_null ) 
    );
    
    // Obtenemos las bases de las 2 dlls
    libOrig     = (DWORD) LoadLibrary( orig );
    libOwner    = (DWORD) LoadLibrary( owner );
    
    if ( libOrig == 0 )
    {
        RemotePrint( sock, "-> Error loading original library.\n" );
        return _ERROR__;
    }
    
    if ( libOwner == 0 )
    {
        RemotePrint( sock, "-> Error loading our own library.\n" );
        return _ERROR__;
    }             
        
    // Obtenemos la direccion del PEB
    peb = GetPtrToPEB();
    if ( peb == NULL )
        return _ERROR__;
    
    // Obtenemos el LDR a partir del PEB
    peb_ldr = peb->LoaderData;    
    
    // Obtenemos la lista de modulos a partir del LDR
    ldr_module = ( PLDR_MODULE ) peb_ldr->InLoadOrderModuleList.Flink;
    ldr_init = ldr_module;       
    
    // Checkeamos cada modulo
    do
    {
        // Activamos el checkeo del modulo
        check = TRUE;
        pIAT = 0;
        
        // Controlamos por las bases que no seamos nosotros ni la dll hookeada
        if ( ( DWORD ) ldr_module->BaseAddress == libOrig)
            check = FALSE;

        if ( ( DWORD ) ldr_module->BaseAddress == libOwner)
            check = FALSE;     
        
        // Controlamos que no sea el inicio
        if 
        ( 
            ( DWORD ) ldr_module->InLoadOrderModuleList.Flink 
            == 
            ( DWORD ) ldr_init 
        )        
            check = FALSE;        
        
        // Cambiamos la IAT
        if ( check )
        {
            // Cambiamos la IAT de ese modulo        
            image_import_descriptor = GetPtrToImpDir( ldr_module->BaseAddress );  
            
            if ( image_import_descriptor != NULL )
            {
                do
                {                
                    pIAT = ( DWORD * ) image_import_descriptor->FirstThunk;                      
                    if ( pIAT != 0)
                    {            
                        pIAT = ( DWORD * )                                    \
                        (                                                     \
                            ( ( DWORD ) pIAT )                                \
                            +                                                 \
                            ( ( DWORD ) ldr_module->BaseAddress )             \
                        );                        
                        
                        ChangeIAT( sock, iat_table, pIAT, NumberOfFunctions );
                    }
                    image_import_descriptor++;
                    
                } 
                while 
                ( 
                    memcmp
                    ( 
                        image_import_descriptor               , 
                        & image_import_descriptor_null        , 
                        sizeof(image_import_descriptor_null) 
                    ) 
                    != 
                    0 
                ); 
            }
        }
                              
        // Cargamos el siguiente modulo
        ldr_module = ( PLDR_MODULE ) ldr_module->InLoadOrderModuleList.Flink;        
            
    } while ( ( DWORD )ldr_module != ( DWORD )ldr_init );            
        
    // Si no hay problemas devolvemos _SUCCESS__
    return _SUCCESS__;
}


/*
    Función: ChangeIAT
    Descripción: 
        Funcion que modifica los valores de la IAT indicada intercambiando las 
        direcciones de la biblioteca original y la que hookea
    Parametros: 
        sock                > Socket de comunicacion con la consola remota
        iat_table           > Tabla donde estan almacenadas la lista de 
                              funciones exportadas        
        NumberOfFuntions    > Numero de funciones de la tabla
    Retorno: 
        -
*/
void
ChangeIAT
( 
    int          sock              , 
    PIAT_TABLE   iat_table         , 
    DWORD      * base              , 
    DWORD        NumberOfFunctions 
)
{
    // Variables locales
    int     i;
    DWORD   oldProtect;

    // Nos aseguramos que podemos escribir en la seccion de codigo de la IAT
     if 
    ( 
        VirtualProtect
        ( 
            base                   , 
            1                      , 
            PAGE_EXECUTE_READWRITE , 
            & oldProtect 
        )
        ==
        0
    )
    {
        RemotePrint( sock, "-> Error changing IAT permissions.\n" );
        return;
    }
    
    // Cambiamos todos los valores referentes a la biblioteca hookeada
    // por direcciones a nuestras funciones
    while ( * base != 0 )
    {
        for ( i = 0; i < NumberOfFunctions; i++ )
        {
            if ( (* base) == iat_table[i].base_orig )
            {
                * base = iat_table[i].base_owner;
                
                break;
            }
        }
           
        base++;
    }
    
    // Restauramos permisos iniciales de la zona.
     if 
    ( 
        VirtualProtect
        ( 
            base                    , 
            1                       , 
            oldProtect              , 
            & oldProtect 
        )
        ==
        0
    )
    {
        RemotePrint( sock, "-> Error restoring IAT permissions.\n" );
        return;
    }
}


/*
/*
    Función: CheckKernel32
    Descripción: 
        Funcion que modifica la cadena 'kernel32.dll' en el ntdll.dll para 
        probocar que el Loader use nuestra DLL como kernel32.dll
    Parametros: 
        sock                > Socket de comunicacion con la consola remota
        orig                > Cadena con el nombre de la libreria original
        hook                > Cadena con el nombre de nuestra biblioteca
    Retorno: 
        _ERROR__            - Fallo en alguno de los pasos del proceso
        _SUCCESS__          - Funcionamiento correcto del proceso
int 
CheckKernel32( int sock, const char * orig, const char * hook )
{
    // Variables locales
    UCHAR       * ntdll;
    UCHAR       * user;
    UCHAR       * user2k;
    UCHAR       * kernel;
    UCHAR       * tmp;
    int           i;
    DWORD         oldp;
    BOOL          found;         
    DWORD         imagesize;
    
    // Controlamos que se sustituya el kernel32.dll
    if ( IS_NOT_EQUALi( orig, "kernel32.dll" ) )
    {        
        return _SUCCESS__;
    }
    
    // Buscamos la posicion de la cadena dentro de ntdll.dll
    ntdll = ( UCHAR * ) GetModuleHandle( "ntdll.dll" );
    if ( ntdll == NULL )
    {
        RemotePrint
        ( 
            sock, 
            " [ERROR]: Can't obtain the image base of \"ntdll.dll\" \n" 
        );        
        return _ERROR__;
    }
    kernel = user = ntdll;
    
    // Obtenemos el tamaño de "ntdll.dll"
    imagesize = GetImageSize( ntdll );
    if( imagesize == 0 )
    {
        RemotePrint
        ( 
            sock, 
            " [ERROR]: Can't obtain the size of \"ntdll.dll\" \n" 
        );
        return _ERROR__;
    }
    
    // Windows Vista:
    tmp = (UCHAR *) GetModuleHandle( "ntdll.dll" );
    if ( tmp != NULL )
    {
        tmp += 0x02E350;
        if ( tmp <= ((UCHAR *) imagesize) )
            if ( wmemcmp( tmp, L"kernel32.dll", strlen( "kernel32.dll" ) ) == 0 )
                return _SUCCESS__;
    }
    
    // Saltamos la cabecera    
    found = FALSE;
    do
    {        
        // Comprobamos si estamos en una cadena "kernel32.dll" en UNICODE
        if ( wmemcmp( kernel, L"kernel32.dll", strlen( "kernel32.dll" ) ) == 0 )
        {       
            // Nos situamos el inicio de la supuesta cadena "user32.dll"
            user  = kernel;
            user += 0x1C;
            if ( wmemcmp( user, L"user32.dll", strlen( "user32.dll" ) ) == 0 )
            {
                found = TRUE;
            }
            
            // Busqueda especial para Windows 2000
            user2k  = kernel;
            user2k -= 0x18;    
            if ( wmemcmp( user, L"user32.dll", strlen( "user32.dll" ) ) == 0 )
            {
                found = TRUE;
            }            
        }
        
        // Si no lo encontramos nos movemos al siguiente byte
        if (!found)
            kernel++;        
            
    } while ( ( !found ) && ( kernel < ( UCHAR * ) imagesize ) );

    // Si encontramos la cadena adecuada la sustituimos por nuestro hook
    if ( found )
    {
        // Nos aseguramos de que podemos escribir en la zona de memoria que 
        // contiene la cadena
        if 
        ( 
            VirtualProtect
            ( 
                kernel                       , 
                1                            , 
                PAGE_EXECUTE_READWRITE       , 
                & oldp 
            ) 
            == 
            0 
        )
        {
            RemotePrint
            ( 
                sock, 
                "-> Error changing permissions to the kernel32.dll area "
                "of LdrInitializeThunk.\n" 
            );
            
            return _ERROR__;
        }
        
        // Situamos los punteros al final de las cadenas para realizar un cambio 
        // inverso y asi olvidarnos de la ruta
        // IMPORTANTE: Deben tener el mismo tamaño las 2 cadenas
        hook   += strlen( hook );
        kernel += ( strlen( orig ) * 2 );
        
        // Sustituimos las cadenas
        for ( i = strlen( "kernel32.dll" ) + 1; i != 0; i-- )
        {
            *kernel = *hook;
            kernel--; kernel--;
            hook--;            
        }
    }
    
    if 
    ( 
        VirtualProtect
        ( 
            kernel                       , 
            1                            , 
            oldp                         , 
            & oldp 
        ) 
        == 
        0 
    )
    {
        RemotePrint
        ( 
            sock, 
            "-> Error restoring permissions to the kernel32.dll area of "
            "LdrInitializeThunk.\n" 
        );
        
        return _ERROR__;
    }
    
    // Si todo va bien devolvemos _SUCCESS__
    return _SUCCESS__;
}
*/

/*
    Función: GetImageSize
    Descripción: 
        Obtenemos el tamaño de la imagen en memoria de un modulo controlando
        las secciones
    Parametros: 
        base                > Base del modulo
    Retorno: 
        Tamaño en memoria del modulo, o 0 en caso de error.
*/
DWORD
GetImageSize( UCHAR * base )
{
    PIMAGE_SECTION_HEADER image_section_header;
    PIMAGE_NT_HEADERS     image_nt_headers;
    int                   i;
    DWORD                 ImageSize, ImageSizeTmp;
    
    image_section_header  = (PIMAGE_SECTION_HEADER) \
        GetPtrToImgDir( (PIMAGE_DOS_HEADER) base );
    
    if ( image_section_header == NULL )
        return 0;
        
    image_section_header = (PIMAGE_SECTION_HEADER)     \
        (                                              \
            ((DWORD) image_section_header)             \
            +                                          \
            ( sizeof(IMAGE_DATA_DIRECTORY)             \
            *                                          \
            IMAGE_NUMBEROF_DIRECTORY_ENTRIES )         \
        );    

    if ( image_section_header == NULL )
        return 0;
    
    image_nt_headers = (PIMAGE_NT_HEADERS)                \
    (                                                     \
        ((DWORD)((PIMAGE_DOS_HEADER) base)->e_lfanew )    \
        +                                                 \
        ((DWORD) base)                                    \
    );                                                    

    ImageSize = 0;
    for ( i = 0; i < image_nt_headers->FileHeader.NumberOfSections; i++ ) 
    {
        if 
        ( 
            image_section_header[i].Misc.VirtualSize 
            > 
            image_section_header[i].SizeOfRawData 
        )
            ImageSizeTmp =                                 \
                image_section_header[i].VirtualAddress     \
                +                                          \
                image_section_header[i].Misc.VirtualSize;
        else
            ImageSizeTmp =                              \
                image_section_header[i].VirtualAddress  \
                +                                       \
                image_section_header[i].SizeOfRawData;
        
        if ( ImageSize < ImageSizeTmp )
            ImageSize = ImageSizeTmp;
    }
        
    ImageSize += ((DWORD) base);
    ImageSize--;
    ImageSize &= 0xFFFFF000;
    ImageSize += 0x1000;
    
    return ImageSize;
}
