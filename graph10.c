#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// limite le nombre d'arcs saisie en une ligne pour 'saisirEdges' et 'parseInput'
// mais les ars sont enregistré dans une chaine chainée, ie, pas de limitation en nombre!
#define limit 50

// variable globale
int notOriented = 0; // par defaut 0:graphe orienté, 1 non orienté
char symArc[3] = "->";// affichage et saisie des arcs: -> si orienté , sinon -
// les sommets: on peut modifier le type de key en d'autre type! il faudrai changer quelques fonctions!
typedef struct node {
  unsigned int key;
} node;

// crée un somet (pointeur sur un sommet) dont la valeur du key = val
void initNode(node** nd, unsigned int val) {
  *nd = malloc(sizeof(node));
  (*nd)->key = val;
}
// liste chainée des arcs (src->dst)
typedef struct edge {
  node* src;
  node* dst;
  struct edge* next;
} edge;
// liste chainée contenant 2 listes un pour les voisins d'un sommet nd : neigh 
// et l'autre: next = la liste suivant dans la liste globale des (listes) de sommets du graphe 
typedef struct list {
  node* nd;
  struct list* next;  // le suivant de nd dans l'ensemble des sommets
  struct list* neigh; // les voisins de nd
} list;


typedef struct graph {
  struct list* lnd;   // liste de tous les sommets  (nodes)
  struct edge* edges; // les voisins des sommets
} graph;

void initGraph(graph** G) {
  *G = malloc(sizeof(graph));
  (*G)->lnd = NULL;
  (*G)->edges = NULL;
}
// compare les valeurs (key) de 2 sommets
// 0 si egale, negatif si strictement inférieur et strictement positif si strict supérieur
// pour faciliter le changement si le key est un *char , même retour que strcmp
int cmpNode(node* n1, node* n2) {
  return (n1->key - n2->key);
}
// supprime la liste *L
void clearList(list** L) {
  list* tmp = *L;
  if (!L) return;
  if (tmp->next) clearList(&tmp->next);
  if (tmp->neigh) clearList(&tmp->neigh);

  //free(tmp->nd);
  free(tmp);
  *L = NULL;
}

// ajout d'un sommet dans la liste next
// verifier s'il est déjà dans la liste les sommets sont uniques
void addNodeNextList(list** L, node* nd) {
  list* tmp = *L;
  list* elem = malloc(sizeof(list));
  elem->nd = nd;
  elem->next = NULL;
  elem->neigh = NULL;
  if (tmp) {
    while (cmpNode(tmp->nd, nd) && tmp->next) {
      tmp = tmp->next;
    }
    if (tmp && cmpNode(tmp->nd, nd)) tmp->next = elem;
    else free(elem);
  }
  else *L = elem;
}
// cherche le node src, puis ajouter le node nei dans son neigh si nei n'est pas dans neigh (vérification)
void addNodeNeighList(list** L, node* src, node* nei) {
  list* tmp = *L;
  list* elem = malloc(sizeof(list));
  elem->nd = nei;
  elem->next = NULL;
  elem->neigh = NULL;
  if (tmp) {
    while (tmp && cmpNode(tmp->nd, src) && tmp->next) {
      tmp = tmp->next;
    }
    if (tmp && cmpNode(tmp->nd, src) == 0) {// node trouvé
      list* tmpnei = tmp->neigh;
      if (tmpnei) {
        while (cmpNode(tmpnei->nd, nei) && tmpnei->neigh) {
          tmpnei = tmpnei->neigh;
        }
        if (tmpnei && cmpNode(tmpnei->nd, nei) != 0) {// pas encore dans la liste des neigh
          tmpnei->neigh = elem;
        }
      }
      else {
        tmp->neigh = elem;
      }
    }
  }
}


// // structure pile LIFO // //
typedef struct stack {
  list* top;
  struct stack* next;
}stack;
// supprime la pile
void clearStack(stack** P) {
  stack* tmp = *P;
  if (!P) return;
  if (tmp->next) clearStack(&tmp->next);

  //free(tmp->top);
  free(tmp);
  *P = NULL;
}

