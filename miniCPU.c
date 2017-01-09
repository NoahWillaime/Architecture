/*
 * -------------------------- TP du module Archi1 -------------------------
 *
 * ATTENTION : un outil de d�tection de plagiat logiciel sera utilis� lors de la correction,
 * vous avez donc tout int�r�t � effectuer un travail PERSONNEL
 *
 * Un mot/registre de 10 � 30 bits est repr�sent� par un tableau d'entiers �gaux � 0 ou 1.
 * Une ALSU est repr�sent�e par une structure ALSU, avec registre accumulateur
 * et registre d'�tat.
 * Un CPU (tr�s tr�s simplifi�) est repr�sent� par une ALSU et quelques registres
 * n�cessaires pour stocker les r�sultats interm�diaires.
 *
 * Certaines fonctions vous sont fournies, d'autres sont � implanter ou � compl�ter,
 * de pr�f�rence dans l'ordre o� eles sont indiqu�es.
 * Il vous est fortement conseill� de lire attentivement l'ensemble des commentaires.
 *
 * Parmi les op�rations arithm�tiques et logiques, seules 4 op�rations de base sont
 * directement fournies par l'ALSU, les autres doivent �tre d�crites comme des algorithmes
 * travaillant � l'aide des op�rateurs de base de l'ALSU simplifi�e et pouvant utiliser les 
 * registres du CPU.
 *
 * La fonction main() vous permet de tester au fur et � mesure les fonctions que vous implantez.
 *
 * ----------------------------------------------------------------------------------------------
 *
 * author: B. Girau
 * version: 2016-2017
 */
#include <stdio.h>
#include <stdlib.h>

/////////////////////////////////////////////////////////
// d�finition de types
/////////////////////////////////////////////////////////

typedef struct {
  int size;
  int *word;
} Register;

typedef struct {
  Register accu;
  int* flags; // indicateurs Z C O N
} ALSU;

typedef struct {
  ALSU alsu;
  Register R0;
  Register R1;
  Register R2;
} CPU;

/////////////////////////////////////////////////////////
// fonctions d'initialisation
/////////////////////////////////////////////////////////

/*
 * allocation d'un mot/registre entier de 10 � 30 bits initialis� � 0
 */
Register reg(int size) {
  Register R;
  
  R.size = size;
  R.word = malloc(sizeof (int) * size);
  for (int i = 0; i < size; i++)
    R.word[i] = 0;
  return R;
}

/*
 * Initialisation du mot (mot de 10 � 30 bits, codant un entier en C�2) avec une valeur enti�re.
 */
void setValue(Register R,int n) {
  int	tmp = 0;
  int	i = 0;

  tmp = n;
  if (tmp < 0)
    tmp = tmp * -1;
  while (i < R.size)
    {
      if (tmp != 0)
	{
	  R.word[i] = tmp % 2;
	  tmp = tmp / 2;
	}
      else
	R.word[i] = 0;
      i++;
    }
  if (n < 0)
    {
      i = 0;
      tmp = 0;
      while (i < R.size)
	{
	  if (tmp == 1)
	    {
	      if (R.word[i] == 1)
		R.word[i] = 0;
	      else
		R.word[i] = 1;
	    }
	  if (R.word[i] == 1)
	    tmp = 1;
	  i++;
	}
    }
}

/*
 * instanciation d'un mot de 10 � 30 bits initialis� avec la valeur n
 */
Register initR(int size,int n) {
  Register R=reg(size);
  setValue(R,n) ;
  return R;
}
    
/*
 * Initialisation du mot (mot de 10 � 30 bits) par recopie des bits du mot en param�tre.
 */
void copyValue(Register R,Register src) {
  R.size = src.size;
  for (int i = 0; i < R.size; i++)
    R.word[i] = src.word[i];
}

/*
 * instanciation d'un mot de 10 � 30 bits initialis� par recopie d'un mot
 */
Register copyRegister(Register R) {
  Register C;

  setValue(C, R.size);
  for (int i = 0; i < C.size; i++)
    C.word[i] = R.word[i];
  return C;
}

