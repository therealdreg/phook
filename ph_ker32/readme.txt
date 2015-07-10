/*******************************************************************************
 
    Project Name:       phook
    Project Version:    1.0.1
    File Name:          readme.txt
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

E  N  G  L  I  S  H :

Copy the files .c and .def that you want to use in the directory ph_ker32.
    - Windows_2003_R2
    - Windows_Vista
    - Windows_XP_SP2
    - Windows_XP_SP3

We use the Windows_XP_SP3 files for default.

Remember: 

ph_ker32 must be compilated as with the -fomit-frame-pointer 
option.

The APIs that we want to make something:

    - Must have the prototype as original API.
    - Can't have -fomit-frame-pointer    
    - Must be _stdcall
    - In .def file, we need mark the number of arguments:
        Alias=APIName@arguments * 4 @ Ordinal

For example:

Sample in .c file:

    DLLEXPORT 
    HANDLE _stdcall _CreateFileA
    (
        LPCSTR lpFileName,	
        DWORD dwDesiredAccess, 
        DWORD dwShareMode, 
        LPSECURITY_ATTRIBUTES lpSecurityAttributes,	
        DWORD dwCreationDistribution, 
        DWORD dwFlagsAndAttributes, 
        HANDLE hTemplateFile 
    )
    {
    ...
    }

Sample in .def file:
    CreateFileA=_CreateFileA@28              @ 122

(7 arguments * 4 = 28).

PD: The symbolic constant, FAKE_LIB in .c file, must have the same name as the
    DLL file.

-

S  P  A  N  I  S  H :

Copiar los ficheros .c y .def que se quieran usar en el directorio ph_ker32.
    - Windows_2003_R2
    - Windows_Vista
    - Windows_XP_SP2
    - Windows_XP_SP3

Por defecto están los ficheros de Windows_XP_SP3.

Recuerde:

ph_ker32 debe compilarse con la opción -fomit-frame-pointer.

Las APIs en las que se quiera hacer algo: 

    - Deben tener el prototipo de la API original.
    - No pueden tener -fomit-frame-pointer
    - Deben ser _stdcall
    - En el .def hay que indicarle el numero de argumentos después del nombre:
        Alias=Nombre@arguments * 4 @ Ordinal

Ejemplo:

Definición en el .c:

    DLLEXPORT 
    HANDLE _stdcall _CreateFileA
    (
        LPCSTR lpFileName,	
        DWORD dwDesiredAccess, 
        DWORD dwShareMode, 
        LPSECURITY_ATTRIBUTES lpSecurityAttributes,	
        DWORD dwCreationDistribution, 
        DWORD dwFlagsAndAttributes, 
        HANDLE hTemplateFile 
    )
    {
    ...
    }

Declaración en el .def:
    CreateFileA=_CreateFileA@28              @ 122

(7 argumentos * 4 = 28).

PD: La constante simbólica FAKE_LIB in .c debe ser igual al nombre del fichero
    de la DLL.
