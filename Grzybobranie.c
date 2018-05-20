#include <X11/Xlib.h>
#include <X11/X.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>

const int SEM_ID = 7256;

//XLIB configuration
Display* mydisplay;
Window mywindow;
XSetWindowAttributes mywindowattributes;
XGCValues mygcvalues;
GC mygc;
Visual* myvisual;
int mydepth;
int myscreen;
Colormap mycolormap;
XColor mycolor, mycolor0, mycolor1, mycolor2, mycolor3, mycolor4, mycolor5, dummy;
XEvent myevent;
char* myname;

int pamiec;
char* adres;

typedef enum { false,
    true } bool;

//Struktura gracza
typedef struct circle {
    int x;
    int y;
    int size;
    int ID;
    bool active_player;
    bool endGame;
    int move;
    int place;
    int score;
    int turnNumber;
    int visited[13];
} circle;

//Numer biezacego gracza
int id;

void firts_player_status(circle* circles)
{
    int i;
    for (i = 1; i < 6; i++) {
        circles[i].ID = i;
        circles[i].active_player = false;
        circles[i].endGame = false;
        circles[i].turnNumber = 0;
        circles[i].place = 0;
        circles[i].score = 0;
        circles[i].visited[0] = 0;
        circles[i].visited[1] = 0;
        circles[i].visited[2] = 1;
        circles[i].visited[3] = 0;
        circles[i].visited[4] = 0;
        circles[i].visited[5] = 2;
        circles[i].visited[6] = 0;
        circles[i].visited[7] = 1;
        circles[i].visited[8] = 0;
        circles[i].visited[9] = 1;
        circles[i].visited[10] = 0;
        circles[i].visited[11] = 0;
        circles[i].visited[12] = 0;
        circles[i].visited[13] = 0;
    }
}
//zmienia ture wszytskim graczom
void changeTurn(circle* circles)
{
    int i;
    for (i = 1; i < 6; i++) {
        circles[i].turnNumber = circles[i].turnNumber + 1;
    }
}
//zmienia wartosci na mapie (odwiedzone grzyby zmienia wartosc polna na 0)
void change_map_status(circle* circles, int actualplace)
{
    int i;
    for (i = 1; i < 6; i++) {
        circles[i].visited[actualplace] = 0;
    }
}
//wraca wsporzedna Y dla gracza do zmiany pozycji w oknie
int returnY(int position, int id)
{
    int i = position;
    int temp;
    if (id == 1) {
        int tablica[] = { 125, 125, 125, 125, 125, 125, 125, 205, 285, 285, 285, 285, 285 };
        if (i > 12)
            return 365;
        return temp = tablica[i];
    }
    if (id == 2) {
        int tablica2[] = { 145, 145, 145, 145, 145, 145, 145, 225, 305, 305, 305, 305, 305 };
        if (i > 12)
            return 385;
        return temp = tablica2[i];
    }
    if (id == 3) {
        int tablica3[] = { 135, 135, 135, 135, 135, 135, 135, 225, 305, 295, 295, 295, 295 };
        if (i > 12)
            return 385;
        return temp = tablica3[i];
    }
}
//wraca wsporzedna X dla gracza do zmiany pozycji w oknie
int returnX(int position)
{
    int i = position;
    int temp;
    if (id == 3) {
        int tablica1[] = { 35, 125, 205, 285, 365, 435, 515, 515, 515, 435, 355, 285, 285 };
        if (i > 12)
            return 285;
        return temp = tablica1[i];
    }

    int tablica[] = { 15, 105, 185, 265, 345, 415, 495, 495, 495, 415, 335, 265, 265 };
    if (i > 12)
        return 265;
    return temp = tablica[i];
}

//Funkcja dodajaca nowego gracza do gry (zmiana statusu active_player)
int new_player(circle* circles)
{

    int i;

    for (i = 1; i < 3; i++) {
        if (circles[i].active_player == false) {
            circles[i].active_player = true;
            break;
        }
    }
    return i;
}
//Funkcja losujaca wartosc (kostki do gry od 1 do 6)
int number(int id)
{
    srand(time(0));
    int r;
    r = rand() % 6 + 1;
    return r;
}