/*
 * initialise l'ALSU
 */
ALSU initALSU(int size) {
  ALSU res;
  res.accu=reg(size);
  res.flags=(int*)malloc(4*sizeof(int));
  return res;
}

/*
 * initialise le CPU
 */
CPU initCPU(int size) {
  CPU res;
  res.alsu=initALSU(size);
  res.R0=reg(size);
  res.R1=reg(size);
  res.R2=reg(size);
  return res;
}

/////////////////////////////////////////////////////////
// fonctions de lecture
/////////////////////////////////////////////////////////

/*
 * Retourne la valeur enti�re sign�e repr�sent�e par le mot (compl�ment � 2).
 */    
int intValue(Register R) {
  int	vl = 0;

  for (int a = 0; a < (R.size - 1); a++)
    vl += R.word[a] * (1 << a);
  vl = vl - (R.word[R.size - 1] * (1 << (R.size - 1)));
  return vl;
}

/*
 * Retourne une cha�ne de caract�res d�crivant les 10 � 30 bits
  printf("%d\n", ~intValue(cpu.alsu.accu)); */
char* toString(Register R) {
  int i,j=0;
  char* s=(char*)malloc(40*sizeof(char));
  for (i=R.size-1;i>=0;i--) {
    if (R.word[i]==1) s[j]='1';
    else s[j]='0';
    j++;
  }
  return s;
}

/*
 * Retourne l'�criture des indicateurs associ�s � l'ALSU.
 */
char* flagsToString(ALSU alsu) {
  char *string=(char*)malloc(10*sizeof(char));
  sprintf(string,"z%dc%do%dn%d",alsu.flags[0],alsu.flags[1],alsu.flags[2],alsu.flags[3]);
  return string;
}

/*
 * affiche � l'�cran le contenu d'une ALSU
 */
void printing(ALSU alsu) {
  printf("Register Size : %d\n", alsu.accu.size);
  printf("Register word : %s\n", toString(alsu.accu));
  printf("ALSU Flags : ");
  for (int i = 0; i < 4; i++)
    printf(" %d",alsu.flags[i]);
  printf("\n");
}



/////////////////////////////////////////////////////////
// fonctions de manipulations �l�mentaires
/////////////////////////////////////////////////////////

/*
 * Positionne l'indicateur Z en fonction de l'�tat de l'accumulateur
 */
void setZ(ALSU alsu) {
  int	i = 0;

  while (i < alsu.accu.size)
    {
      if (alsu.accu.word[i] == 1)
	{
	  alsu.flags[0] = 0;
	  return ;
	}
      i++;
    }
  alsu.flags[0] = 1;
}

/////////////////////////////////////////////////////////
// op�rateurs de base de l'ALSU
/////////////////////////////////////////////////////////

/*
 * Stocke le param�tre dans le registre accumulateur
 */
void pass(ALSU alsu,Register B) {
  copyValue(alsu.accu, B);
  setZ(alsu);
  alsu.flags[1] = 0;
  alsu.flags[2] = 0;
  if (intValue(B) < 0)
    alsu.flags[3] = 1;
  else
    alsu.flags[3] = 0;
}

/*
 * Effectue un NAND (NON-ET) entre le contenu de l'accumulateur et le param�tre.
 */
void nand(ALSU alsu,Register B) {
  Register	A;
  int		i = 0;
  int		b = 0;
  int		r = 0;
  
  if (alsu.accu.size >= B.size)
    {
      A = reg(alsu.accu.size);
      while (i < alsu.accu.size)
	{
	  if (i > B.size)
	    b = 0;
	  else
	    b = B.word[i];
	  if (alsu.accu.word[i] == 1 && b == 1)
	    A.word[i] = 0;
	  else
	    A.word[i] = 1;
	  i++;
	}
    }
  else
    {
      A = reg(B.size);
      while (i < B.size)
	{
	  if (i > alsu.accu.size)
	    b = 0;
	  else
	    b = alsu.accu.word[i];
	  if (b == 1 && B.word[i] == 1)
	    A.word[i] = 0;
	  else
	    A.word[i] = 1;
	  i++;
	}
    }
  copyValue(alsu.accu, A);
  setZ(alsu);
  alsu.flags[1] = 0;
  alsu.flags[2] = 0;
  if (intValue(A) < 0)
    alsu.flags[3] = 1;
  else
    alsu.flags[3] = 0;
}

