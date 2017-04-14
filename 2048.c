#include<ncurses.h> 
#include<stdio.h> 
#include<string.h> 
#include<ctype.h> 
#include<sys/select.h> 
#include<stdlib.h> 
#include<time.h> 
#include<signal.h> 
#include<unistd.h>
			 
int coordy(int ii) { //functia genereaza coordonata y corespunzatoare fiecarei celule de pe tabla de joc
    int y;
    if (ii == 0)
        y = 5;
    if (ii == 1)
        y = 7;
    if (ii == 2)
        y = 9;
    if (ii == 3)
        y = 11;
    return y;
    }

int coordx(int jj) {//functia genereaza coordonata x corespunzatoare fiecarei celule de pe tabla de joc
    int x;
    if (jj == 0)
        x = 18;
    if (jj == 1)
        x = 25;
    if (jj == 2)
        x = 32;
    if (jj == 3)
        x = 39;
    return x;
}

int matrixCoordinates(int val) {//returneaza valori aleatorii pentru ii si jj
    int nr;
    nr = rand() % val;
    return nr;
}

int randomNumbers(int num) {//functia generaza 2 sau 4 
    int nr;
    while (nr == 0 || nr % 2 == 1) {
        nr = rand() % num;
    }
    return nr;
}

char generateRandomMove(void){ //functia generaza o apasare falsa 
	int nr;
	char key = 'a';
	nr = (int) rand() % 4; //genereaza numere aleatorii intre 0 si 3
	switch(nr){ // transforma numarul intr-o tasta
		case 0:
			key = 'a';
			break;
		case 1:
			key = 's';
			break;
		case 2:
			key = 'd';
			break;
		case 3:
			key = 'w';
			break;
		default:
		    break;
	}
	return key; //returneaza tasta transformata
}

int printTime(int firstRun, int *timeSpentPlaying) {//functia calculeaza si printeaza timpul si intervalul de timp petrecut in joc
	time_t currentTime;
	static time_t startTime; //folosit ca sa evitam variabilele globale
	if(firstRun == 1){//daca jocul este nou(nu Resume) recalculeaza timpul la care s-a intrat in joc
		time(&startTime);
	}
	time(&currentTime);
	mvprintw(0, 0, "%s", ctime(&currentTime));// timpul este intotdeauna printat la coordonatele x=0 si y=0
	*timeSpentPlaying = (int) difftime(currentTime, startTime); //calculeaza timpul petrecut in joc
	mvprintw(1, 0, "%s", "Play time: ");
	mvprintw(1, 15, "%s", " seconds");
	mvprintw(1, 11, "%d", *timeSpentPlaying); // printeaza timpul petrecut in joc
    return 0;
}
int printScore(int firstRun, int *score){ //functia printeaza scorul
	if(firstRun == 1) //daca jocul este nou(nu Resume) reseteaza scorul
		*score = 0;
	
	mvprintw(2, 0, "%s", "Score: ");//actualizeaza scorul
	mvprintw(2, 11, "%s", " points");
	mvprintw(2, 8, "%d", *score); //score is a global variable
return 0;
}

int drawHelp(){ //functia afiseaza o fereastra pentru optiunea help
	initscr();
    clear();
    noecho();
	mvprintw(0, 0, "%s", "HELP MENU");
	
	mvprintw(2, 0, "%s", "Controls:");
	mvprintw(4, 2, "%s", "w / arrow up - move up");
	mvprintw(5, 2, "%s", "s / arrow down - move down");
	mvprintw(6, 2, "%s", "a / arrow left - move left");
	mvprintw(7, 2, "%s", "d / arrow right - move right");
	mvprintw(9, 2, "%s", "q - quit to terminal");
	
	mvprintw(11, 0, "%s", "Author: Stroinea Ioana");

	while(getch() != 'q');
	refresh();
	clear();
	return 0;
}