void initStack(stack** P) {
  *P = malloc(sizeof(stack));
  (*P)->top = NULL;
  (*P)->next = NULL;
}
void empiler(stack** P, list* lnd) {
  stack* elem = malloc(sizeof(stack));
  elem->top = lnd;
  elem->next = NULL;
  if (*P) {
    elem->next = *P;
  }
  *P = elem;
}
void depiler(stack** P, list* lnd) {
  if (*P) {
    lnd = (*P)->top;
    stack* tmp = *P;
    *P = tmp->next;
    free(tmp);
  }
  else lnd = NULL;
}
// // // structure file FIFO (queue) // // //
typedef struct queue {
  struct queue* head; // la tête
  struct queue* tail; // la queue
  list* top;
} queue;
// suprime la file
void clearQueue(queue** P) {
  queue* tmp = *P;
  if (!P) return;
  if (tmp->head) clearQueue(&tmp->head);
  if (tmp->tail) clearQueue(&tmp->tail);

  //free(tmp->top);
  free(tmp);
  *P = NULL;
}

void initQueue(queue** P) {
  *P = malloc(sizeof(queue));
  (*P)->head = NULL;
  (*P)->tail = NULL;
  (*P)->top = NULL;
}
void enfiler(queue** P, list* lnd) {
  queue* elem = malloc(sizeof(queue));
  elem->head = *P;
  elem->top = lnd;
  elem->tail = NULL;
  if (*P) {
    queue* tmp = *P;
    while (tmp->tail) tmp = tmp->tail;
    tmp->tail = elem;
  }
  else *P = elem;
}

void defiler(queue** P, list* lnd) {
  if (*P) {
    lnd = (*P)->top;
    queue* tmp = *P;
    *P = tmp->tail;
    free(tmp->head);
    free(tmp);
    if (*P) (*P)->head = NULL;
  }
  else lnd = NULL;
}
// teste si (src--dst) est dans les arcs dg
// 0 si (src--dst) n'est pas dans les arcs dg
// 1 si (src--dst) est dans les arcs dg
int isInEdge(edge* dg, node* src, node* dst) {
  edge* tmp = dg;
  while (tmp) {
    if (cmpNode(tmp->src, src) == 0 && cmpNode(tmp->dst, dst) == 0) return 1;
    tmp = tmp->next;
  }
  return 0;
}

// ajouter un arc (src--dst) dans la liste dg
// vérifie avant d'ajouter si les entrées sont déjà dans dg
void addEdge(edge** dg, node* src, node* dst) {
  if (!isInEdge(*dg, src, dst)) {
    edge* tmp = *dg;
    edge* elem = malloc(sizeof(edge));
    elem->src = src;
    elem->dst = dst;
    elem->next = NULL;
    if (tmp) {
      while (tmp->next) tmp = tmp->next;
      tmp->next = elem;
    }
    else *dg = elem;
  }
}
// suprime la liste des arcs
void clearEdge(edge** dg) {
  edge* tmp = *dg;
  if (!dg) return;
  if (tmp->next) clearEdge(&tmp->next);

  free(tmp->src);
  free(tmp->dst);
  free(tmp);
  *dg = NULL;
}
//affiche les arcs d'un graphe
void printEdge(edge* dg) {
  edge* tmp = dg;
  while (tmp) {
    printf(" %u %s %u |", tmp->src->key, symArc, tmp->dst->key);
    tmp = tmp->next;
  }
  printf("\n");
}
// affiche les sommets d'un graphe
void printListNodesNext(list* L) {
  list* tmp = L;
  while (tmp) {
    printf(" %u ", tmp->nd->key);
    tmp = tmp->next;
  }
  printf("\n");
}
// affiche le soisin du sommet src
void printListNodeNeigh(list* L, node* src) {
  list* tmp = L;
  while (tmp && cmpNode(tmp->nd, src)) tmp = tmp->next;
  if (tmp && cmpNode(tmp->nd, src) == 0) {
    list* tmpnei = tmp->neigh;
    while (tmpnei) {
      printf(" %u ", tmpnei->nd->key);
      tmpnei = tmpnei->neigh;
    }
  }
  printf("\n");
}
// affiche les voisins d'un sommet en entier, 
// créer un node avec la valeur entier et appel la fonction précédent 'printListNodeNeighKey'
void printListNodeNeighKey(list* L, unsigned int key) {
  node* src = NULL;
  initNode(&src, key);
  printListNodeNeigh(L, src);
}

