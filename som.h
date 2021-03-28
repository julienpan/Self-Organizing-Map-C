#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <limits.h>

typedef struct iris {
  double *element;
  int n_element;
  double norme;
  char *nom; 
}iris_t;

typedef struct neuron {
  double *data;
  int val;
}neuron_t;

typedef struct liste {
  int x;
  int y;
  double val;
  struct liste *suivant;
}liste_t;

typedef struct coord {
  int x;
  int y;
  double val;
}coord_t;

int nb_ligne(FILE *);
void remplir(FILE *, iris_t *);
void normalisation(iris_t*, int);
void voisinage(neuron_t **, coord_t, iris_t, double, int);
coord_t bmu(iris_t, neuron_t **);
void apprentissage(iris_t *, neuron_t **,int, int, int);
void affichage(neuron_t **);