int drawBoard(int firstRun) { // functia deseneaza tabla de joc si celulele
	int i = 0;
	int h = 0;
    initscr();
    clear();
    noecho();
	nodelay(stdscr, TRUE); 
    start_color();
    for (i = 4; i <= 12; i++) {
        mvaddch(i, 16, '|');
        mvaddch(i, 23, '|');
        mvaddch(i, 30, '|');
        mvaddch(i, 37, '|');
        mvaddch(i, 44, '|');
        if (i % 2 == 0) {
            for (h = 16; h <= 44; h++) {
                mvaddch(i, h, '-');
            }
        }
    }
return 0;
}


//functia playGame e folosita pentru a juca(actualizeaza celulele,timpul, timpul petrecut in joc si scorul
int playGame(int firstRun, int *timeSpentPlaying, int *score, int **a){
	int i, h, num = 5, k, ii, jj, val = 4, nn = 0, y, x, b, c, i1, i2, j1, j2, ch, ok = 0, contord = 0, contors = 0, contorw = 0, contora = 0;
	int timeSinceLastMove = 0;
	keypad(stdscr, TRUE); //ofera posibilitatea ca tastatura sa merga, daca optiunea nu exista nu se pot folosi tastele pentru directii
    curs_set(0); //ascunde cursorul
	start_color();
	printTime(firstRun, timeSpentPlaying); //sa poata fi evitata folosirea variabilelor globale
	
if(firstRun == 1){ //daca e prima data cand jucam(New Game), genereaza numere aleatorii si incepe jocul
    while (nn < 2) {
        ii = matrixCoordinates(val);
        jj = matrixCoordinates(val);
        a[ii][jj] = randomNumbers(num);
        y = coordy(ii);
        x = coordx(jj);
        if (nn == 0) {
            i1 = ii;
            j1 = jj;
        }
        if (nn == 1) {
            i2 = ii;
            j2 = jj;
        }
        if (nn > 0 && c == x && b == y)
            nn--;
        else
            mvprintw(y, x, "%d", a[ii][jj]);
        b = y;
        c = x;
        nn++;
    }

    for (ii = 0; ii < 4; ii++)
        for (jj = 0; jj < 4; jj++) {
            if ((ii != i1 || jj != j1) && (ii != i2 || jj != j2))//cand coordonatele numerelor generate aleator sunt diferite de cele ale unei celule, matricea ia valoarea 0
                a[ii][jj] = 0;
            if (ii == i1 && jj == j1)//altfel se initializeaza cu numarul generat 2 sau 4
                a[ii][jj] = a[i1][j1];
            if (ii == i2 && jj == j2)
                a[ii][jj] = a[i2][j2];
        }
    for (ii = 0; ii < 4; ii++)
        for (jj = 0; jj < 4; jj++) {
            y = coordy(ii);
            x = coordx(jj);
            if (a[ii][jj] == 0) {//cand matricea are valoarea 0, se printeaza spatii libere
                mvaddch(y, x, ' ');
                mvaddch(y, x + 1, ' ');
                mvaddch(y, x + 2, ' ');
                mvaddch(y, x + 3, ' ');
            }
        }
	}
	
	else{ //daca nu e prima data cand se joaca(Resume) , printeaza vechea matrice pe ecran
		for (i = 0; i < 4 ; i++){
			for (h = 0; h < 4; h++){  
				if(a[i][h] == 0){
					mvprintw(coordy(i), coordx(h), "%c", ' '); //umple cu spatii
				}
				else{
					mvprintw(coordy(i), coordx(h), "%d", a[i][h]); //umple tabla cu ultimele date retinute
				}
			}
		}
	}
	
    while (ch != 'q') { //acum incepe jocul sa mearga
		mvprintw(16, 0, "%s", "Info: To play the game use w,a,s,d keys or arrow keys."); //informatii despre joc
		mvprintw(17, 6, "%s", "To exit the game use q key.");
		
		timeSinceLastMove = *timeSpentPlaying; //calculeaza timpul trecut de la ultima miscare
		
		do{
			ch = getch(); //citeste urmatorul caracter
			printTime(firstRun, timeSpentPlaying); //printeaza ora si timpul petrecut in joc
			printScore(firstRun, score); //printeaza scorul
			firstRun = 0; 
			if((*timeSpentPlaying - timeSinceLastMove) >= 10) //if for 15 seconds no key is pressed (just change the number to increase or decrease the idle time)//daca timp de 10 secunde nu se face nicio miscare, celulele se misca automat
			{
				ch = generateRandomMove(); // simuleaza o apasare de tasta
				break; 
			}
		}while(ch == ERR); //ch==ERR inseamna ca nu se apasa niciun buton.Daca se apasa o tasta atunci aceasta va fi retinuta in ch si se va iesi din ciclul do...while
        
		timeSinceLastMove = 0; //reseteaza timpul pentru ca a fost apasata alta tasta
		
		switch (ch) {
        case 'w':
		case KEY_UP:
            {
                for (ii = 0; ii < 4; ii++)//daca se afla o celula libera desupra uneia diferita de 0 atunci ele fac schimb de valori
                    for (jj = 0; jj < 4; jj++) {
                        if (!a[jj][ii]) {
                            for (k = jj + 1; k < 4; k++)
                                if (a[k][ii]) {
                                    a[jj][ii] = a[k][ii];
                                    a[k][ii] = 0;
                                    break;
                                }
                        }
                    }

                for (ii = 0; ii < 4; ii++)//printeaza matricea dupa miscare prin apasarea tastei w
                    for (jj = 0; jj < 4; jj++) {
                        y = coordy(ii);
                        x = coordx(jj);
                        if (a[ii][jj] != 0) {
                            mvaddch(y, x, ' ');
                            mvaddch(y, x + 1, ' ');
                            mvaddch(y, x + 2, ' ');
                            mvaddch(y, x + 3, ' ');
                            mvprintw(y, x, "%d", a[ii][jj]);
                        } else {
                            mvaddch(y, x, ' ');
                            mvaddch(y, x + 1, ' ');
                            mvaddch(y, x + 2, ' ');
                            mvaddch(y, x + 3, ' ');
                        }
                    }
                break;
            }

        case 's':
		case KEY_DOWN:
            {
                for (ii = 0; ii < 4; ii++)//daca se afla o celula libera sub una diferita de 0 atunci ele fac schimb de valori
                    for (jj = 3; jj >= 0; jj--) {
                        if (!a[jj][ii]) {
                            for (k = jj - 1; k >= 0; k--)
                                if (a[k][ii]) {
                                    a[jj][ii] = a[k][ii];
                                    a[k][ii] = 0;

                                    break;
                                }
                        }

                    }

                for (ii = 0; ii < 4; ii++)//printeaza matricea dupa miscare prin apasarea tastei s
                    for (jj = 0; jj < 4; jj++) {
                        y = coordy(ii);
                        x = coordx(jj);
                        if (a[ii][jj] != 0) {
                            mvaddch(y, x, ' ');
                            mvaddch(y, x + 1, ' ');
                            mvaddch(y, x + 2, ' ');
                            mvaddch(y, x + 3, ' ');
                            mvprintw(y, x, "%d", a[ii][jj]);
                        } else {
                            mvaddch(y, x, ' ');
                            mvaddch(y, x + 1, ' ');
                            mvaddch(y, x + 2, ' ');
                            mvaddch(y, x + 3, ' ');
                        }
                    }
                break;
            }
        case 'a':
		case KEY_LEFT:
            {
                for (ii = 0; ii < 4; ii++)//daca se afla o celula libera in stanga uneia diferita de 0 atunci ele fac schimb de valori
                    for (jj = 0; jj < 4; jj++) {
                        if (!a[ii][jj]) {
                            for (k = jj + 1; k < 4; k++)
                                if (a[ii][k]) {
                                    a[ii][jj] = a[ii][k];
                                    a[ii][k] = 0;

                                    break;
                                }

                        }

                    }

                for (ii = 0; ii < 4; ii++)//printeaza matricea dupa miscare prin apasarea tastei a
                    for (jj = 0; jj < 4; jj++) {
                        y = coordy(ii);
                        x = coordx(jj);
                        if (a[ii][jj] != 0) {
                            mvaddch(y, x, ' ');
                            mvaddch(y, x + 1, ' ');
                            mvaddch(y, x + 2, ' ');
                            mvaddch(y, x + 3, ' ');
                            mvprintw(y, x, "%d", a[ii][jj]);
                        } else {
                            mvaddch(y, x, ' ');
                            mvaddch(y, x + 1, ' ');
                            mvaddch(y, x + 2, ' ');
                            mvaddch(y, x + 3, ' ');
                        }
                    }
                break;
            }
        case 'd':
		case KEY_RIGHT:
            {
                for (ii = 0; ii < 4; ii++)//daca se afla o celula libera in stanga uneia diferita de 0 atunci ele fac schimb de valori
                    for (jj = 3; jj >= 0; jj--) {
                        if (!a[ii][jj]) {
                            for (k = jj - 1; k >= 0; k--)
                                if (a[ii][k]) {
                                    a[ii][jj] = a[ii][k];
                                    a[ii][k] = 0;

                                    break;
                                }
                        }

                    }

                for (ii = 0; ii < 4; ii++)//printeaza matricea dupa miscare prin apasarea tastei d
                    for (jj = 0; jj < 4; jj++) {
                        y = coordy(ii);
                        x = coordx(jj);
                        if (a[ii][jj] != 0) {
                            mvaddch(y, x, ' ');
                            mvaddch(y, x + 1, ' ');
                            mvaddch(y, x + 2, ' ');
                            mvaddch(y, x + 3, ' ');
                            mvprintw(y, x, "%d", a[ii][jj]);
                        } else {
                            mvaddch(y, x, ' ');
                            mvaddch(y, x + 1, ' ');
                            mvaddch(y, x + 2, ' ');
                            mvaddch(y, x + 3, ' ');
                        }
                    }
                break;
            }

        }

        switch (ch) {
        case 'w':
		case KEY_UP:
            {
                for (ii = 0; ii < 4; ii++)//cand se intalnesc doua celule consecutive de aceeasi valoare,ele se aduna
                    for (jj = 0; jj < 3; jj++) {
                        if (a[jj][ii] && a[jj][ii] == a[jj + 1][ii]) {
                            a[jj][ii] = a[jj][ii] + a[jj + 1][ii];
                            a[jj + 1][ii] = 0;
							*score = *score + a[jj][ii]; // actualizeaza scorul pentru celulele imbinate
                        }

                    }
                for (ii = 0; ii < 4; ii++)//verifica daca dupa imbinari raman celule goale intre cele imbinate, iar daca exista toate cele diferite de 0 sunt trecute in directia mutarii
                    for (jj = 0; jj < 3; jj++)
                        if (a[jj][ii] == 0 && a[jj + 1][ii] != 0) {
                            a[jj][ii] = a[jj + 1][ii];
                            a[jj + 1][ii] = 0;
                        }

                for (ii = 0; ii < 4; ii++)//printeaza matricea dupa miscare prin apasarea tastei w
                    for (jj = 0; jj < 4; jj++) {
                        y = coordy(ii);
                        x = coordx(jj);
                        if (a[ii][jj] != 0) {
                            mvaddch(y, x, ' ');
                            mvaddch(y, x + 1, ' ');
                            mvaddch(y, x + 2, ' ');
                            mvaddch(y, x + 3, ' ');
                            mvprintw(y, x, "%d", a[ii][jj]);
                        } else {
                            mvaddch(y, x, ' ');
                            mvaddch(y, x + 1, ' ');
                            mvaddch(y, x + 2, ' ');
                            mvaddch(y, x + 3, ' ');
                        }
                    }
                break;
            }

        case 's':
		case KEY_DOWN:
            {

                for (ii = 0; ii < 4; ii++)//cand se intalnesc doua celule consecutive de aceeasi valoare,ele se aduna
                    for (jj = 3; jj > 0; jj--) {
                        if (a[jj][ii] && a[jj][ii] == a[jj - 1][ii]) {

                            a[jj][ii] = a[jj][ii] + a[jj - 1][ii];
                            a[jj - 1][ii] = 0;
							*score = *score + a[jj][ii]; // actualizeaza scorul pentru celulele imbinate
                        }
                    }
                for (ii = 0; ii < 4; ii++)//verifica daca dupa imbinari raman celule goale intre cele imbinate, iar daca exista toate cele diferite de 0 sunt trecute in directia mutarii
                    for (jj = 3; jj > 0; jj--)
                        if (a[jj][ii] == 0 && a[jj - 1][ii] != 0) {
                            a[jj][ii] = a[jj - 1][ii];
                            a[jj - 1][ii] = 0;
                        }

                for (ii = 0; ii < 4; ii++)//printeaza matricea dupa miscare prin apasarea tastei s
                    for (jj = 0; jj < 4; jj++) {
                        y = coordy(ii);
                        x = coordx(jj);
                        if (a[ii][jj] != 0) {
                            mvaddch(y, x, ' ');
                            mvaddch(y, x + 1, ' ');
                            mvaddch(y, x + 2, ' ');
                            mvaddch(y, x + 3, ' ');
                            mvprintw(y, x, "%d", a[ii][jj]);
                        } else {
                            mvaddch(y, x, ' ');
                            mvaddch(y, x + 1, ' ');
                            mvaddch(y, x + 2, ' ');
                            mvaddch(y, x + 3, ' ');
                        }
                    }
                break;
            }
        case 'a':
		case KEY_LEFT:
            {
                for (ii = 0; ii < 4; ii++)//cand se intalnesc doua celule consecutive de aceeasi valoare,ele se aduna
                    for (jj = 0; jj < 3; jj++) {
                        if (a[ii][jj] && a[ii][jj] == a[ii][jj + 1]) {
                            a[ii][jj] = a[ii][jj] + a[ii][jj + 1];
                            a[ii][jj + 1] = 0;
							*score = *score + a[ii][jj]; // actualizeaza scorul pentru celulele imbinate
                        }
                    }
                for (ii = 0; ii < 4; ii++)//verifica daca dupa imbinari raman celule goale intre cele imbinate, iar daca exista toate cele diferite de 0 sunt trecute in directia mutarii
                    for (jj = 0; jj < 3; jj++)
                        if (a[ii][jj] == 0 && a[ii][jj + 1] != 0) {
                            a[ii][jj] = a[ii][jj + 1];
                            a[ii][jj + 1] = 0;
                        }

                for (ii = 0; ii < 4; ii++)//printeaza matricea dupa miscare prin apasarea tastei a
                    for (jj = 0; jj < 4; jj++) {
                        y = coordy(ii);
                        x = coordx(jj);
                        if (a[ii][jj] != 0) {
                            mvaddch(y, x, ' ');
                            mvaddch(y, x + 1, ' ');
                            mvaddch(y, x + 2, ' ');
                            mvaddch(y, x + 3, ' ');
                            mvprintw(y, x, "%d", a[ii][jj]);
                        } else {
                            mvaddch(y, x, ' ');
                            mvaddch(y, x + 1, ' ');
                            mvaddch(y, x + 2, ' ');
                            mvaddch(y, x + 3, ' ');
                        }
                    }
                break;
            }
        case 'd':
		case KEY_RIGHT:
            {
                for (ii = 0; ii < 4; ii++)//cand se intalnesc doua celule consecutive de aceeasi valoare,ele se aduna
                    for (jj = 3; jj > 0; jj--) {
                        if (a[ii][jj] && a[ii][jj] == a[ii][jj - 1]) {

                            a[ii][jj] = a[ii][jj] + a[ii][jj - 1];
                            a[ii][jj - 1] = 0;
							*score = *score + a[ii][jj]; // actualizeaza scorul pentru celulele imbinate
                        }
                    }
                for (ii = 0; ii < 4; ii++)//verifica daca dupa imbinari raman celule goale intre cele imbinate, iar daca exista toate cele diferite de 0 sunt trecute in directia mutarii
                    for (jj = 3; jj > 0; jj--)
                        if (a[ii][jj] == 0 && a[ii][jj - 1] != 0) {
                            a[ii][jj] = a[ii][jj - 1];
                            a[ii][jj - 1] = 0;
                        }

                for (ii = 0; ii < 4; ii++)//printeaza matricea dupa miscare prin apasarea tastei d
                    for (jj = 0; jj < 4; jj++) {
                        y = coordy(ii);
                        x = coordx(jj);
                        if (a[ii][jj] != 0) {
                            mvaddch(y, x, ' ');
                            mvaddch(y, x + 1, ' ');
                            mvaddch(y, x + 2, ' ');
                            mvaddch(y, x + 3, ' ');
                            mvprintw(y, x, "%d", a[ii][jj]);
                        } else {
                            mvaddch(y, x, ' ');
                            mvaddch(y, x + 1, ' ');
                            mvaddch(y, x + 2, ' ');
                            mvaddch(y, x + 3, ' ');
                        }
                    }
                break;
            }

        }

        switch (ch) {
        case 'w':
		case KEY_UP:
            for (ii = 0; ii < 4; ii++)
                for (jj = 0; jj < 4; jj++)
                    if (a[ii][jj] == 0)
                        ok++;

            if (ok != 0 && contorw != 12) {//genereaza aleator 2 sau 4 in una din pozitiile in care matricea are valoarea 0 , daca s-a efectuat o miscare valida
                do {
                    ii = matrixCoordinates(val);
                    jj = matrixCoordinates(val);
                } while (a[ii][jj] != 0);
                a[ii][jj] = randomNumbers(num);
                y = coordy(ii);
                x = coordx(jj);
                mvprintw(y, x, "%d", a[ii][jj]);
            }
            contorw = 0;
            for (ii = 0; ii < 4; ii++)//verifica daca exista miscari valide
                for (jj = 0; jj < 3; jj++)
                    if ((a[jj][ii] && a[jj][ii] != a[jj + 1][ii]) || (a[jj][ii] == 0 && a[jj][ii] == a[jj + 1][ii]))
                        contorw++;
            break;
        case 's':
		case KEY_DOWN:
            for (ii = 0; ii < 4; ii++)
                for (jj = 0; jj < 4; jj++)
                    if (a[ii][jj] == 0)
                        ok++;

            if (ok != 0 && contors != 12) {//genereaza aleator 2 sau 4 in una din pozitiile in care matricea are valoarea 0 , daca s-a efectuat o miscare valida
                do {
                    ii = matrixCoordinates(val);
                    jj = matrixCoordinates(val);
                } while (a[ii][jj] != 0);
                a[ii][jj] = randomNumbers(num);
                y = coordy(ii);
                x = coordx(jj);
                mvprintw(y, x, "%d", a[ii][jj]);
            }
            contors = 0;
            for (ii = 0; ii < 4; ii++)//verifica daca exista miscari valide
                for (jj = 3; jj > 0; jj--)
                    if ((a[jj][ii] && a[jj][ii] != a[jj - 1][ii]) || (a[jj][ii] == 0 && a[jj][ii] == a[jj - 1][ii]))
                        contors++;
            break;

        case 'a':
		case KEY_LEFT:
            for (ii = 0; ii < 4; ii++)
                for (jj = 0; jj < 4; jj++)
                    if (a[ii][jj] == 0)
                        ok++;

            if (ok != 0 && contora != 12) {//genereaza aleator 2 sau 4 in una din pozitiile in care matricea are valoarea 0 , daca s-a efectuat o miscare valida
                do {
                    ii = matrixCoordinates(val);
                    jj = matrixCoordinates(val);
                } while (a[ii][jj] != 0);
                a[ii][jj] = randomNumbers(num);
                y = coordy(ii);
                x = coordx(jj);
                mvprintw(y, x, "%d", a[ii][jj]);
            }
            contora = 0;
            for (ii = 0; ii < 4; ii++)//verifica daca exista miscari valide
                for (jj = 0; jj < 3; jj++)
                    if ((a[ii][jj] && a[ii][jj] != a[ii][jj + 1]) || (a[ii][jj] == 0 && a[ii][jj] == a[ii][jj + 1]))
                        contora++;
            break;

        case 'd':
		case KEY_RIGHT:
            for (ii = 0; ii < 4; ii++)
                for (jj = 0; jj < 4; jj++)
                    if (a[ii][jj] == 0)
                        ok++;

            if (ok != 0 && contord != 12) {//genereaza aleator 2 sau 4 in una din pozitiile in care matricea are valoarea 0 , daca s-a efectuat o miscare valida
                do {
                    ii = matrixCoordinates(val);
                    jj = matrixCoordinates(val);
                } while (a[ii][jj] != 0);
                a[ii][jj] = randomNumbers(num);
                y = coordy(ii);
                x = coordx(jj);
                mvprintw(y, x, "%d", a[ii][jj]);
            }
            contord = 0;
            for (ii = 0; ii < 4; ii++)//verifica daca exista miscari valide
                for (jj = 3; jj > 0; jj--)
                    if ((a[ii][jj] && a[ii][jj] != a[ii][jj - 1]) || (a[ii][jj] == 0 && a[ii][jj] == a[ii][jj - 1]))
                        contord++;
            break;
        }
	
	for(ii = 0; ii < 4; ii++){ //lasa jucatorul sa stie ca a castigat
		for(jj = 0; jj < 4; jj++){
			if(a[ii][jj] == 2048){          //pentru a fi testata optiunea, se inlocuieste 2048 cu o valoare mai mica
				mvprintw(14, 13, "%s", "YOU WIN ! ! ! - PRESS ANY KEY to EXIT"); //daca o celula atinge valoare 2048, s-a terminat jocul(castigat)
					while (getch() == ERR); //daca se apasa o tasta(oricare) se intoarce la meniu
	                clear();
					return 1; // return 1 = jocul s-a terminat! -  iese din joc, se intoarce la  main menu
			}
		} 
	}
	}
    refresh();
	clear();
    return 0;
}