// pour traiter les saisis des arcs pars l'utilisateur
// avec des séparateurs: ' ' ou '|' ou ','
// on accepte les chaines liées par un caractère différents des séparateurs comme '-', '>', '.', 'a'
// par exemple (N1-N2>N3*N4|N5-N6,N8@N9)==(N1-N2,N2-N3,N3-N4,N5-N6,N8-N9)
void parseInput(char input[8 * limit], unsigned int x[limit][2], unsigned int* cur) {
  // x tableau des arcs, pour l'arc i: x[i][0] source et x[i][1] destination, cur pointeur sur la taille de x remplie
  printf("input: %s\n", input);
  *cur = 0;
  unsigned char valInBuffer = 0; // si 0 pas de nouveau valeur dans v, si 1 nouveau valeur existant 

  unsigned char precAdd = 0;
  unsigned char pPrecAdd = 0;
  // precAdd et pPrecAdd sont 2 variables pour enregistrer les types des précédents entrer input[i] 
  // 0 si separateur (' ' ou '|' ou ','
  // 2 si chiffree 
  // 1 sinon interprété comme lien  

  unsigned int size = strlen(input);
  unsigned int v = 0; // pour créer les entiers écrits dans la chaine de caractères
  for (unsigned int i = 0; i < size; i++) {
    // printf("debug [%u]=%c: [%d > %d] v:%u cur:%u\n",i,input[i],pPrecAdd,precAdd,v,*cur);
    if (*cur < limit) { // pour ne pas deborder dans le tableau x 
      if (input[i] == ' ' || input[i] == ',' || input[i] == '|') {//separation
        if (precAdd != 0 && valInBuffer) {
          if (pPrecAdd == 1 && precAdd == 2) {// chiffre,v: sommet src
            x[(*cur)++][1] = v;
            pPrecAdd = precAdd;
            v = 0;
            valInBuffer = 0;
            precAdd = 0;
          }
        }
      }
      else if (input[i] >= '0' && input[i] <= '9') {
        v *= 10;
        v += (input[i] - '0');
        valInBuffer = 1;
        if (precAdd != 2) pPrecAdd = precAdd;
        precAdd = 2;

      }
      else { //symbole (!= des séparateur == espace virgule barre)
        if (precAdd != 1) {
          if (valInBuffer) {
            if (pPrecAdd == 0 && precAdd == 2) {// chaine -v-v
              x[(*cur)][0] = v;
              pPrecAdd = precAdd;
            }
            else if ((pPrecAdd == 2 && precAdd == 0) || (pPrecAdd == 1 && precAdd == 2)) {//s'il y a un séparateur: chaine v- v ou v--v
              x[(*cur)++][1] = v;
              x[(*cur)][0] = v;
              pPrecAdd = precAdd;
            }
            else {//  
              printf("erreur saisi symbole, espace_%c verifier svp\n", input[i]);
            }
          }
          v = 0;
          valInBuffer = 0;
          precAdd = 1;
        }
      }
    }
  }
}
// pour vider le buffer de la saisi de l'écran (input) stdin
void flushInputScreen()
{
  int c;
  while ((c = getchar()) != '\n' && c != EOF); ;
}
// écrire les edges et extraire en même temps les nodes
// extraction des entrées avec 'parseInput'
// vérifie aussi si les sommets de l'arc sont parmis les sommets, sinon on l'ajoute
// ajoute le sommet dst parmis les voisins de src si dst n'est pas déjà dans la liste
// si graphe non orienté: dst-src est aussi un arc
void saisirEdges(graph** G) {

  char input[8 * limit];
  unsigned int in[limit][2];
  unsigned int size = 0;
  printf("entrée liste d'arcs de la forme N1-N2-N3 N4>N5>N6,N7-N8-N9|N10-N11\n");
  printf("les séparateurs possibles sont:' ' ou ',' ou '|' comme exemples au dessus\n");
  printf("Ni sont des sommets liés par les caractères comme '-','>','.','#'... != séparateurs\n");
  printf("par exemple (N1-N2>N3*N4|N5-N6,N8@N9 N7@N10)==(N1-N2,N2-N3,N3-N4,N5-N6,N8-N9,N7-N10)\n");
  flushInputScreen();
  fgets(input, 8 * limit, stdin);
  parseInput(input, in, &size);

  //printf("debug  size = %d\n",size); 

  for (unsigned int i = 0;i < size;i++) {
    //printf(" debug %u %s %u \n",in[i][0],symArc,in[i][1]);

    node* src = NULL, * dst = NULL;
    initNode(&src, in[i][0]);
    initNode(&dst, in[i][1]);
    addEdge(&(*G)->edges, src, dst);
    addNodeNextList(&(*G)->lnd, src);
    addNodeNextList(&(*G)->lnd, dst);
    addNodeNeighList(&(*G)->lnd, src, dst);
    if (notOriented) addNodeNeighList(&(*G)->lnd, dst, src); // si non orienté, src est aussi voisin de dst

  }
}
// ajouter un sommet dans le graphe
// on vérifie la saisie (entier positif ou pas, sinon recommencer à saisir)
void saisirNode(graph** G) {
  unsigned int s;
  int scn = 0, cnt = 0;
  do {
    if (cnt == 0) {
      printf("entrée un sommet qui est un entier positif\n");
      cnt = 1;
    }
    else printf("erreur saisie, entier positif\n");
    scn = scanf("%u", &s);
  } while (scn != 1);
  node* src = NULL;
  initNode(&src, s);
  addNodeNextList(&(*G)->lnd, src);
}

