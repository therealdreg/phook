/*******************************************************************************
 
    Project Name:       phook
    Project Version:    1.0.1
    File Name:          injectordll.c
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

/*
    Función: InjectorDll
    Descripción: Proceso principal encargado de crear el nuevo proceso e 
                 e insertar en memoria del proceso cargado los datos necesarios
                 para poder cargarse la dll especificada
    Parametros: 
        program_name    - Nombre del programa
        dll_name        - Nombre de la dll
    Retorno: 
        _ERROR__        - Fallo en alguno de los pasos del proceso
        _SUCCESS__      - Funcionamiento correcto del proceso
*/
int 
InjectorDll
(    
    const char * dll_name, 
    const char * typeapp, 
    const char * program_name, 
    DWORD pid 
)
{
    STARTUPINFO           startup_info;
    PROCESS_INFORMATION   pinfo;
    int                   return_function;
    char                * debug_string;
    int                   type_of_app;
    DWORD                 create_flag;
 
    // Inicializacion de variables locales
    ZeroMemory( & startup_info       , sizeof(startup_info) );
    ZeroMemory( & pinfo, sizeof(pinfo) );    
    startup_info.cb = sizeof(startup_info);
    return_function = _SUCCESS__;    
            
    
    // Controlamos si debemos lanzar el proceso o engancharnos a uno ya creado
    if ( program_name == NULL )
    {
        // Nos enganchamos al proceso ya creado
        debug_string = "Open process";
        pinfo.hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pid );
        if ( pinfo.hProcess == NULL )
        {
            printf( " [FAIL] - %s\n", debug_string );
            ShowGetLastErrorString( "InjectorDll:OpenProcess(pid)" );
    
            return _ERROR__;
        }
        else
        {
            printf( "\n [OK]   - Procces Attached [%04d].\n", pid );      
        }
        
    } 
    else
    {
        // Mostramos los datos del programa
        ShowInjectorTask( program_name, dll_name );
        
        // Checkeamos el tipo de programa 
        if ( !strcmp( typeapp, "-c" ) )             
            type_of_app = _CONS_APP__;
        else if ( !strcmp( typeapp, "-g" ) )        
            type_of_app = _GUI_APP__;                            
        else if ( !strcmp( typeapp, "-u" ) )        
            type_of_app = GetTypeOfApp( ( LPTSTR ) program_name );        
        else
            return _ERROR__;
            
        if ( type_of_app == _CONS_APP__ )
        {
            puts( " [OK]   - CONSOLE." ); 
             create_flag = CREATE_SUSPENDED | CREATE_NEW_CONSOLE;     
        }
        else if ( type_of_app == _GUI_APP__ )
        {
            puts( " [OK]   - GUI" );
            create_flag = 0;
        }
        else if ( type_of_app == _UNK_APP__ )
            puts( " [OK]   - UNKNOW" );
        
                                  
        // Creamos el proceso en funcion del tipo de exe
        debug_string = "Create process";
        if 
        ( 
            CreateProcess
            (  
                NULL                                  ,
                (LPTSTR) program_name                 ,
                NULL                                  ,
                NULL                                  ,
                FALSE                                 ,
                create_flag                           ,      
                NULL                                  ,
                NULL                                  ,
                & startup_info                        ,
                & pinfo
            ) 
            == 
            0 
        )
        {
            printf( " [FAIL] - %s\n", debug_string );
            ShowGetLastErrorString( "InjectorDll:CreateProcess(program_name)" );
    
            return _ERROR__;
        }
                                    
        // Mostramos los datos del proceso creado
        printf
        ( 
            " [OK]   - %s:\n"
            "     [INFO] PID:       0x%04X\n"
            "     [INFO] P. HANDLE: 0x%08X\n"
            "     [INFO] TID:       0x%04X\n"
            "     [INFO] T. HANDLE: 0x%08X\n"                        
            
            , debug_string
            , pinfo.dwProcessId 
            , pinfo.hProcess 
            , pinfo.dwThreadId 
            , pinfo.hThread                                     
        );               
    
        // Esperamos a la carga del proceso en memoria y en ese momento
        // suspendemos el hilo principal para poder trabajar con el sin problemas
        // en caso de que sea una aplicacion con ventanas
        if ( type_of_app == _GUI_APP__ )
        {
            debug_string = " Waiting for process load.";
            if 
            ( 
                WaitForInputIdle( pinfo.hProcess, INFINITE ) 
                == 
                0
            )
            {
    
                debug_string = "Suspension of remote main thread.";
                if ( SuspendThread( pinfo.hThread ) == -1 )
                {
                    printf( " [FAIL] - %s\n", debug_string );
                    ShowGetLastErrorString
                        ( "InjectorDll:SuspendThread(_GUI_APP__)" );
     
                    return_function = _ERROR__;
                }
                else            
                {
                    printf( " [OK]   - %s.\n", debug_string );
                    return_function == _SUCCESS__;
                }
            }
            else
            {
                printf( " [ERROR] - %s\n", debug_string );
                ShowGetLastErrorString
                    ( "InjectorDll:WaitForInputIdle(_GUI_APP__)" );                   
            
                return_function = _ERROR__;
            }
        } 
        
    }

    // Preparamos y inyectamos la carga de la DLL en el proceso indicado
    if ( return_function == _SUCCESS__ )
    {
        puts( " [INFO] - Injecting DLL... " );
        
        debug_string = " DLL injected.";
        if ( InjectDll( pinfo.hProcess, dll_name ) == _SUCCESS__ )
        {
            printf( " [OK]   - %s\n", debug_string );                          
        }
        else
        {
            printf( " [FAIL] - %s\n", debug_string );
            return_function = _ERROR__;
        }
    }

    // Controlamos los errores y si pasa algo terminamos el proceso remoto
    if ( return_function == _ERROR__ )
    {
        debug_string = " Remote process ended due to an error.";
        if ( TerminateProcess( pinfo.hProcess, 0 ) == -1 )
        {
            printf( " [FAIL] - %s\n", debug_string );
            ShowGetLastErrorString
                ( "InjectorDll:TerminateProcess(program)" );
        }
        else
            printf( " [OK]   - %s.\n", debug_string );
    }
    
    debug_string = " Injection ended:";
    if ( return_function == _SUCCESS__ )
    {
        printf
        (
            "\n"
            " [OK]   - %s\n" 
            "     Try to connect to port written in \n"
            "     C:\\ph_listen_ports.log, syntax: PID-PORT\n"
            "     Example: nc 127.0.0.1 1234 (1234 is the first default port)\n"
            ,
            
            debug_string 
        );
    }
    else
        printf( "\n [FAIL] - %s\n", debug_string );
    
    
    return return_function;
}

