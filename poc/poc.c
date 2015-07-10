#include <windows.h>

int main( void )
{
    HANDLE hFile;
    
    printf( " Creating files...\n" );
    
    hFile = CreateFileW( L"C:\\file1", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
    if ( hFile != INVALID_HANDLE_VALUE )
    {
        printf( " C:\\file1 created!!\n" ); 
        CloseHandle( hFile );
    }
    else
        printf( " C:\\file1 cant created!!\n" );

    hFile = CreateFileW( L"C:\\file2", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
    if ( hFile != INVALID_HANDLE_VALUE )
    {
        printf( " C:\\file2 created!!\n" ); 
        CloseHandle( hFile );
    }
    else
        printf( " C:\\file2 cant created!!\n" );
        
    hFile = CreateFileW( L"C:\\file3", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
    if ( hFile != INVALID_HANDLE_VALUE )
    {
        printf( " C:\\file3 created!!\n" ); 
        CloseHandle( hFile );
    }
    else
        printf( " C:\\file3 cant created!!\n" );
    
    printf
    ( 
      "\n" 
      " If this process has been peb hooked with ph_ker32.dll\n" 
      " view the files created in the log file: C:\\CreateFile.log\n" 
    );
    
    getchar();
        
    return 0;
}
