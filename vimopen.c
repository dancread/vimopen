#include <windows.h>
#include <string.h>
#include <shlwapi.h>
#include <stdio.h>

/*
 * Consider
 * https://support.microsoft.com/en-us/help/830473/command-prompt-cmd.-exe-command-line-string-limitation
 */
/*
 * user remote gvim server to open lots of files
 * gvim --servername foo --remote edi-tokenization-service\pom.xml
 */
#define VIM_EXE "gvim.exe"
#define VIM_OPTS "--servername vimopen "
INT main() {
    CHAR szCommand[MAX_PATH];
    CHAR szInputBuffer[MAX_PATH];
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    // If piped input, then pipe it to the empty file
    if(GetFileType(GetStdHandle(STD_INPUT_HANDLE)) == FILE_TYPE_PIPE){
      ShellExecute(NULL, "open",VIM_EXE, VIM_OPTS, NULL, SW_SHOWDEFAULT);
      // Read stdin
      while (fgets(szInputBuffer, sizeof(szInputBuffer),stdin)) {
        // Strip newline fro fgets return
        szInputBuffer[strcspn(szInputBuffer, "\r\n")] = 0;
        PathQuoteSpaces(szInputBuffer);
        lstrcpyn(szCommand, VIM_OPTS, MAX_PATH);
        lstrcat(szCommand, "--remote ");
        lstrcat(szCommand, szInputBuffer);
        ShellExecute(NULL, "open",VIM_EXE, szCommand, NULL, SW_SHOWDEFAULT);
      }
      ExitProcess(0);
    }
    printf("No input detected from standard in.\n");
    ExitProcess(1);
}
