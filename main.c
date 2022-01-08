#include<stdio.h>
#include<stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>

#define ELIGIBLE 200  // le score minimal à battre pour pouvoir s'enregistrer à la liste des fameux


/*_______________________________________________ fonction game _________________________________*/
int game(void){


const int taille=LINES-10;   //  taille ~ nombre de lignes de bords affichés
int jeu[taille][2];    //  [i][0] = numero de la ligne,  [i][1]= numero de la colonne de bord gauche
int i; 
int echec=1;         // echec =1 <=> pas encore echoué,  echec=0 <=> echec
int col_cycliste;     // le nombre de colonne de cycliste ( la ligne c'est par defaut la derniere ligne
int score=0;          // score =  le nombre de lignes franchies

// initialisation des bords
  for(i=0;i<taille;i++){
    jeu[i][0]=i+5;                    // mise en place des numéros de lignes
    jeu[i][1]=(COLS-LARGEUR)/2.0;    // expression permettant de situé les bords en premier temps au centre de la fenetre 
  }



col_cycliste=jeu[taille-1][1]+ LARGEUR*1.0/2;   // cycliste placé au centre des bords

  while (echec != 0){    // tant qu'il n'a encore heurté un des bords
    clear();             // effacer le contenu de la fenetre avant de dessiner le jeu
    
    //mettre en place les bords
    for(i=0; i<taille; i++){
      mvprintw(jeu[i][0],jeu[i][1], "*");
      mvprintw(jeu[i][0],jeu[i][1]+LARGEUR, "*");  
    }
    
    // placer le cycliste ( ^ ou X selon son etat)
    if(col_cycliste > jeu[taille-1][1] && col_cycliste < jeu[taille-1][1]+LARGEUR){ 
      mvprintw(jeu[taille-1][0],col_cycliste, "^");
      score++;  // calcul du score = nombre de lignes franchies
    }
    else { 
      mvprintw(jeu[taille-1][0],col_cycliste, "X");
      echec =0;
    }
    
    //update des bords
    for(i=taille-1; i>=1; i--){
      jeu[i][1]=jeu[i-1][1];
    }
    
    // determination pseudo-aleatoire de la position de la nouvelle ligne par rapport à celle qui la précéde
    if (clock() %3 ==1 ){jeu[0][1] += 1;}
    if (clock() %3 ==2 ){jeu[0][1] -= 1;}
    
    nodelay(stdscr, TRUE);
    
    //update de position de cycliste selon le choix selectionné par le joueur
    switch ( getch()){
    
      case KEY_LEFT : col_cycliste--;
      break;
      
      case KEY_RIGHT : col_cycliste++;
      break;
      
      default :
      break;
    
    } 
    
    refresh();       // rafraichir la page
    usleep(DELAY);   // Interrompre le programme durant DELAY microsecondes
         
  }
  clear();      // jeu achevé => effacer le contenu de la fenetre/le jeu 
  return score;
}

//ajouter le nom du joueur (et son score) ayant dépasser un certain sueil
int add_username_score( int score)
{

char username[50];

    clear(); // Clearing the screen
    printw("\n\tThe Score you just hit is AMAZING ! : %d \n \nWhat's your Name ?\n", score); // print in curses window
    getch();
    scanf("%s", username);      // la lecture du nom du joueur
    FILE* f = fopen("HoF.txt", "a+");
    
    if (f == NULL)    
      return 0;
    else {
      fprintf(f, "%s : %d\n", username, score);   // l'écriture de nom et du score dans un fichier
    }
          
    fclose(f);
    return 1;
}




