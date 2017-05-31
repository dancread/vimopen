#include <windows.h>
#include <string.h>
#include <shlwapi.h>
#include <stdio.h>

/*
 * Consider
 * https://support.microsoft.com/en-us/help/830473/command-prompt-cmd.-exe-command-line-string-limitation
 */
#define INPUT_BUFFER_SIZE 100000 // Might just make MAX_PATH
#define VIM_CONSOLE
#ifdef VIM_CONSOLE // Using console vim
    #define VIM_EXE "vim"
#endif
#ifdef VIM_GUI // Using gui vim
    #define VIM_EXE "gvim"
#endif
INT main() {
    HANDLE hTempFile;
    INT iBytesWritten;
    CHAR szInputBuffer[MAX_PATH];
    CHAR szTempDirectoryPath[MAX_PATH];
    CHAR szTempFileName[MAX_PATH];
    CHAR szTempBatchFileName[MAX_PATH];
    //CHAR szFullCommand[INPUT_BUFFER_SIZE+sizeof(VIM_EXE)];
    // If piped input, then pipe it to the empty file
    if(GetFileType(GetStdHandle(STD_INPUT_HANDLE)) == FILE_TYPE_PIPE){
      // Get %TEMP% env variable
      GetTempPath(MAX_PATH, &szTempDirectoryPath);
      // Create unique temp bat file
      GetTempFileName(szTempDirectoryPath, "edx", 0, szTempFileName);
      // Change it to a *.bat file
      lstrcpyn(szTempBatchFileName, szTempFileName, MAX_PATH);
      szTempBatchFileName[lstrlen(szTempBatchFileName)-3]='\0';
      lstrcat(szTempBatchFileName,"bat");
      MoveFile(szTempFileName, szTempBatchFileName );
      // Write temp file
      hTempFile = CreateFile((LPTSTR) szTempBatchFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
      // Build command path to execute vim
      //lstrcpy(szFullCommand, VIM_EXE);
      // write vim executable -1 is to remove NULL terminator
      WriteFile(hTempFile, VIM_EXE, strlen(VIM_EXE), &iBytesWritten, NULL);
      // Read stdin
      while (fgets(szInputBuffer, sizeof(szInputBuffer),stdin)) {
        // Strip newline fro fgets return
        szInputBuffer[strcspn(szInputBuffer, "\r\n")] = 0;
        //printf("READ: %s\n", szInputBuffer);
        PathQuoteSpaces(szInputBuffer);
        WriteFile(hTempFile, " ", strlen(" "), &iBytesWritten, NULL);
        WriteFile(hTempFile, szInputBuffer, strlen(szInputBuffer), &iBytesWritten, NULL);
        //lstrcat(szFullCommand, szInputBuffer);
        //lstrcat(szFullCommand, " ");
        //printf("QUOTED: %s\n", szInputBuffer);
      }
      CloseHandle(hTempFile);
      //printf("%s\n",szFullCommand);
#ifdef VIM_CONSOLE // Using console vim
      // Reset stdin to FILE_TYPE_CHAR for console vim
      freopen("CONIN$", "r", stdin);
      // Reset stdout for pdcurses
      freopen("CONOUT$", "w", stdout);
#endif
      // Execute .bat file
      printf("%s\n",szTempBatchFileName); 
      system(szTempBatchFileName);
      ExitProcess(0);
    }
    printf("No input detected from standard in.\n");
    ExitProcess(1);
}