int menu(void) {//functia genereaza si controleaza meniul
	system("clear");
	int firstRun = 1;
	int i = 0;
	
	int timeSpentPlaying = 0;
	int *pTimeSpentPlaying = &timeSpentPlaying; //variabila este folosita penrtua retin timpul trecut de cand a inceput jocul pana in prezent(exprimat in secunde)
	
	int score = 0;
	int *pScore = &score; //this variable is used to store the score;//variabila este folosita pentru a retine scorul
	
	int **pTable; //aloca matricea dinamic pentru a retine datele din tabla de joc
	pTable = malloc(4 * sizeof *pTable);
	for(i = 0; i < 4; i++){
		pTable[i] = malloc(4 * sizeof *pTable[i]);
	}
	
    WINDOW * w;
    char string[6][10] = { // afiseaza meniul
		"2048 MENU",
        "New Game",
        "Resume",
        "Quit",
		"Fast Quit",
		"Help"
    };
    char option[9];
    int ch;

    initscr(); // initializeaza Ncurses
    clear();
    w = newwin(8, 14, 9, 33); // creeaza o fereastra
    box(w, 0, 0); // seteaza margini pentru fereastra
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_RED);

    // printeaza toate optiunile meniului
    for (i = 0; i < 6; i++) {
        if (i == 1)
            wattron(w, COLOR_PAIR(1)); // evidentiaza prima optiune
        else
            wattroff(w, COLOR_PAIR(1));
        sprintf(option, "%-8s", string[i]);
        mvwprintw(w, i + 1, 2, "%s", option);
    }
    wrefresh(w); // actualizeaza fereastra din terminal

    i = 1;
    noecho(); //folosita pentru a impiedica sa apara pe ecran caractere
    keypad(w, TRUE); //permite ca tastatura sa fie folosita in fereastra
    curs_set(0); 

    
    while ((ch = wgetch(w)) != 'q') {
        sprintf(option, "%-8s", string[i]);
        mvwprintw(w, i + 1, 2, "%s", option);
        switch (ch) {
		case 'w':
        case KEY_UP:
            i--;
            if (i < 1)
                i = 5;
			if((i == 2)&&(firstRun == 1)) //daca jocul nu a fost jucat niciodata de la rulare, nu poate fi accesat butonul Resume
				i = 1;
            break;
		case 's':
        case KEY_DOWN:
            i++;
            if (i > 5)
                i = 1;
			if((i == 2)&&(firstRun == 1)) //daca jocul nu a fost jucat niciodata de la rulare, nu poate fi accesat butonul Resume
				i = 3;
            break;
        }
        wattron(w, COLOR_PAIR(1));//evidentiaza urmatoarea optiune
        sprintf(option, "%-8s", string[i]);
        mvwprintw(w, i + 1, 2, "%s", option);
        wattroff(w, COLOR_PAIR(1));
        switch (ch) {
        case '\n':
            if (i == 1) {
				firstRun = 1;
                drawBoard(firstRun);
				if (playGame(firstRun, pTimeSpentPlaying, pScore, pTable) == 0){
					firstRun = 0; //jocul a fost jucat cel putin o data si nu este inca terminat
				}
				else{
					firstRun = 1; //jocul s-a terminat si jucatorul a castigat
				}
				
				//printeaza iar meniul si evidentiaza prima optiune
				refresh();
				box(w, 0, 0); 
				for (i = 0; i < 6; i++) {
					if (i == 1)
						wattron(w, COLOR_PAIR(1));
					else
						wattroff(w, COLOR_PAIR(1));
					sprintf(option, "%-8s", string[i]);
					mvwprintw(w, i + 1, 2, "%s", option);
				}
				i=1; //i=1 deoarece textul "2048 MENU" este pus pe prima pozitie
				
                break;
            }
			if (i == 2){
				drawBoard(firstRun);
				if (playGame(firstRun, pTimeSpentPlaying, pScore, pTable) == 0){
					firstRun = 0; 
				}
				else{
					firstRun = 1; 
				}
				
				
				refresh();
				box(w, 0, 0); 
				for (i = 0; i < 6; i++) {
					if (i == 1)
						wattron(w, COLOR_PAIR(1)); 
					else
						wattroff(w, COLOR_PAIR(1));
					sprintf(option, "%-8s", string[i]);
					mvwprintw(w, i + 1, 2, "%s", option);
				}
				i=1;
				
				break;
			}
            if (i == 3) { //iese in terminal daca se apasa tasta enter
				refresh();
				endwin();
				delwin(w); //sterge fereastra
				return 0;
                
            }
			if (i == 4){ //optiune care inchide si terminalul
				for (i = 0; i < 4; i++){ //elibereaza memoria folosita pentru matricea tablei de joc
					free(pTable[i]);
				}
				free(pTable);
				endwin();
				delwin(w); 
				kill(getppid(), SIGKILL); 
				break;
			}
			if (i == 5){ //optiunea care afiseaza informatii ajutatoare
				drawHelp();
				refresh();
				box(w, 0, 0); 
				for (i = 0; i < 6; i++) {
					if (i == 1)
						wattron(w, COLOR_PAIR(1)); 
					else
						wattroff(w, COLOR_PAIR(1));
					sprintf(option, "%-8s", string[i]);
					mvwprintw(w, i + 1, 2, "%s", option);
				}
				i=1;
				
				break;
			}
        }
    }
	for (i = 0; i < 4; i++) //elibereaza memoria folosita pentru matricea tablei de joc
      {
        free(pTable[i]);
      }
    free(pTable);
    refresh();  //iese in terminal daca se apasa 'q'
	endwin();
    delwin(w); //sterge fereastra
    return 0;
}
int main(void) {
    srand(time(NULL));
    menu();
	system("clear"); //mesaje transmise termnalului
	system("exit");
	system("reset"); //iese in terminal
    return 0;
}