//Funkcja wyswietlajaca graczy i stan mapy
void overview_game(circle* circles, int id)
{

    XSetForeground(mydisplay, mygc, mycolor4.pixel);
    XFillArc(mydisplay, mywindow, mygc, 15, 115, 60, 60, 0, 360 * 64);
    XFillArc(mydisplay, mywindow, mygc, 95, 115, 60, 60, 0, 360 * 64);
    XFillArc(mydisplay, mywindow, mygc, 175, 115, 60, 60, 0, 360 * 64);
    XFillArc(mydisplay, mywindow, mygc, 255, 115, 60, 60, 0, 360 * 64);
    XFillArc(mydisplay, mywindow, mygc, 335, 115, 60, 60, 0, 360 * 64);
    XFillArc(mydisplay, mywindow, mygc, 405, 115, 60, 60, 0, 360 * 64);
    //grzyby pole[2]
    if (circles[id].visited[2] > 0) {
        XSetForeground(mydisplay, mygc, mycolor5.pixel);
        XFillArc(mydisplay, mywindow, mygc, 195, 130, 20, 20, 0, 360 * 64);
    }
    //grzyby pole[5]
    if (circles[id].visited[5] > 0) {
        XSetForeground(mydisplay, mygc, mycolor5.pixel);
        XFillArc(mydisplay, mywindow, mygc, 425, 120, 20, 20, 0, 360 * 64);
        XFillArc(mydisplay, mywindow, mygc, 425, 145, 20, 20, 0, 360 * 64);
    }
    XSetForeground(mydisplay, mygc, mycolor4.pixel);
    XFillArc(mydisplay, mywindow, mygc, 485, 115, 60, 60, 0, 360 * 64);
    XFillArc(mydisplay, mywindow, mygc, 485, 195, 60, 60, 0, 360 * 64);
    //grzyb pole[7]

    if (circles[id].visited[7] > 0) {
        XSetForeground(mydisplay, mygc, mycolor5.pixel);
        XFillArc(mydisplay, mywindow, mygc, 505, 215, 20, 20, 0, 360 * 64);
    }
    XSetForeground(mydisplay, mygc, mycolor4.pixel);
    XFillArc(mydisplay, mywindow, mygc, 485, 275, 60, 60, 0, 360 * 64);
    XFillArc(mydisplay, mywindow, mygc, 405, 275, 60, 60, 0, 360 * 64);

    //grzyb pole[9]
    if (circles[id].visited[9] > 0) {
        XSetForeground(mydisplay, mygc, mycolor5.pixel);
        XFillArc(mydisplay, mywindow, mygc, 425, 295, 20, 20, 0, 360 * 64);
    }
    XSetForeground(mydisplay, mygc, mycolor4.pixel);
    XFillArc(mydisplay, mywindow, mygc, 325, 275, 60, 60, 0, 360 * 64);
    XFillArc(mydisplay, mywindow, mygc, 255, 275, 60, 60, 0, 360 * 64);
    XSetForeground(mydisplay, mygc, mycolor.pixel);
    XFillArc(mydisplay, mywindow, mygc, 255, 355, 60, 60, 0, 360 * 64);
    XSetForeground(mydisplay, mygc, mycolor5.pixel);

    //rozstrzyganie wyniku gry
    
    if (number_of_end(circles) == number_of_players(circles)) {
        int i;
        int max = 0;
        int winID;
        if (circles[1].score == circles[2].score) {
            char buf1[10];
            sprintf(buf1, "REMIS", winID, max);
            XDrawString(mydisplay, mywindow, mygc, 220, 240, buf1, strlen(buf1));
        }
        else {
            for (i = 1; i < 6; i++) {
                if (circles[i].score > max) {
                    max = circles[i].score;
                    winID = i;
                }
            }
            char buf[50];
            sprintf(buf, "wygral gracz z ID %d zdobyl %d punktow", winID, max);
            XDrawString(mydisplay, mywindow, mygc, 220, 240, buf, strlen(buf));
        }
    }

    int i;
    char napis[2]; //punkty gracza
    char legendaID[1]; // ID gracza

    if (id == 3) {
        printf("Nie mozesz dolaczyc do gry! Max liczba graczy to: %d\n", id - 1);
    }

    else {

        for (i = 1; i < 6; i++) {
            if (circles[i].active_player == true) {

                //Drukowanie graczy

                if (circles[i].ID == 1) {
                    XSetForeground(mydisplay, mygc, mycolor0.pixel);
                    sprintf(napis, "%d", circles[i].ID);
                    XDrawString(mydisplay, mywindow, mygc, circles[i].x, circles[i].y, napis, strlen(napis));
                    XFillArc(mydisplay, mywindow, mygc, 10, 300, 40, 40, 0, 360 * 64);
                    sprintf(legendaID, "%d", circles[i].score);
                    XDrawString(mydisplay, mywindow, mygc, 10, 300, legendaID, strlen(legendaID));
                }
                if (circles[i].ID == 2) {
                    XSetForeground(mydisplay, mygc, mycolor2.pixel);
                    sprintf(napis, "%d", circles[i].ID);
                    XDrawString(mydisplay, mywindow, mygc, circles[i].x, circles[i].y, napis, strlen(napis));
                    XFillArc(mydisplay, mywindow, mygc, 10, 350, 40, 40, 0, 360 * 64);
                    sprintf(legendaID, "%d", circles[i].score);
                    XDrawString(mydisplay, mywindow, mygc, 10, 350, legendaID, strlen(legendaID));
                }
                if (circles[i].ID == 123) {
                    XSetForeground(mydisplay, mygc, mycolor1.pixel);
                    sprintf(napis, "%d", circles[i].ID);
                    XDrawString(mydisplay, mywindow, mygc, circles[i].x, circles[i].y, napis, strlen(napis));
                    XFillArc(mydisplay, mywindow, mygc, 10, 400, 40, 40, 0, 360 * 64);
                    sprintf(legendaID, "%d", circles[i].score);
                    XDrawString(mydisplay, mywindow, mygc, 10, 400, legendaID, strlen(legendaID));
                }
                if (circles[i].ID == 4) {
                    XSetForeground(mydisplay, mygc, mycolor4.pixel);
                    sprintf(napis, "%d", circles[i].ID);
                    XDrawString(mydisplay, mywindow, mygc, circles[i].x, circles[i].y, napis, strlen(napis));
                }
                if (circles[i].ID == 5) {
                    XSetForeground(mydisplay, mygc, mycolor5.pixel);
                    sprintf(napis, "%d", circles[i].ID);
                    XDrawString(mydisplay, mywindow, mygc, circles[i].x, circles[i].y, napis, strlen(napis));
                }

                XFillArc(mydisplay, mywindow, mygc, circles[i].x, circles[i].y, circles[i].size - 40, circles[i].size - 40, 0, 360 * 64);
                XFlush(mydisplay);
            }
        }
    }
}
//Ilosc aktywnych graczy
int number_of_players(circle* circles)
{

    int i, number_players = 0;

    for (i = 1; i < 6; i++) {
        if (circles[i].active_player == true) {
            number_players++;
        }
    }
    return number_players;
}
//ilosc graczy ktorzy zakonczyli gre
int number_of_end(circle* circles)
{

    int i, number_players = 0;

    for (i = 1; i < 6; i++) {
        if (circles[i].endGame == true) {
            number_players++;
        }
    }
    return number_players;
}

