#include <windows.h>
#include <stdio.h>

/*
 * Consider
 * https://jira3.cerner.com/browse/EHOSTTXSRV-1025984?focusedCommentId=2691639&page=com.atlassian.jira.plugin.system.issuetabpanels%3Acomment-tabpanel#comment-2691639
 */
#define INPUT_BUFFER_SIZE 10000
#define VIM_CONSOLE
#ifdef VIM_CONSOLE // Using console vim
    #define VIM_EXE "vim "
#endif
#ifdef VIM_GUI // Using gui vim
    #define VIM_EXE "gvim "
#endif
INT main() {
    HANDLE hClipBoardData;
    INT iBytesRead, iBytesWritten = 0;
    CHAR wsInputBuffer[INPUT_BUFFER_SIZE];
    CHAR *ptr;
    CHAR szFullCommand[MAX_PATH+sizeof(VIM_EXE)];
    // If piped input, then pipe it to the empty file
    if(GetFileType(GetStdHandle(STD_INPUT_HANDLE)) == FILE_TYPE_PIPE){
      // Read stdin
      do{
         ReadFile(GetStdHandle(STD_INPUT_HANDLE), wsInputBuffer, INPUT_BUFFER_SIZE, &iBytesRead, NULL);
      } while(iBytesRead);
    }
    //printf("%s\n", wsInputBuffer);
    // Remove newlines an replace with spaces
    ptr = wsInputBuffer;
    while(*ptr){
      if(*ptr=='\n' || *ptr=='\r'){
        *ptr = ' ';
      }
      ptr++;
    }
#ifdef VIM_CONSOLE // Using console vim
    // Reset stdin to FILE_TYPE_CHAR for console vim
    freopen("CONIN$", "r", stdin);
    // Reset stdout for pdcurses
    freopen("CONOUT$", "w", stdout);
#endif
    // Build command path and execute vim
    lstrcpy(szFullCommand, VIM_EXE);
    lstrcat(szFullCommand, wsInputBuffer);
    system(szFullCommand);
    ExitProcess(0);
}