/*
 * D�cale le contenu de l'accumulateur de 1 bit vers la droite
 */
void shift(ALSU alsu) {
  for (int i = 0; i < alsu.accu.size; i++)
    alsu.accu.word[i] = alsu.accu.word[i] >> 1;
  alsu.accu.word[alsu.accu.size] = 0;
  setZ(alsu);
  alsu.flags[1] = 0;
  alsu.flags[2] = 0;
  if (intValue(alsu.accu) < 0)
    alsu.flags[3] = 1;
  else
    alsu.flags[3] = 0;
}

/*
 * module Full Adder : a+b+cin = s + 2 cout
 * retourne un tableau contenant s et cout
 */
int* fullAdder(int a,int b,int cin) {
  int* res=(int*)malloc(2*sizeof(int));

  if (a == 0 && b == 0 && cin == 0)
    {
      res[0] = 0;
      res[1] = 0;
    }
  else if (a == 0 && b == 0 && cin == 1)
    {
      res[0] = 1;
      res[1] = 0;
    }
  else if (a == 0 && b == 1 && cin == 0)
    {
      res[0] = 1;
      res[1] = 0;
    }
  else if (a == 1 && b == 0 && cin == 0)
    {
      res[0] = 1;
      res[1] = 0;
    }
  else if (a == 0 && b == 1 && cin == 1)
    {
      res[0] = 0;
      res[1] = 1;
    }
  else if (a == 1 && b == 0 && cin == 1)
    {
      res[0] = 0;
      res[1] = 1;
    }
  else if (a == 1 && b == 1 && cin == 0)
    {
      res[0] = 0;
      res[1] = 1;
    }
  else if (a == 1 && b == 1 && cin == 1)
    {
      res[0] = 1;
      res[1] = 1;
    }
  return res;
}

/*
 * Additionne le param�tre au contenu de l'accumulateur (addition enti�re C�2).
 * Les indicateurs sont positionn�s conform�ment au r�sultat de l'op�ration.
 */
void add(ALSU alsu,Register B) {
  int	a = 0;
  int	b = 0;
  int	*res = malloc(sizeof (int) * 2);
  int	cin = 0;
  Register R;

  if (alsu.accu.size >= B.size)
    R = reg(alsu.accu.size);
  else
    R = reg(B.size);
  for (int i = 0; i < alsu.accu.size || i < B.size; i++)
    {
      if (i < alsu.accu.size)
	a = alsu.accu.word[i];
      else
	a = 0;
      if (i < B.size)
	b = B.word[i];
      else
	b = 0;
      res = fullAdder(a, b, cin);
      R.word[i] = res[0];
      cin = res[1];
    }
  if ((alsu.accu.word[alsu.accu.size - 1] == 1 && B.word[B.size - 1] == 1)
      && R.word[R.size - 1] == 0)
    alsu.flags[2] = 1;
  else if ((alsu.accu.word[alsu.accu.size - 1] == 0 && B.word[B.size - 1] == 0)
	   && R.word[R.size - 1] == 1)
    alsu.flags[2] = 1;
  else
    alsu.flags[2] = 0;
  copyValue(alsu.accu, R);
  setZ(alsu);
  alsu.flags[1] = cin;
  if (intValue(alsu.accu) < 0)
    alsu.flags[3] = 1;
  else
    alsu.flags[3] = 0;
  free(res);
}

////////////////////////////////////////////////////////////////////
// Op�rations logiques :
////////////////////////////////////////////////////////////////////