int Restart_Exit(int record, int score){
  
  clear();  //effacer le contenu de la fenetre avant d'afficher le menu
  
        /**************** partie creation de cadre du Menu ********************/

  // Creation d'une nouvelle fenêtre 
  WINDOW *win = newwin(12, 50, (LINES-12)/2, (COLS-50)/2);

  box(win, 0, 0);   // Making box border with default border styles
  refresh();        // rafaraichie la fenetre principale
  wrefresh(win);    // rafaraichie la fenetre win

  // If enabled (bf is TRUE), the user can press a function key
  // (such as an arrow key) and wgetch
  keypad(win, TRUE); 
  
        /*************** corps du Menu ********************/
  int choice;
  int final_choice;
  int hightlight = 0;
  int exit=0;
  char* choices[3] = {"START A GAME","HALL OF FAME", "EXIT"};
    
  mvwprintw(win, 1, 11, "!!!___CANYON DE LA MORT___!!!");
  mvwprintw(win, 3, 1, " Record : %d", record);
  mvwprintw(win, 4, 1, " Score  : %d", score);
  
  while(exit==0){  // tant que le choix n'est pas encore effectué
  
    mvwprintw(win, 6, 10, choices[0]);
    mvwprintw(win, 7, 10, choices[1]);
    mvwprintw(win, 8, 10, choices[2]);
  
    wattron(win, A_REVERSE);   // Effet de selection d'un choix [en blanc]
    mvwprintw(win, hightlight+6, 10, choices[hightlight]);  // Move the cursor in the window to the position (hightlight+6, 10) and print choices[hightlight]
    wattroff(win, A_REVERSE);   // enleve l'effet
      
    
  /**************************** traitement de choix (rejouer/quitter/acceder au hall) **************************/
   choice = wgetch(win);   //Recuperer une touche du clavier du joueur dans choice
    switch(choice)
    {
      case KEY_UP: 
        hightlight--;
        // Making the menu cyclic
        if (hightlight == -1)
          hightlight = 2;
      break;
  
      case KEY_DOWN:
        hightlight++;
        // Making the menu cyclic
        if (hightlight == 3)
          hightlight = 0;
      break;
  
      // If the user hits ENTER button
      case 10:
        if (hightlight == 0) { final_choice=1; }
        if (hightlight == 1) { final_choice=2; }
        if (hightlight == 2) { final_choice=0; }
        
        exit=1;    // choix effectué => exit = 1 => quitter la boucle while 
      break;
  
      default:
      break;
    }
  }
  
  delwin(win);   // supprimer/fermer la fenetre win
  return final_choice;
}

// afficher le fichier representant le hall of fame et sortir duquel après clique touche "entrer"
void Hall_of_fame(){

  char c;
  int touche;
  
      clear();  // Clearing the screen
       
      // Showing the file's content 
      FILE *file = fopen("HoF.txt", "r");;      
      
      // lire le contenu du fichier element par element et l'afficher à la fenetre
      while ((c=fgetc(file)) != EOF){
          refresh();
          printw("%c", c);                    
      }
      fclose(file);          
      
      // attendre la touche "Entrer" pour quitter le "Hall"
      do{
        touche = getch();
      }while (touche != 10);    
      

}

//____________________________________________ main __________________________________

int main(){

 initscr();      // initialisation l'écran 
 noecho();       // noecho controls whether characters typed by the user are echoed by getch as they are typed
 curs_set(FALSE);   // making the cursor invisible
 keypad(stdscr, TRUE);   // Enables the keypad of the user's terminal

int score=0, record=0;   // initialiser le score et le record à zero
int decision;
  
  // le premier menu : avant tout commencer
  decision= Restart_Exit (record, score);     // afficher le menu et obtenir le choix du joueur
  while ( decision == 2){        // tant qu'il choisit/rechoisit d'acceder au hall (qui est encore vide)
      
      Hall_of_fame();   // afficher la liste des joueurs ayant battus le seuil        
      decision= Restart_Exit (record, score);      // choix de jouer/ quitter/ accéder au hall (qui est encore vide)

   }  
  
  while ( decision == 1){  // tant qu'il choisit de jouer le jeu
   
   score=game();     // lance le jeu et retourne le score
   
   
   // le max des differents scores obtenus = record
   if ( record < score){
     record= score ;
   }
   // si score dépasse un seuil fixé auparavant, le joueur sera inscrit au hall 
   if (score >= ELIGIBLE){
     add_username_score( score);
   }
   
   // affichage du menu puis nouvelle decision
   decision= Restart_Exit (record, score);
   
   //tant qu'il choisit d'acceder à la liste
   while ( decision == 2){
      
      Hall_of_fame();   // afficher la liste des joueurs ayant battus le seuil       
      decision= Restart_Exit (record, score);      // reaffichage du menu et obtention d'une nouvelle décision
   }      
   
 }

usleep(DELAY);  // l'attente après le DELAY (en micro-seconde)
endwin();   // ferme la fenetre

return 0;
}




































