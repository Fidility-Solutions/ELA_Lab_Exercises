#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
    FILE *io,*iodir,*ioval;

    io = fopen("/sys/class/gpio/export", "w");
    fseek(io,0,SEEK_SET);
    fprintf(io,"%d",53);
    fflush(io);

    iodir = fopen("/sys/class/gpio/gpio53/direction", "w");
    fseek(iodir,0,SEEK_SET);
    fprintf(iodir,"out");
    fflush(iodir);

    ioval = fopen("/sys/class/gpio/gpio53/value", "w");
    fseek(ioval,0,SEEK_SET);

    while(1)
    {
        fprintf(ioval,"%d",1);
        fflush(ioval);
        sleep(1);
        fprintf(ioval,"%d",0);
        fflush(ioval);
        sleep(1);
    }

    fclose(io);
    fclose(iodir);
    fclose(ioval);
    return 0;
}