/*
 * N�gation.
 */
void not(CPU cpu){
  nand(cpu.alsu, cpu.alsu.accu);
}

/*
 * Et.
 */
void and(CPU cpu,Register B) {
  nand(cpu.alsu, B);
  not(cpu);
}


/*
 * Ou.
 */
void or(CPU cpu,Register B)
{
  ALSU a;

  a = initALSU(B.size);
  copyValue(a.accu, B);
  nand(a, B);
  copyValue(cpu.R0, cpu.alsu.accu);
  nand(cpu.alsu, cpu.R0);
  nand(cpu.alsu, a.accu);
}

/*
 * Xor.
 */
void xor(CPU cpu,Register B) {
  Register	A;
  int		i = 0;
  int		b = 0;
  int		r = 0;
  
  if (cpu.alsu.accu.size >= B.size)
    {
      cpu.R0 = reg(cpu.alsu.accu.size);
      while (i < cpu.alsu.accu.size)
	{
	  if (i > B.size)
	    b = 0;
	  else
	    b = B.word[i];
	  if ((cpu.alsu.accu.word[i] == 0 && b == 1)
	      || (cpu.alsu.accu.word[i] == 1 && b == 0))
	    cpu.R0.word[i] = 1;
	  else
	    cpu.R0.word[i] = 0;
	  i++;
	}
    }
  else
    {
      cpu.R1 = reg(B.size);
      while (i < B.size)
	{
	  if (i > cpu.alsu.accu.size)
	    b = 0;
	  else
	    b = cpu.alsu.accu.word[i];
	  if ((b == 1 && B.word[i] == 0)
	      || (b == 0 && B.word[i] == 1))
	    cpu.R0.word[i] = 1;
	  else
	    cpu.R0.word[i] = 0;
	  i++;
	}
    }
  copyValue(cpu.alsu.accu, cpu.R0);
  setZ(cpu.alsu);
  cpu.alsu.flags[1] = 0;
  cpu.alsu.flags[2] = 0;
  if (intValue(A) < 0)
    cpu.alsu.flags[3] = 1;
  else
    cpu.alsu.flags[3] = 0;
}

/*
 * D�cale le receveur de |n| positions.
 * Le d�calage s'effectue vers la gauche si n>0 vers la droite dans le cas contraire.
 * C'est un d�calage logique (pas de report du bit de signe dans les positions 
 * lib�r�es en cas de d�calage � droite).
 * Les indicateurs sont positionn�s avec le dernier bit "perdu".
 */
void logicalShift(CPU cpu,int n) {
  if (n > 0)
    {
      copyValue(cpu.R0, cpu.alsu.accu);
      for (int i = 0; i < cpu.alsu.accu.size; i++)
	cpu.alsu.accu.word[i] = 0;
      for (int a = 0; a + n < cpu.alsu.accu.size; a++)
	cpu.alsu.accu.word[a + n] = cpu.R0.word[a];
    }
  else
    {
      copyValue(cpu.R1, cpu.alsu.accu);
      for (int i = 0; i < cpu.alsu.accu.size; i++)
	cpu.alsu.accu.word[i] = 0;
      for (int a = 0; a < cpu.alsu.accu.size; a++)
	{
	  if (a + n >= 0)
	    cpu.alsu.accu.word[a + n] = cpu.R1.word[a];
	}
    }
}

/////////////////////////////////////////////////////////
// Op�rations arithm�tiques enti�res
/////////////////////////////////////////////////////////

/*
 * Oppos�.
 */
void opp(CPU cpu) {
  not(cpu);
  cpu.R0 = initR(cpu.alsu.accu.size, 1);
  add(cpu.alsu, cpu.R0);
}

/*
 * Soustraction.
 */
void sub(CPU cpu,Register B) {
  CPU cpu2 = initCPU(B.size);

  copyValue(cpu2.alsu.accu, B);
  opp(cpu2);
  add(cpu.alsu, cpu2.alsu.accu);
}

/*
 * Multiplication.
 */
