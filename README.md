# phook
Full DLL Hooking via Process Environment Block

phook - The PEB Hooker, Phrack 65, Shearer & Dreg: http://phrack.org/issues/65/10.html#article

[Paper in Spanish](https://github.com/David-Reguera-Garcia-Dreg/phook/tree/master/papers/Spanish)
[Paper in Russian](https://github.com/David-Reguera-Garcia-Dreg/phook/tree/master/papers/Russian)
[Paper in French](https://github.com/David-Reguera-Garcia-Dreg/phook/tree/master/papers/French)
[Paper in Enlgish](https://github.com/David-Reguera-Garcia-Dreg/phook/tree/master/papers/Enlgish)

## Usage
Just Imagine that we want to do PEB HOOKING at kernel32.dll with ph_ker32.dll, we have chosen this software poc.exe for the example (allocated in the phook bin\ folder)

Steps:

1.- Execute InjectorDLL specifying a software to execute and the DLL console 
    that will be injected in the process:
        - InjectorDLL.exe console.dll -u poc.exe 

The process will become to a suspend state and there will be a bind socket at 
the port specified at the C:\ph_listen_ports.log  file.
    
        C:\phook\bin>InjectorDll.exe console.dll -u poc.exe 
          ______________________________________________
         |               InjectorDLL v1.0.1             |
         |                                              |
         | Juan Carlos Montes   eunimedes@hotmail.com   |
         | David Reguera Garcia Dreg@fr33project.org    |
         | -------------------------------------------- |
         |          http://www.fr33project.org          |
         |______________________________________________|

         Showing injection data .....
             Program to inject : C:\phook\bin\poc.exe
             Library to inject: C:\phook\bin\console.dll

         [OK]   - CONSOLE.
         [OK]   - Create process:
             [INFO] PID:       0x0254
             [INFO] P. HANDLE: 0x000007B0
             [INFO] TID:       0x0CF0
             [INFO] T. HANDLE: 0x000007A8
         [INFO] - Injecting DLL...
             [OK]   - Allocate memory in the extern process.
             [INFO] - Address reserved on the other process: 0x00240000
             [INFO] - Space requested: 306
             [OK]   - Creating structure for the dll load.
             [OK]   - Writing structure for the dll load.
             [OK]   - Creating remote thread.
             [INFO] - Thread created with TID: 0x0580
             [INFO] - Attempt: 1
             [INFO] - Thread has entered suspension mode.
             [OK]   - Injection thread ended.
             [OK]   - Memory in remote thread freed.
         [OK]   -  DLL injected.

         [OK]   -  Injection ended:
             Try to connect to port written in
             C:\ph_listen_ports.log, syntax: PID-PORT
             Example: nc 127.0.0.1 1234 (1234 is the first default port)


2.-  To connect to the server it is necessary to use a client, similar 
     to netcat, to an open port, in this case 1234.
    
    C:\>nc 127.0.0.1 1234

          ________________________________________________________
         |                   Phook Prompt v1.0.1                  |
         |       Juan Carlos Montes   eunimedes@hotmail.com       |
         |       David Reguera Garcia Dreg@fr33project.org        |
         | ------------------------------------------------------ |
         |            http://www.fr33project.org                  |
         |________________________________________________________|
    
    
     ph > help
     _________________________________________________________________
    |                       Phook Prompt v1.0.1                       |
    |                                                                 |
    | Command list:                                                   |
    | --------------------------------------------------------------- |
    | help                      - Shows this screen                   |
    | exit                      - Closes and unloads the console      |
    | suspend                   - Pauses the programs execution       |
    | resume                    - Resumes the programs execution      |
    | showmodules               - Shows the modules list              |
    | load [param1]             - Loads in memory the library         |
    |                             especified in [param1]              |
    | unload [param1]           - Unloads a librery in memory         |
    |                             especified in [param1]              |
    | pebhook [param1] [param2] - Performs PEB Hook over a dll        |
    |                             [param1]: Name of the original dll  |
    |                             [param2]: Path to the DLL hook      |
    |_________________________________________________________________|


3.- PEB HOOKING is performed to kernel32.dll with ph_ker32.dll:
    ph > pebhook kernel32.dll C:\phook\bin\windows_xp_sp3\ph_ker32.dll

4.- Resume command is sent to start the execution process:
    
    ph > resume
    ph >    
    C:\phook\bin> 

5.- poc.exe create the files in C:\
    - file
    - file2
    - file3

6.- ph_ker32.dll registers the successful calls to the APIs 
    CreateFileA and CreateFileW [R.14] in file C:\CreateFile.log

7.-
    C:\>more CreateFile.log
    
    C:\file1
    C:\file2
    C:\file3
