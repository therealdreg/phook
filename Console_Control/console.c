/*******************************************************************************
 
    Project Name:       phook
    Project Version:    1.0.1
    File Name:          console.c
    
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

#include "console.h"
#include "console_admin.h"


/*
    Función: LoadConsole
    Descripción: 
        Funcion encargada la crear la conexion por sockets y esperar llamadas 
        externas desde el "Netcat".
    Parametros: 
        -
    Retorno: 
        -
*/
void
LoadConsole( void )
{
   // Variables locales
    PCOMMAND        cmd;    
    WSABUF          DataBuf;
    char            buffer[SIZE_OF_BUFFER];
    int             i = 0;
    int             exit_sucess, rc;
    int             sock;
    int             recvbuflen, iSendResult;
    BOOL            cmd_ok;
    WSAOVERLAPPED   RecvOverlapped = {0};
    DWORD           iFlags, iResult;
        
    // Inicializacion de variables    
    cmd         = (PCOMMAND) calloc( 1              , sizeof(COMMAND) );    

    // Esperamos una conexion externa para la comunicacion        
    sock = WaitForConn();                    
    if 
    ( 
        (sock == SOCKET_ERROR) ||  (cmd == NULL)     
    )
    {
        MessageBoxA
        ( 
            NULL                                  , 
            "Can't start console."                , 
            "ERROR"                               , 
            MB_ICONERROR | MB_OK 
        );
        return;
    }
                
    // Creamos un evento para manejar la conexion Overlapped
    RecvOverlapped.hEvent = WSACreateEvent();
    if ( RecvOverlapped.hEvent  == NULL )   
    {        
        WSACloseEvent(RecvOverlapped.hEvent);
        closesocket(sock);
        
        free( cmd );
        
        // Volvemos a escuchar
        LoadConsole(); 
            
        return; 
    }
        
    // Seteamos los buffers            
    DataBuf.len = SIZE_OF_BUFFER;
    DataBuf.buf = buffer;                  
                
    // Enviamos la cabecera el cliente conectado            
    ShowConsoleHeader( sock );    
    
    exit_sucess = 0;
                
    // Ciclo principal de recepcion de datos y procesado
    do 
    {
        // Preparamos los buffers        
        memset( buffer, 0, SIZE_OF_BUFFER );        
        memset( cmd, 0, sizeof( COMMAND ) );
        cmd_ok = FALSE;
        iFlags = 0;
        
        // Enviamos el PROMPT del sistema
        RemotePrint( sock, PROMPT );    
        
        // Leemos el buffer del socket de forma asincrona        
        rc = WSARecv( sock, &DataBuf, 1, NULL, &iFlags, &RecvOverlapped, NULL );
        if ( ( rc == SOCKET_ERROR ) && ( WSA_IO_PENDING != WSAGetLastError() ) ) 
        {
            iResult = 0;
            exit_sucess == 1;
        }

        rc = \
            WSAWaitForMultipleEvents
                ( 1, &RecvOverlapped.hEvent, TRUE, INFINITE, TRUE );
        if ( rc == WSA_WAIT_FAILED ) 
        {
            iResult = 0;
            exit_sucess == 1;
        }

        rc = \
            WSAGetOverlappedResult
                ( sock, & RecvOverlapped, & iResult, FALSE, & iFlags );
        if ( rc == FALSE ) 
        {
            iResult = 0;
            exit_sucess == 1;
        }

        WSAResetEvent(RecvOverlapped.hEvent);                            
        
        // Procesamos los datos recibidos si hemos recibido algo
        if ( iResult > 0 ) 
        {                                                 
            // Eliminamos el salto de linea del Netcat
            buffer[ iResult - 1 ] = 0;
            
            // Parseamos el buffer de entrada                  
            Parser( buffer, cmd );    
                   
            // CONTROL DE COMANDOS
            exit_sucess = ConsoleAdmin( sock, cmd );
            
        }  
        
        // Controlamos si debemos salir
        if ( exit_sucess == 1 )
        {
            closesocket(sock);
        }
        
    } while ( ( iResult > 0) && (exit_sucess == 0) );
    
    // Cerramos el evento
    WSACloseEvent(RecvOverlapped.hEvent);
    
    // Liberamos la memoria pedida
    free( cmd );
        
    // Volvemos a escuchar
    LoadConsole();            
}