// liste chainée des états des sommets
// on enregistre le sommet et son état
// state = état: 0 blanc, 1 gris, 2 noir
typedef struct state {
  node* nd;
  int state;
  struct state* next;
}state;

void clearState(state** st) {
  state* tmp = *st;
  if (!st) return;
  if (tmp->next) clearState(&tmp->next);

  //free(tmp->nd);
  free(tmp);
  *st = NULL;
}

// initialise une liste d'état 'st' des sommets d'un graphe à '0' (blanc)
void initStateGraph(state** st, graph* G) {
  if (*st) clearState(st);
  list* tmplist = G->lnd;
  if (tmplist) {
    *st = malloc(sizeof(state));
    state* tmp = *st;
    while (tmplist) {
      tmp->nd = tmplist->nd;
      tmp->state = 0;
      if (tmplist->next) {
        tmp->next = malloc(sizeof(state));
        tmp = tmp->next;
      }
      else {
        tmp->next = NULL;
      }
      tmplist = tmplist->next;
    }
  }
  else *st = NULL;
}
// change l'état d'un sommet 'src' en état 's' dans la liste des états 'st'
void changeStateToVisited(state** st, node* src, int s) {
  state* tmp = *st;
  while (tmp && cmpNode(tmp->nd, src)) tmp = tmp->next;
  if (tmp && cmpNode(tmp->nd, src) == 0) {
    tmp->state = s;
  }
}
// retourne l'état d'un sommet 'src' enregistré dans la liste 'st'
int stateNodeGraph(state* st, node* src) {
  state* tmp = st;
  while (tmp && cmpNode(tmp->nd, src)) tmp = tmp->next;
  if (tmp && cmpNode(tmp->nd, src) == 0) return tmp->state;
  return 3;
}

// parcours en profondeur en récursif
void parcoursDFSRec(graph* G, node* src, state** st) {
  if (stateNodeGraph(*st, src) == 0) {

    changeStateToVisited(st, src, 1);
    printf(" %u ", src->key);
    list* tmp = G->lnd;
    while (tmp && cmpNode(tmp->nd, src))tmp = tmp->next;
    if (tmp) {
      list* tmpnei = tmp->neigh;
      while (tmpnei) {
        if (stateNodeGraph(*st, tmpnei->nd) == 0) {
          parcoursDFSRec(G, tmpnei->nd, st);
        }
        tmpnei = tmpnei->neigh;
      }
    }
  }
}
// liste chainée pour enregistrement des sommets 
// (comme un tableau de sommet)
typedef struct chemin {
  unsigned int key;
  struct chemin* next;
} chemin;
// ajout à la fin de la liste
void addQueuChemin(chemin** L, unsigned int key) {
  chemin* tmp = *L;
  chemin* elem = malloc(sizeof(chemin));
  elem->key = key;
  elem->next = NULL;
  if (tmp) {
    while (tmp->next) {
      tmp = tmp->next;
    }
    tmp->next = elem;
  }
  else *L = elem;
}
// ajout au début de la liste
void addHeadChemin(chemin** L, unsigned int key) {
  chemin* tmp = *L;
  chemin* elem = malloc(sizeof(chemin));
  elem->key = key;
  elem->next = tmp;
  *L = elem;
}
// affichage de la liste
void printChemin(chemin* L) {
  chemin* tmp = L;
  while (tmp) {
    printf(" %u ", tmp->key);
    tmp = tmp->next;
  }
  printf("\n");
}
// copie l'ordre des sommets vers une nouvelle liste
void copyChemin(chemin** dst, chemin* src) {
  chemin* tmp = src;
  while (tmp) {
    addQueuChemin(dst, tmp->key);
    tmp = tmp->next;
  }
}
// supprime la liste
void clearChemin(chemin** L) {
  chemin* tmp = *L;
  if (!L) return;
  if (tmp->next) clearChemin(&tmp->next);
  free(tmp);
  *L = NULL;
}
// teste si un sommet est dans la liste 
// 1 si oui (il est dans la liste) et 0 sinon
int isInChemin(chemin* pth, unsigned int key) {
  chemin* tmp = pth;
  while (tmp) {
    if (tmp->key == key) return 1;
    tmp = tmp->next;
  }
  return 0;
}