/*
    Función: InjectDll
    Descripción: Proceso encargado de controlar la correcta carga en memoria
                 del proceso indicado, de la dll pasada como parametro.
    Parametros: 
        process         - Manejador del proceso donde cargar la dll
        dll_name        - Nombre de la dll
    Retorno: 
        _ERROR__        - Fallo en alguno de los pasos del proceso
        _SUCCESS__      - Funcionamiento correcto del proceso
*/
int
InjectDll( HANDLE process, const char * dll_name )
{
    LOADER_DLL_t            loader_struct;
    unsigned long         * loaderdll_address;
    ULONG                   suspend_count;
    DWORD                   bytes_written;
    DWORD                   htread_id;
    HANDLE                  htread;
    int                     return_function;
    char                  * debug_string;
    int                     timeout_suspend;
    
    // Inicializacion de las variables locales
    return_function = _SUCCESS__;
    
    // Pedimos memoria en el proceso remoto
    debug_string = "Allocate memory in the extern process.";
    loaderdll_address                                                          \
    =   (unsigned long *)                                                      \
        VirtualAllocEx                                                         \
        (                                                                      \
            process                 ,                                          \
            NULL                    ,                                          \
            sizeof( loader_struct ) ,                                          \
            MEM_COMMIT              ,                                          \
            PAGE_EXECUTE_READWRITE                                             \
        )                                                                      \
    ;

    if ( loaderdll_address == NULL )
    {
        printf( "     [FAIL] - %s\n", debug_string );
        ShowGetLastErrorString( "InjectDll:VirtualAllocEx(loader)" );
        
        return _ERROR__;
    }
    else
        printf( "     [OK]   - %s\n", debug_string );
    
    printf
    ( 
        "     [INFO] - Address reserved on the other process: 0x%08X\n", 
        loaderdll_address 
    );
    printf( "     [INFO] - Space requested: %d\n", sizeof( loader_struct ) );
    
    
    // Creamos la estructura del cargador de la dll para poder inyectarlo
    debug_string = "Creating structure for the dll load.";
    if 
    ( 
        CreateLoaderDLLStruct
        ( 
            & loader_struct                   , 
            (unsigned long) loaderdll_address , 
            dll_name 
        ) 
        == 
        _ERROR__ 
    )
    {
        printf( "     [FAIL] - %s\n", debug_string );
        
        return_function = _ERROR__;
    }
    else
        printf( "     [OK]   - %s\n", debug_string );
    
    
    // Escribimos en la memoria remota la estructura de carga de la DLL
    if ( return_function == _SUCCESS__ )
    {
        debug_string = "Writing structure for the dll load.";
        if 
        ( 
            WriteProcessMemory
            (
                process                         ,
                loaderdll_address               ,
                & loader_struct                 ,
                (DWORD) sizeof( loader_struct ) ,
                & bytes_written                 
            )
            ==
            0
        )
        {
            printf( "     [FAIL] - %s\n", debug_string );
            ShowGetLastErrorString( "InjectDll:WriteProcessMemory(loader)" );
            printf( " Has been written: %d bytes.\n\n", bytes_written );
        
            return_function = _ERROR__;
        }
        else
            printf( "     [OK]   - %s\n", debug_string );
    }
    
    // Creamos un hilo remoto para que se ejecute el codigo de nuestro cargador
    if ( return_function == _SUCCESS__ )
    {                     
        debug_string = "Creating remote thread.";
        htread = \
            CreateRemoteThread
            (
                process                    ,
                NULL                       ,
                0                          ,
                (LPVOID) loaderdll_address ,
                NULL                       ,
                0                          ,
                & htread_id
            );

        if ( htread == NULL )
        {
            printf( "     [FAIL] - %s\n", debug_string );
            ShowGetLastErrorString( "InjectDll:CreateRemoteThread(loader)" );
            
            return_function = _ERROR__;
        }
        else
        {
            printf( "     [OK]   - %s\n", debug_string );   
            printf
                ( "     [INFO] - Thread created with TID: 0x%04X\n", htread_id );
        }
    }
    
    // Recuperamos la direccion de la funcion para controlar el estado del
    // hilo remoto
    if ( return_function == _SUCCESS__ )
    {
        timeout_suspend = 0;
        printf( "     [INFO] - Attempt: " );
        do
        {
            printf( "%d", timeout_suspend + 1 );
            Sleep( TIME_OUT_CHECK_SUSPEND_THREAD );
            timeout_suspend++;
            
            if 
            (
                ReadProcessMemory
                (
                    process                                                    ,
                    (LPCVOID) ((ULONG) loaderdll_address ) + DISTANCE_TO_FLAG  ,
                    & loader_struct.flag                                       ,
                    sizeof( loader_struct.flag )                               ,
                    & bytes_written          
                )
                ==
                0
            )
            {
                printf
                ( 
                    " Failure in ReadProccessMemory, : %d bytes have been read."
                    "\n\n", 
                    bytes_written 
                );
                ShowGetLastErrorString
                    ( "InjectDll:ReadProcessMemory(suspend flag)" );
            }
            
            if 
            ( 
                ( loader_struct.flag == 0 ) 
                && 
                ( timeout_suspend < TIME_OUT_SUSPEND ) 
            )
                printf( "\b" );
            else
                putchar( '\n' );
        } 
        while 
            ( 
                ( loader_struct.flag == 0 ) 
                && 
                ( timeout_suspend < TIME_OUT_SUSPEND ) 
            );        
        
        if ( timeout_suspend == TIME_OUT_SUSPEND )
        {
            puts( "     [FAIL] - Thread couldn't be suspended." );
            return_function == _ERROR__;
        }
        else        
            puts( "     [INFO] - Thread has entered suspension mode." );
    }
    
    // Si todo esta correcto terminamos el hilo remoto
    if ( return_function == _SUCCESS__ )
    {
        debug_string = "Injection thread ended.";
        if ( TerminateThread( htread, 0 ) == 0 )
        {
            printf( "     [FAIL] - %s\n", debug_string );
            ShowGetLastErrorString( "InjectDll:TerminateThread(loader)" );
            
            return_function = _ERROR__;
        }
        else
            printf( "     [OK]   - %s\n", debug_string );   
    }
    
    // Liberamos la memoria pedida 
    debug_string = "Memory in remote thread freed.";
    if 
    (
        VirtualFreeEx
        ( 
            process            , 
            loaderdll_address  , 
            0                  , 
            MEM_RELEASE 
        )
        ==
        0
    )
    {
        printf( "     [FAIL] - %s\n", debug_string );
        ShowGetLastErrorString( "InjectDll:VirtualFreeEx(loader)" );    
    }
    else
        printf( "     [OK]   - %s\n", debug_string );  
    
    return return_function;
}


