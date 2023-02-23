/* Includes */
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
/* ---------------------------------*/

/* Constants */

#define CTRL_KEY(k) (k &(0x1f))

/* ---------------------------------*/

/* Data */

struct editorConfig {
    struct termios orig_termios;
};

struct editorConfig E;

/* ---------------------------------*/

/* Functions */

/* Prints error and terminates */
void die(const char *str){
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    perror(str);
    exit(1);
}

/* Upon exiting, reverses the termios struct back to it's original. */
void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios);
    die("tcsetattr");
} 

/* Sets a bunch of flags off to enable raw mode */
void enableRawMode() {
    
    if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) die("tcgetattr"); 
    atexit(disableRawMode);

    struct termios raw = E.orig_termios;
    raw.c_iflag &= ~(ICRNL | IXON | BRKINT | INPCK | ISTRIP);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag &= ~(CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcgetattr");
}

/* Reads a key from the keyboard */
char editorReadKey() {
    int nread;
    char c;
    while ((nread = read(STDERR_FILENO, &c, 1)) != 1){
        if (nread == -1 && errno != EAGAIN) die("read");   
    }
    return c;
}

void editorDrawRows() {
    int y;
    for (y = 0; y < 24; y++){
        write(STDOUT_FILENO, "~\r\n", 3);
    }
}

void editorRefreshScreen(){
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    editorDrawRows();
    write(STDOUT_FILENO, "\x1b[H", 3);
}

/* ---------------------------------*/

/* Input */

/* Processes the keypress given from editorReadKey */
void editorProcessKeypress() {
    char c = editorReadKey();

    switch (c) {
        case CTRL_KEY('q'):
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
            break;
    }
}
/* ---------------------------------*/

/* Init */

/* Loops and recieves input, printing the input to the terminal */
int main() {
    enableRawMode();
    while(1){
        editorRefreshScreen();
        editorProcessKeypress();
    }
}