//Funkcja obslugujaca rozgrywke, wyjscie z programu
int game(circle* circles, int id)
{

    int xw, yw;
    int turn = 1;
    int numberofplayers;

    while (1) {
        XSetForeground(mydisplay, mygc, mycolor3.pixel);
        XFillRectangle(mydisplay, mywindow, mygc, 0, 0, 640, 480);

        overview_game(circles, id);
        usleep(5000);

        if (XPending(mydisplay) > 0) {

            XNextEvent(mydisplay, &myevent);
            switch (myevent.type) {

            case Expose:

                //PUNKT STARTOWY
                if (id == 1) {
                    circles[id].x = 25;
                    circles[id].y = 120;
                    circles[id].size = 60;
                }
                if (id == 2) {
                    circles[id].x = 40;
                    circles[id].y = 130;
                    circles[id].size = 60;
                }
                if (id == 3) {
                    circles[id].x = 40;
                    circles[id].y = 150;
                    circles[id].size = 60;
                }
                break;

            //OBSLUGA KLAWISZY
            case KeyPress:

                //WYJSCIE Z PROGRAMU (KLAWISZ ESC)
                if (myevent.xkey.keycode == 0x09) {
                    XCloseDisplay(mydisplay);
                    shmdt(adres);
                    shmctl(pamiec, IPC_RMID, NULL);
                    exit(0);
                }

                //STEROWANIE ZA POMOCA KLAWIATURY
                //STRZALKA W PRAWO
                if (myevent.xkey.keycode == 0x72 && id == (circles[1].turnNumber % 2) + 1) {
                    XSetForeground(mydisplay, mygc, mycolor.pixel);
                    XFillArc(mydisplay, mywindow, mygc, circles[id].x, circles[id].y, circles[id].size, circles[id].size, 0, 360 * 64);

                    int t = number(id);

                    circles[id].place = circles[id].place + t;

                    circles[id].x = returnX(circles[id].place);
                    circles[id].y = returnY(circles[id].place, id);
                    if (circles[id].place >= 13) {
                        circles[id].endGame = true;

                    }
                    if (circles[id].visited[circles[id].place] > 0 && circles[id].place < 12) {
                        circles[id].score = circles[id].score + circles[id].visited[circles[id].place];
                        change_map_status(circles, circles[id].place);
                    }

                    printf("numery planszy: %d\n", circles[id].place);

                    XFillArc(mydisplay, mywindow, mygc, circles[id].x, circles[id].y, circles[id].size, circles[id].size, 0, 360 * 64);
                    changeTurn(circles);
                    XFlush(mydisplay);
                }

            } 
        } 
    } 
} 