/*
    Función: CreateLoaderDLLStruct
    Descripción: Carga y preparacion de la estructura principal de carga de 
                 la DLL remota.
    Parametros: 
        loader_struct           - Manejador del proceso donde cargar la dll
        init_reserved_memory    - Posicion de la direccion pedida en el proceso
                                  remoto.     
        dll_name                - Nombre de la dll
    Retorno: 
        _ERROR__        - Fallo en alguno de los pasos del proceso
        _SUCCESS__      - Funcionamiento correcto del proceso
*/
int 
CreateLoaderDLLStruct
( 
    LOADER_DLL_t  * loader_struct, 
    unsigned long   init_reserved_memory, 
    const char    * dll_name
)
{
    HANDLE        library;
    FARPROC       load_library;
    FARPROC       get_current_thread;
    FARPROC       suspend_thread;
    
    library = GetModuleHandle( "KERNEL32.DLL" );
    if ( library == NULL )
    {
        ShowGetLastErrorString
            ( "CreateLoaderDLLStruct:GetModuleHandle(KERNEL32.DLL)" );
        return _ERROR__;
    }
    
    load_library = GetProcAddress( library, "LoadLibraryA" );
    if ( load_library == NULL )
    {
        ShowGetLastErrorString
            ( "CreateLoaderDLLStruct:GetProcAddress(LoadLibraryA)" );
        return _ERROR__;
    }
    
    get_current_thread = GetProcAddress( library, "GetCurrentThread" );
    if ( get_current_thread == NULL )
    {
        ShowGetLastErrorString
            ( "CreateLoaderDLLStruct:GetProcAddress(GetCurrentThread)" );
        return _ERROR__;
    }
    
    suspend_thread = GetProcAddress( library, "SuspendThread" );
    if ( suspend_thread == NULL )
    {
        ShowGetLastErrorString
            ( "CreateLoaderDLLStruct:GetProcAddress(SuspendThread)" );
        return _ERROR__;
    }
    
    /* CODE */
    CreatePush  
    ( 
        & loader_struct->push_name_dll               , 
        init_reserved_memory + DISTANCE_TO_NAME_DLL 
    );
    CreateCALL  
    ( 
        & loader_struct->call_load_library              , 
        init_reserved_memory + DISTANCE_TO_FIRST_RETURN , 
        (unsigned long) load_library 
    );
    CreateCALL  
    ( 
        & loader_struct->call_get_current_thread         , 
        init_reserved_memory + DISTANCE_TO_SECOND_RETURN , 
        (unsigned long) get_current_thread 
    );
    loader_struct->PUSH_EAX = PUSH_EAX_OPCODE;
    CreateCALL  
    ( 
        & loader_struct->call_suspendthread , 
        0                                   , 
        (unsigned long) suspend_thread 
    );    
    CreateINCMEM
    ( 
        & loader_struct->inc_flag               , 
        init_reserved_memory + DISTANCE_TO_FLAG 
    );
    
    /* DATA */
    strncpy( loader_struct->name_dll, dll_name, MAX_PATH );
    loader_struct->flag = 0;
    
    return _SUCCESS__;
}


