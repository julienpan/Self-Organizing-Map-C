#include "som.h"

int LIGNE = 6;
int COLONNE = 10;

// Compte le nombre de ligne
int nb_ligne(FILE *file) {
  char *line_buf = NULL;
  size_t line_buf_size = 0;
  int line_count = 0;
  ssize_t line_size;

  line_size = getline(&line_buf, &line_buf_size, file); 
  while(line_size >= 0) {
    line_count += 1;
    line_size = getline(&line_buf, &line_buf_size, file); 
  }
  rewind(file);
  return line_count;
}

// Fonction remplissage des iris
void remplir(FILE *file, iris_t *iris) {
  char *line_buf = NULL;
  size_t line_buf_size = 0;
  ssize_t line_size;
  double val_petale_iris;
  int i, ele;
  char *courant;
  char sep[2] = ",\n";

  courant = malloc(sizeof(char)*10);
  line_size = getline(&line_buf, &line_buf_size, file); 
  i = 0;
  while (line_size >= 0) { 
    // Premiere valeur
    courant = strtok(line_buf,sep); 
    val_petale_iris = atof(courant);
    iris[i].n_element = 4;
    iris[i].element = malloc(sizeof(double)*iris[i].n_element);
    iris[i].element[0] = val_petale_iris;
    // Un parcours des 4 valeurs directement n'a pas fonctionné
    for (ele = 1; ele < iris[i].n_element; ++ele) {
      // Les autres valeurs
      courant = strtok(NULL,sep); 
      val_petale_iris = atof(courant);
      iris[i].element[ele] = val_petale_iris;
    }
    courant = strtok(NULL,sep); 
    iris[i].nom = malloc(sizeof(char) * strlen(courant));
    iris[i].nom = strcpy(iris[i].nom,courant);
    i += 1;   
    line_size = getline(&line_buf, &line_buf_size, file);
  }
  free(line_buf);
  line_buf = NULL;   
}

// Equation de normalisation
void normalisation(iris_t *iris, int nb) {
	double tmp;

  for (int i = 0; i < nb; ++i) {
  	tmp = 0;
  	for (int j = 0; j < iris[i].n_element; ++j) {
  		tmp = tmp + pow(iris[i].element[j], 2);
  	}
    iris[i].norme = sqrt(tmp);
    for (int ele = 0; ele < iris[i].n_element; ++ele) {
      iris[i].element[ele] = iris[i].element[ele] / iris[i].norme;
    }
  }
}

void voisinage(neuron_t **reseau, coord_t gagnant, iris_t courant, double lr, int rayon) {

  int b1, b2, b3, b4;

  b1 = gagnant.x - rayon;
  b2 = gagnant.x + rayon;
  b3 = gagnant.y - rayon;
  b4 = gagnant.y + rayon;

  if(b1 < 0) {
    b1 = gagnant.x;
  }
  if(b2 >= COLONNE) {
    b2 = gagnant.x;
  }
  if(b3 < 0) {
    b3 = gagnant.y;
  }
  if(b4 >= LIGNE) {
    b4 = gagnant.y;
  }

  // On parcous avec les bornes trouvées du voisinage choisi
  for (int i = b1; i <= b2; ++i) {
    for (int j = b3; j <= b4; ++j) {
      for (int ele = 0; ele < courant.n_element; ++ele) {
        reseau[i][j].data[ele] += ((courant.element[ele] - reseau[i][j].data[ele]) * lr);
        
      }
    }
  }
}