// parcours en profondeur du graph en itératif
// on enregistre les nodes en noir (state==2) pour avoir la liste en profondeur  
// note: si on affiche à chaque empilement, l'affichage est identique au parcours en largeur 
// solution: enregistrement à la tête d'une liste 'chemin' les nodes noir (state==2) pendant le dépilement
// on utilise ici une pile (LIFO)
void parcoursDFSIteratif(graph* G, list* lsrc, state** st) {
  if (stateNodeGraph(*st, lsrc->nd) == 0) {
    stack* P = NULL;
    list* tmpL;
    list* tmpI;
    chemin* C = NULL;
    unsigned int cnt = 0;
    changeStateToVisited(st, lsrc->nd, 1);
    //printf(" %u ",lsrc->nd->key); 
    empiler(&P, lsrc);
    while (P) {
      tmpL = P->top;
      if (tmpL) {
        list* tmp = G->lnd;
        while (tmp && cmpNode(tmp->nd, tmpL->nd)) tmp = tmp->next;
        if (tmp) {
          tmpI = tmp->neigh;
          cnt = 0;
          while (tmpI) {
            if (stateNodeGraph(*st, tmpI->nd) == 0) {
              changeStateToVisited(st, tmpI->nd, 1);
              //printf(" %u ",tmpI->nd->key);
              empiler(&P, tmpI);
              cnt++;
            }
            tmpI = tmpI->neigh;
          }
          if (cnt == 0) {
            changeStateToVisited(st, tmpL->nd, 2);
            depiler(&P, tmpL);
            addHeadChemin(&C, tmpL->nd->key);
          }
        }
      }
    }
    printChemin(C);
  }
}
// avec ce parcours en itératif,
// on affiche les éléments à son enfilement pour voir les nodes en largeur
// on utilise la file (queue) FIFO
void parcoursBFSIteratif(graph* G, list* lsrc, state** st) {
  if (stateNodeGraph(*st, lsrc->nd) == 0) {
    queue* F = NULL;
    list* tmpL;
    list* tmpI;
    printf(" %u ", lsrc->nd->key);
    changeStateToVisited(st, lsrc->nd, 1);
    enfiler(&F, lsrc);
    while (F) {
      tmpL = F->top;
      if (tmpL) {
        tmpI = tmpL->neigh;
        while (tmpI) {
          if (stateNodeGraph(*st, tmpI->nd) == 0) {
            changeStateToVisited(st, tmpI->nd, 1);
            printf(" %u ", tmpI->nd->key);
            enfiler(&F, tmpI);
          }
          tmpI = tmpI->neigh;
        }
      }
      defiler(&F, tmpL);
      changeStateToVisited(st, tmpL->nd, 2);
    }
  }
}
// affiche la matrice correspondant à un graphe
// en affichant le poid d'un lien entre 2 sommets du graphe
// ici le lien est 1 s'il existe (l'arc exite) et 0 sinon
// on liste d'abord les sommets dans l'ordre d'entrée (saisie) dans le graphe
// puis sur chaque ligne correspond à un sommet et son lien avec les autres sommets
void printMatriceGraph(graph* G, int nbchar) {
  list* cur = G->lnd, * curg, * tmpx = G->lnd;
  //affichage entête de la matrice
  for (int i = 0; i < nbchar + 2;i++) printf("%c", ' ');
  printf("|");
  while (cur) {
    printf(" %*u ", nbchar, cur->nd->key);
    cur = cur->next;
  }
  printf("\n");
  cur = G->lnd;
  for (int i = 0; i < nbchar + 2;i++) printf("%c", '-');
  while (cur) {
    for (int i = 0; i < nbchar + 2;i++) printf("%c", '-');
    cur = cur->next;
  }
  printf("\n");
  //corps de la matrice
  while (tmpx) {
    cur = G->lnd;
    printf(" %*u |", nbchar, tmpx->nd->key);
    while (cur) {
      curg = tmpx->neigh;
      while (curg) {
        if (cmpNode(curg->nd, cur->nd) == 0) break;
        curg = curg->neigh;
      }
      if (curg && cmpNode(curg->nd, cur->nd) == 0) {
        printf(" %*d ", nbchar, 1);
      }
      else printf(" %*d ", nbchar, 0);
      cur = cur->next;
    }
    printf("\n");
    tmpx = tmpx->next;
  }
}
// affiche les chemins éxistant entre les sommets src et dst (src vers dst si orienté)
// méthode récursif
// on affiche seulement si la src == dst
void path(graph* G, unsigned int src, unsigned int dst, chemin* pth) {
  chemin* C = NULL;
  list* tmpx = G->lnd;
  copyChemin(&C, pth);
  addQueuChemin(&C, src);
  if (src == dst) {
    printChemin(C);
    clearChemin(&C);
  }
  else {
    while (tmpx && tmpx->nd->key != src) tmpx = tmpx->next;
    if (tmpx && tmpx->nd->key == src) {
      list* tmp = tmpx->neigh;
      while (tmp) {
        if (!isInChemin(C, tmp->nd->key)) {
          path(G, tmp->nd->key, dst, C);
        }
        tmp = tmp->neigh;
      }
    }
  }
}
// le menu déroulant pour choisir les actions à éxecuter
void printMenu() {

  printf("\n");for (int i = 0;i < 40;i++)printf("=");printf("\n");
  printf("1. saisir arcs\n2. afficher les arcs\n3. saisir sommet\n4. afficher les sommets\n5. afficher les voisins d'un sommet\n");
  printf("6. parcourir en profondeur (recurssive)\n7. parcourir en profondeur (iterative)\n8. afficher la matrice du graphe\n");
  printf("9. lister les chemins entre 2 sommets\n0. parcourir en largeur (iteratif)\nq. Quitter (ou s. Sortir)\n");
  for (int i = 0;i < 40;i++)printf("=");printf("\n");
}
int main() {
  printf("voulez vous travailler sur les graphes non orientés? o/n\n");
  char cont[256];
  scanf("%s", cont);
  if (cont[0] == 'o' || cont[0] == 'O' || cont[0] == 'y' || cont[0] == 'Y') {
    notOriented = 1;
    strcpy(symArc, "-");
  }

  unsigned int sommet;
  graph* G;
  list* tmp;
  state* st = NULL;
  initGraph(&G);
  do {
    printMenu();
    scanf("%s", cont);
    switch (cont[0]) {
    case '1':
    saisirEdges(&G);
    break;
    case '2':
    printf("liste des arcs du graphe\n");
    printEdge(G->edges);
    break;
    case '3':
    saisirNode(&G);
    break;
    case '4':
    printf("liste des sommets du graphe\n");
    printListNodesNext(G->lnd);
    break;
    case '5':
    printf("entrer un sommet:\n"); scanf("%u", &sommet);
    printf("les voisins de %u sont:\n", sommet);
    printListNodeNeighKey(G->lnd, sommet);
    break;
    case '6':
    printf("parcours en profondeur du graphe en recursif\n");
    initStateGraph(&st, G);
    tmp = G->lnd;
    while (tmp) {
      parcoursDFSRec(G, tmp->nd, &st);
      tmp = tmp->next;
    }
    printf("\n");
    break;
    case '7':
    printf("parcours en profondeur du graphe en itératif\n");
    initStateGraph(&st, G);
    tmp = G->lnd;
    while (tmp) {
      parcoursDFSIteratif(G, tmp, &st);
      tmp = tmp->next;
    }
    printf("\n");
    break;
    case '8':
    int nbchar = 2;
    printf("entrer le nombre de decimal du plus grand sommet\n");
    scanf("%d", &nbchar);
    printf("matrice du graph\n");
    printMatriceGraph(G, nbchar);
    break;
    case '9':
    unsigned int s, d, cntds = 0;
    do {
      printf("entrer 2 sommets N%sM, N,M 2 sommets entier positif\n", symArc);
      if (notOriented) cntds = scanf("%u-%u", &s, &d);
      else cntds = scanf("%u->%u", &s, &d);
    } while (cntds != 2);
    chemin* pth = NULL;
    printf("les chemins entre %u et %u:\n", s, d);
    path(G, s, d, pth);
    break;
    case '0':
    printf("parcours en largeur du graphe en itératif\n");
    initStateGraph(&st, G);
    tmp = G->lnd;
    while (tmp) {
      parcoursBFSIteratif(G, tmp, &st);
      tmp = tmp->next;
    }
    printf("\n");
    break;
    }
  } while (cont[0] != 's' && cont[0] != 'S' && cont[0] != 'q' && cont[0] != 'Q');
  return 0;
}