/*
    Función: GetTypeOfApp
    Descripción: Obtiene el tipo de programa en funcion de la espera en la carga
                 de su interface grafico.
    Parametros: 
        program_name            - Nombre y ruta del programa a checkear
        
    Retorno: 
        _GUI_APP__ 
        _CONS_APP__
        _UNK_APP__ 
*/
int 
GetTypeOfApp( LPTSTR program_name )
{
    STARTUPINFO           startup_info;
    PROCESS_INFORMATION   process_information;
    int                   return_function;
    
    memset( & startup_info, 0, sizeof( STARTUPINFO ) );
    
    return_function = _UNK_APP__;
    
    if 
    ( 
        CreateProcess
        (  
            NULL                                  ,
            program_name                          ,
            NULL                                  ,
            NULL                                  ,
            FALSE                                 ,
            CREATE_SUSPENDED | CREATE_NEW_CONSOLE ,      
            NULL                                  ,
            NULL                                  ,
            & startup_info                        ,
            & process_information
        ) 
        == 
        0 
    )
        ShowGetLastErrorString( "GetTypeOfApp:CreateProcess(program_name)" );
    else
    {
        if ( WaitForInputIdle( process_information.hProcess, 0 ) == WAIT_FAILED)
            return_function = _CONS_APP__;
        else
            return_function = _GUI_APP__;
    }
    
    if ( TerminateProcess( process_information.hProcess, 0) == 0 )
        ShowGetLastErrorString( "GetTypeOfApp:TerminateProcess(program)" );
    
    return return_function;
}