void mul(CPU cpu, Register B){
  ALSU res = initALSU(cpu.alsu.accu.size);

  for (int i = 0; i + 1 < B.size; i++)
    {
      if (B.word[i] == 1 && i == 0)
	copyValue(res.accu, cpu.alsu.accu);
      else if (B.word[i] == 0 && i == 0)
	for (int a = 0; a < res.accu.size; a++)
	  res.accu.word[a] = 0;
      logicalShift(cpu, 1);
      if (B.word[i + 1] == 1)
	copyValue(cpu.R0, cpu.alsu.accu);
      else
	for (int b = 0; b < cpu.R0.size; b++)
	  cpu.R0.word[b] = 0;
      add(res, cpu.R0);
    }
  copyValue(cpu.alsu.accu, res.accu);
}

/////////////////////////////////////////////////////////
// Programme de test
/////////////////////////////////////////////////////////

int main(int argc,char *argv[]) {
  
  // Champs priv�s
  
  Register operand;
  ALSU alsu;
  CPU cpu;
  
  int chosenInt,integer ;
  int go_on = 1 ;
  
  char* menu =     
    "              Programme de test\n\n0  Quitter\n1  setValue(operande,int)\n2  pass(alsu,operande)\n3  printing(alsu)\n4  afficher toString(operande)\n5  afficher intValue(operande)\n6  afficher intValue(accu)\n8  accu=add(accu,operande)\n9  accu=sub(accu,operande)\n10  accu=and(accu,operande)\n11 accu=or(accu,operande)\n12 accu=xor(accu,operande)\n13 accu=not(accu)\n14 accu=opp(accu)\n15 accu=logicalShift(accu,int)\n17 accu=mul(accu,operande)\n\n" ;
  
  char* invite = "--> Quel est votre choix  ? " ;
  
  printf("%s",menu) ; 

  operand=reg(20);
  cpu=initCPU(20);
  alsu=cpu.alsu;
  

  while (go_on==1) {
    printf("%s",invite);
    scanf("%d",&chosenInt);
    switch (chosenInt) {
    case 0 : 
      go_on = 0 ;
      break ;
    case 1 :
      printf("Entrez un nombre :"); 
      scanf("%d",&integer);
      setValue(operand,integer);
      break ;
    case 2 : 
      pass(alsu,operand);
      break ;
    case 3 : 
      printing(alsu);
      break ;
    case 4 : 
      printf("%s\n",toString(operand));
      break ;
    case 5 : 
      printf("intValue(operand)=%d\n",intValue(operand));
      break ;
    case 6 : 
      printf("intValue(accu)=%d\n",intValue(alsu.accu));
      break ;
    case 7 : 
      add(alsu,operand);
      printf("apres add(), accu = ");
      printing(alsu);
      break ;
    case 8 : 
      sub(cpu,operand);
      printf("apres sub(), A = ");
      printing(alsu);
      break ;
    case 9 : 
      and(cpu,operand);
      printf("apres and(), A = ");
      printing(alsu);
      break ;
    case 10 : 
      or(cpu,operand);
      printf("apres or(), A = ");
      printing(alsu);
      break ;
    case 11 : 
      xor(cpu,operand);
      printf("apres xor(), A = ");
      printing(alsu);
      break ;
    case 12 : 
      not(cpu);
      printf("apres not(), A = ");
      printing(alsu);
      break ;
    case 13 : 
      opp(cpu);
      printf("apres opp(), A = ");
      printing(alsu);
      break ;
    case 14 : 
      printf("Entrez un entier :") ;
      scanf("%d",&integer);
      logicalShift(cpu,integer);
      printf("apres logicalshift(), A = ");
      printing(alsu);
      break ;
    case 15 : 
      mul(cpu,operand);
      printf("apres mul(), A = ");
      printing(alsu);
      break ;
    default : 
      printf("Choix inexistant !!!!\n");
      printf("%s",menu);
    }
  }
  printf("Au revoir et a bientot\n");
  return 0;
}


  
