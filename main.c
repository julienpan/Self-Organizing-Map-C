// make
// ./som
#include "som.h"

int main() {
  FILE * file;
  iris_t * iris;
  neuron_t ** reseau;
  int nb, n, iteration;
  double random;
  int LIGNE, COLONNE;
  double * tab;
  LIGNE = 6;
  COLONNE = 10;

  n = 4;

  srand(time(NULL));
  file = fopen("iris.data","r");
  if(file == NULL){
    fprintf(stderr,"Impossible d'ouvrir le fichier\n");
    exit(1);
  }
  nb = nb_ligne(file);
  iris = malloc(sizeof(iris_t) * nb);
  remplir(file,iris);
  normalisation(iris,nb);

  // Remplir le reseau de valeur proche de 0
  reseau = malloc(sizeof(neuron_t*)*COLONNE);
  for (int i = 0; i < COLONNE; ++i) {
    reseau[i] = malloc(sizeof(neuron_t)*LIGNE);
  }
  for (int i = 0; i < COLONNE; ++i) { 
    for (int j = 0; j < LIGNE; ++j) {
      reseau[i][j].data = malloc(sizeof(double) * n);
      for (int v = 0; v < n; ++v) {
        random = (rand()/(double)RAND_MAX)*0.99;
        reseau[i][j].data[v] = random;
      }
    }
  }
  
  apprentissage(iris,reseau,nb,1000,3);
  affichage(reseau);
  fclose(file);
  return 0;
}
