/* Includes */
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
/* ---------------------------------*/

/* Data */
struct termios orig_termios;

/* ---------------------------------*/

/* Functions */

/* Prints error and terminates */
void die(const char *str){
    perror(str);
    exit(1);
}

/* Upon exiting, reverses the termios struct back to it's original. */
void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    die("tcsetattr");
} 

/* Sets a bunch of flags off to enable raw mode */
void enableRawMode() {
    
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr"); 
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    raw.c_iflag &= ~(ICRNL | IXON | BRKINT | INPCK | ISTRIP);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag &= ~(CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcgetattr");
}
/* ---------------------------------*/

/* init */

/* Loops and recieves input, printing the input to the terminal */
int main() {
    enableRawMode();

    while (1) {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
        if (iscntrl(c)) {
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);
        }
        if (c == 'q') break;
    }
    return 0;
}
