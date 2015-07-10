/*******************************************************************************
 
    Project Name:       phook
    Project Version:    1.0.1
    File Name:          PEB.h
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

#ifndef _PEB_H__
#define _PEB_H__


////////////////////////////////////////////////////////////////////////////////
//
//      INCLUDES
//
#include <windows.h>
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//      DEFINICIONES
//
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L) 

typedef LPVOID  * PPVOID;
typedef void   (* PPEBLOCKROUTINE) (PVOID PebLock);
typedef LONG 	  NTSTATUS;
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//      ENUMERADOS
//
typedef enum _PROCESSINFOCLASS { ProcessBasicInformation } PROCESSINFOCLASS;
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//      ESTRUCTURAS
//
typedef struct _PEB_FREE_BLOCK 
{
    struct _PEB_FREE_BLOCK *Next; 
    ULONG Size;

} PEB_FREE_BLOCK, *PPEB_FREE_BLOCK;

typedef struct _UNICODE_STRING 
{
    USHORT  Length;
    USHORT  MaximumLength;
    PWSTR  Buffer;
  
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _RTL_DRIVE_LETTER_CURDIR 
{
    USHORT Flags; 
    USHORT Length; 
    ULONG TimeStamp; 
    UNICODE_STRING DosPath;

} RTL_DRIVE_LETTER_CURDIR, *PRTL_DRIVE_LETTER_CURDIR;

typedef struct _RTL_USER_PROCESS_PARAMETERS 
{
    ULONG MaximumLength; 
    ULONG Length; 
    ULONG Flags; 
    ULONG DebugFlags; 
    PVOID ConsoleHandle; 
    ULONG ConsoleFlags; 
    HANDLE StdInputHandle; 
    HANDLE StdOutputHandle; 
    HANDLE StdErrorHandle; 
    UNICODE_STRING CurrentDirectoryPath; 
    HANDLE CurrentDirectoryHandle; 
    UNICODE_STRING DllPath; 
    UNICODE_STRING ImagePathName; 
    UNICODE_STRING CommandLine; 
    PVOID Environment; 
    ULONG StartingPositionLeft; 
    ULONG StartingPositionTop; 
    ULONG Width; ULONG Height; 
    ULONG CharWidth; 
    ULONG CharHeight; 
    ULONG ConsoleTextAttributes; 
    ULONG WindowFlags; 
    ULONG ShowWindowFlags; 
    UNICODE_STRING WindowTitle; 
    UNICODE_STRING DesktopName; 
    UNICODE_STRING ShellInfo; 
    UNICODE_STRING RuntimeData; 
    RTL_DRIVE_LETTER_CURDIR DLCurrentDirectory[0x20];

} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

typedef struct _PEB_LDR_DATA 
{
    ULONG Length; 
    BOOLEAN Initialized; 
    PVOID SsHandle; 
    LIST_ENTRY InLoadOrderModuleList; 
    LIST_ENTRY InMemoryOrderModuleList; 
    LIST_ENTRY InInitializationOrderModuleList;

} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _LDR_MODULE 
{
    LIST_ENTRY InLoadOrderModuleList; 
    LIST_ENTRY InMemoryOrderModuleList; 
    LIST_ENTRY InInitializationOrderModuleList; 
    PVOID BaseAddress; 
    PVOID EntryPoint; 
    ULONG SizeOfImage; 
    UNICODE_STRING FullDllName; 
    UNICODE_STRING BaseDllName; 
    ULONG Flags; 
    SHORT LoadCount; 
    SHORT TlsIndex; 
    LIST_ENTRY HashTableEntry; 
    ULONG TimeDateStamp;

} LDR_MODULE, *PLDR_MODULE;

typedef struct _PEB 
{   
    BOOLEAN InheritedAddressSpace; 
    BOOLEAN ReadImageFileExecOptions; 
    BOOLEAN BeingDebugged; 
    BOOLEAN Spare; 
    HANDLE  Mutant; 
    PVOID ImageBaseAddress; 
    PPEB_LDR_DATA LoaderData; 
    PRTL_USER_PROCESS_PARAMETERS ProcessParameters; 
    PVOID SubSystemData; 
    PVOID ProcessHeap; 
    PVOID FastPebLock; 
    PPEBLOCKROUTINE FastPebLockRoutine; 
    PPEBLOCKROUTINE FastPebUnlockRoutine; 
    ULONG EnvironmentUpdateCount; 
    PPVOID KernelCallbackTable; 
    PVOID EventLogSection; 
    PVOID EventLog; 
    PPEB_FREE_BLOCK FreeList; 
    ULONG TlsExpansionCounter; 
    PVOID TlsBitmap; 
    ULONG TlsBitmapBits[0x2]; 
    PVOID ReadOnlySharedMemoryBase; 
    PVOID ReadOnlySharedMemoryHeap; 
    PPVOID ReadOnlyStaticServerData;
    PVOID AnsiCodePageData; 
    PVOID OemCodePageData; 
    PVOID UnicodeCaseTableData; 
    ULONG NumberOfProcessors; 
    ULONG NtGlobalFlag; 
    BYTE Spare2[0x4]; 
    LARGE_INTEGER CriticalSectionTimeout; 
    ULONG HeapSegmentReserve; 
    ULONG HeapSegmentCommit; 
    ULONG HeapDeCommitTotalFreeThreshold; 
    ULONG HeapDeCommitFreeBlockThreshold; 
    ULONG NumberOfHeaps; 
    ULONG MaximumNumberOfHeaps; 
    PPVOID *ProcessHeaps; 
    PVOID GdiSharedHandleTable; 
    PVOID ProcessStarterHelper; 
    PVOID GdiDCAttributeList; 
    PVOID LoaderLock; 
    ULONG OSMajorVersion; 
    ULONG OSMinorVersion; 
    ULONG OSBuildNumber; 
    ULONG OSPlatformId; 
    ULONG ImageSubSystem; 
    ULONG ImageSubSystemMajorVersion; 
    ULONG ImageSubSystemMinorVersion; 
    ULONG GdiHandleBuffer[0x22]; 
    ULONG PostProcessInitRoutine; 
    ULONG TlsExpansionBitmap; 
    BYTE TlsExpansionBitmapBits[0x80]; 
    ULONG SessionId;

} PEB, *PPEB;

typedef struct _PROCESS_BASIC_INFORMATION 
{
    PVOID Reserved1;
    PPEB PebBaseAddress;
    PVOID Reserved2[2];
    ULONG_PTR UniqueProcessId;
    PVOID Reserved3;
    
} PROCESS_BASIC_INFORMATION;
////////////////////////////////////////////////////////////////////////////////

    
////////////////////////////////////////////////////////////////////////////////
//
//      DEFINICION DE APIS NATIVAS
//    
typedef NTSTATUS 
    (WINAPI * ZWQUERYINFORMATIONPROCESS)
    (
        HANDLE, 
        PROCESSINFOCLASS, 
        PVOID, 
        ULONG, 
        PULONG
    ); 
////////////////////////////////////////////////////////////////////////////////

        
////////////////////////////////////////////////////////////////////////////////
//
//      DEFINICION DE LAS FUNCIONES
//
PPEB GetPtrToPEB( void );
////////////////////////////////////////////////////////////////////////////////


#endif /* _PEB_H__ */
