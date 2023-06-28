#ifndef KBHIT_H_INCLUDED
#define KBHIT_H_INCLUDED

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

int kbhit(void);
int kbhit_no_buf(void);

#endif // KBHIT_H_INCLUDED