// Best matching Unit par la distance euclidienne
coord_t bmu(iris_t courant, neuron_t ** reseau) {
  double res, tmp;
  int size, nombre, nr;
  coord_t gagnant;
  liste_t *tete, *distance;
  tete = malloc(sizeof(liste_t));
  distance = tete;
  distance->val = INT_MAX;

  for (int i = 0; i < COLONNE; ++i) {
    for (int j = 0; j < LIGNE; ++j) {
      tmp = 0;
    	for (int k = 0; k < courant.n_element; ++k) {
    		res = reseau[i][j].data[k] - courant.element[k];
    		tmp = tmp + pow(res,2);
    	}
      tmp = sqrt(tmp);

      if(tmp < distance->val) {
        free(distance);
        distance = malloc(sizeof(liste_t));
        size = 1;
        distance->val = tmp;
        distance->x = i;
        distance->y = j;
        distance->suivant = NULL;
        tete = distance;
      }
      
      if(tmp == distance->val) {
        if(distance != NULL) {
          size += 1;
          distance->suivant = malloc(sizeof(liste_t));
          distance = distance->suivant;
          distance->val = tmp;
          distance->x = i;
          distance->y = j;
          distance->suivant = NULL;
        } else {
          size = 1;
          distance->val = tmp;
          distance->x = i;
          distance->y = j;
          distance->suivant = NULL;
          tete = distance;
        }
      }
    }
  }

  //printf("%f \t : [%d, %d]\n", distance->val, distance->x, distance->y);

  nombre = rand() % size;
  distance = tete;
  nr = 0;
  while(nr < nombre) {
    distance = distance->suivant;
    //printf("Distance = %f\n", distance->val);
    nr++;
  }
  gagnant.val = distance->val;
  gagnant.x = distance->x;
  gagnant.y = distance->y;
  
  //printf("Gagnant = %f\t : [%d, %d]\n", gagnant.val, gagnant.x, gagnant.y);

  return gagnant;
}


void apprentissage(iris_t *iris, neuron_t **reseau,int nb, int iteration, int rayon) {

  double lr;
  coord_t gagnant;
  int x,y;
  int coefficient;
  coefficient = 0.8;

  int per, tmp;

  
  if (rayon > 1) { // Si rayon == 1, phase1 : rayon = 1
    rayon -= 1;
  }
  per = iteration / rayon; // 1000 / 9 (10) = 111
  tmp = per;

  // phase 1

  for (int i = 0; i < iteration; ++i) {
    if (rayon > 1 && i == per) {
      rayon -= 1;
      per = per + tmp;
    }
    lr =  (1 - (i/iteration)) * coefficient;
    for (int j = 0; j < nb; ++j) {
      gagnant = bmu(iris[j],reseau);
      voisinage(reseau,gagnant,iris[j],lr,rayon);
    }
  }

  // phase2 : rayon = 1
  
  for (int i = 0; i < iteration; ++i) {
    lr = (1 - (i/iteration)) * 0.08;
    for (int j = 0; j < nb; ++j) {
      gagnant = bmu(iris[j],reseau);
      voisinage(reseau,gagnant,iris[j],lr,rayon);
    }
  }

  for (int i = 0; i < nb; ++i) {
    gagnant = bmu(iris[i],reseau);
    x = gagnant.x;
    y = gagnant.y;
    if((reseau[x][y].val == 0)) {
      if(strcmp(iris[i].nom,"Iris-setosa") == 0) {
        reseau[x][y].val = 1;
      }
      if(strcmp(iris[i].nom,"Iris-versicolor") == 0) {
        reseau[x][y].val = 2;
      }
      if(strcmp(iris[i].nom,"Iris-virginica") == 0) {
        reseau[x][y].val = 3;
      }
    }
  }
  
}

void affichage(neuron_t ** reseau) {

  printf("\033[0;31m Iris-setosa : 1\n");
  printf("\033[0;32m Iris-versicolor : 2\n");
  printf("\033[0;34m Iris-virginica : 3\n\n");
  
  for (int i = 0; i < LIGNE; ++i) {
    for (int j = 0; j < COLONNE; ++j) {
      if(reseau[j][i].val == 1){
        printf("\033[0;31m%d ",reseau[j][i].val);
      }
      else if(reseau[j][i].val == 2){
        printf("\033[0;32m%d ",reseau[j][i].val);
      }
      else if(reseau[j][i].val == 3){
        printf("\033[0;34m%d ",reseau[j][i].val);
      }
      else{
        printf("\033[0;37m%d ",reseau[j][i].val);
      }
    }
    printf("\n");
  }
}