/*
    Función: Parser
    Descripción: 
        Funcion que rellena una estructura COMMAND usando un buffer
    Parametros: 
        buffer      - Buffer en ANSI con la cadena que se desea parsear
        cmd         - Estructura donde se almacenará la salida del parser
    Retorno: 
        -
*/
void 
Parser ( char * buffer, PCOMMAND cmd)
{
    int     i       = 0;
    int     last_i  = 0;
    int     pos     = 0;
    int     len     = 0;
    
    len = strlen( buffer );
    
    for (i = 0; i < (len+1); i++)
    {
        if ( ( (buffer[i] == ' ') || (buffer[i] == 0) ) && ( pos == 0 ) )
        {
            memcpy( &cmd->cmd, buffer, i);
            last_i = i + 1;
            pos++;
        }
        else
        {
            if ( ( (buffer[i] == ' ') || (buffer[i] == 0) ) && ( pos == 1 ) )
            {
                memcpy( &cmd->param1, &buffer[last_i], i - last_i);
                last_i = i + 1;            
                pos++;         
            }
            else
            {
        
                if 
                ( 
                    ( (buffer[i] == ' ') || (buffer[i] == 0) ) && ( pos == 2 ) 
                )
                {
                    memcpy( &cmd->param2, &buffer[last_i], i - last_i);
                    pos++;
                }
            }   
        }             
    }    
}


/*
    Función: WaitForConn
    Descripción: 
        Funcion espera la conexion de un cliente externo por NETCAT
    Parametros: -
    Retorno: 
        Socket del cliente conectado
*/
int
WaitForConn( void )
{
    // Variables locales
    int             sock_srv;     // Socket de escucha
    int             sock_cli;     // Socket conectado
    SOCKADDR_IN     srv_addr;     // Informacion Local    
    DWORD           res;
    u_short         port;

    // Inicializacion variables locales
    res = 0;
    port = FIRST_PORT_TO_BIND;    

    // Creamos el socket para las conexiones entrantes
    if ( ( sock_srv = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ) ) < 0 )
    {
        MessageBoxA( NULL, "socket", "error", MB_OK );
        
        return SOCKET_ERROR;
    }
    
    // Preparamos las estructuras para la conexion del socket
    memset( &srv_addr, 0, sizeof( srv_addr ) );   
    srv_addr.sin_family         = AF_INET;                
    srv_addr.sin_addr.s_addr    = inet_addr( "0.0.0.0" ); 
    srv_addr.sin_port           = htons( port );              

    // Bindeamos el socket
    do 
    {        
        
        res = bind( sock_srv, ( SOCKADDR * )&srv_addr, sizeof( srv_addr ) );
        if (  res == SOCKET_ERROR )
        {
            port++;
            srv_addr.sin_port = htons( port );   
        }
    }
    while ( res == SOCKET_ERROR );
    
    LogPort( port );
    
    // Nos ponemos en escucha
    if ( listen( sock_srv, MAX_CONNECTIONS ) < 0 )
    {
        MessageBoxA( NULL, "listen", "error", MB_OK );
        closesocket( sock_srv ); 
        
        return SOCKET_ERROR;
    }        

    // Esperamos a que se conecte un cliente
    sock_cli = accept(sock_srv, NULL, NULL);
    if ( sock_cli == INVALID_SOCKET )
    {
        MessageBoxA( NULL, "accept", "error", MB_OK );     
        closesocket( sock_srv );
    
        return SOCKET_ERROR;
    }
      
    // Cerramos el socket de escucha
    closesocket( sock_srv );        
    
    // Devolvemos el socket cliente
    return sock_cli;    	
}

void 
LogPort( u_short port )
{
    char            log_port[MAX_PATH];

    // Logueamos el puerto del proceso
    sprintf( log_port, "%04d - %04d", GetCurrentProcessId(), port );
    
    // Escribimos la entrada en un fichero
    HANDLE file;
    DWORD  chars;    
    
    file = \
        CreateFileA
        ( 
            FILE_LOG                            , 
            GENERIC_WRITE    | GENERIC_READ     , 
            0                                   , 
            NULL                                , 
            OPEN_ALWAYS                         , 
            0                                   , 
            NULL 
        );
        
    if ( file != INVALID_HANDLE_VALUE )
    {        
        if ( SetFilePointer( file, 0, NULL, FILE_END ) != -1 )
        {
            WriteFile( file, log_port, strlen( log_port ), &chars, NULL );
            WriteFile( file, "\x0D\x0A", 2, &chars, NULL );
        }
        CloseHandle( file );         
    }    
}