//Glowna funkcja main
int main(int argc, char** argv)
{

    circle* adres;

    id = 1;
    pamiec = shmget(SEM_ID, 1024, 0777 | IPC_CREAT | IPC_EXCL);

    printf("pamiec: %d\n", pamiec);

    if (pamiec == -1) {
        id++;
    }

    pamiec = shmget(SEM_ID, 1024, 0777 | IPC_CREAT);

    printf("pamiec2: %d\n", pamiec);

    adres = shmat(pamiec, 0, 0);

    printf("adres: %d\n", adres);

    if (id == 1) {
        firts_player_status(adres);
    }

    id = new_player(adres);

    //XLIB setup
    mydisplay = XOpenDisplay("");
    myscreen = DefaultScreen(mydisplay);
    myvisual = DefaultVisual(mydisplay, myscreen);
    mydepth = DefaultDepth(mydisplay, myscreen);
    mywindowattributes.background_pixel = XWhitePixel(mydisplay, myscreen);
    mywindowattributes.override_redirect = False;
    mywindowattributes.backing_store = Always;
    mywindowattributes.bit_gravity = NorthWestGravity;
    mywindow = XCreateWindow(mydisplay, XRootWindow(mydisplay, myscreen),
        100, 100, 640, 480, 10, mydepth, InputOutput,
        myvisual, CWBackingStore | CWBackingPlanes | CWBitGravity | CWBackPixel | CWOverrideRedirect, &mywindowattributes);
    XSelectInput(mydisplay, mywindow, ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonMotionMask);
    mycolormap = DefaultColormap(mydisplay, myscreen);
    XAllocNamedColor(mydisplay, mycolormap, "white", &mycolor, &dummy); //tlo
    XAllocNamedColor(mydisplay, mycolormap, "red", &mycolor1, &dummy); //berek
    XAllocNamedColor(mydisplay, mycolormap, "orange", &mycolor0, &dummy); //1 gracz (kolor dla 1 gracza gdy nie jest berkiem)
    XAllocNamedColor(mydisplay, mycolormap, "yellow", &mycolor2, &dummy); //2 gracz
    XAllocNamedColor(mydisplay, mycolormap, "green", &mycolor3, &dummy); //3 gracz
    XAllocNamedColor(mydisplay, mycolormap, "blue", &mycolor4, &dummy); //4 gracz
    XAllocNamedColor(mydisplay, mycolormap, "black", &mycolor5, &dummy); //5 gracz

    char buf[12];
    sprintf(buf, "Gzybobranie %d", id);
    myname = "Grzybobranie";
    XStoreName(mydisplay, mywindow, buf);
    XMapWindow(mydisplay, mywindow);
    mygc = DefaultGC(mydisplay, myscreen);

    game(adres, id);

    return 0;
}
