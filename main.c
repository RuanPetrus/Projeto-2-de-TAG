// Ruan Petrus
// 2022-1
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>


// Representa a strutura de um professor
typedef struct {
  int cap;
  int pref[4];
  int signed_to;
} Professor;


// Rpresenta a estrutura de uma escola
typedef struct {
  int size;
  int pref[2];
  int signed_to[2];
} Escola;

#define PROFESSOR_size 101
#define ESCOLA_size 51

Professor professor_array[PROFESSOR_size] = {0};
Escola escola_array[ESCOLA_size] = {0};

// Implementam uma estrutara de fila
#define QUEUE_SIZE 1024
int QUEUE[QUEUE_SIZE] = {0};
int Rear = - 1;
int Front = - 1;
int queue_counter = 0;

// Representa um array relacinando professores com capacidades
int CAP_LIST_COUNTER[4] = {0};
int CAP_LIST[4][PROFESSOR_size] = {0};
int ESCOLA_COUNTER[ESCOLA_size] = {0};

// Adiciona um elemento na fila
void push(int insert_item){

    if (Rear == QUEUE_SIZE - 1)
       printf("Overflow \n");
    else {
        if (Front == - 1)
      
        Front = 0;
        Rear = Rear + 1;
        QUEUE[Rear] = insert_item;
        queue_counter += 1;
    }
} 
 
// Retira um elemento da fila
int pop(void){
  if (Front == - 1 || Front > Rear) { 
      printf("Underflow \n");
      return 0;
  }
  else {
      queue_counter -= 1; 
      int item = QUEUE[Front];
      Front = Front + 1;
      return item;
  }
}

// Cria uma lista relacionando a capacidade com o professor
void create_cap_list(void) {
  for(int i = 1; i < PROFESSOR_size; i++) {
    if (professor_array[i].signed_to != 0) continue;
    int cap = professor_array[i].cap;
    CAP_LIST[cap][CAP_LIST_COUNTER[cap]] = i;
    CAP_LIST_COUNTER[cap] += 1;
  }
}

// Printa no stream como utilizar o programa
void usage(FILE *stream){
    fprintf(stream, "USAGE: program_name <input_file_path>\n");
}

// Le um arquivo e devolve o conteudo em forma de string
char *slurp_file(const char *file_path, size_t *size){
    char *buffer = NULL;

    FILE *f = fopen(file_path, "rb");
    if (f == NULL) {
        goto error;
    }

    if (fseek(f, 0, SEEK_END) < 0) {
        goto error;
    }

    long m = ftell(f);
    if (m < 0) {
        goto error;
    }

    buffer = malloc(sizeof(char) * (m + 1));

    if (buffer == NULL) {
        goto error;
    }

    if (fseek(f, 0, SEEK_SET) < 0 ) {
        goto error;
    }

    size_t n = fread(buffer, 1, m, f);

    assert(n == (size_t) m);

    if (ferror(f)) {
        goto error;
    }

    if (size) {
        *size = n;
    }

    fclose(f);

    // Add null terminated
    buffer[m + 1] = '\0';
    return buffer;

error:  
    if (f) {
        fclose(f);
    }

    if (buffer) {
        free(buffer);
    }

    return NULL;
}

// Retira os espaços na frente e atras de uma string
char *strstrip(char *s){
        size_t size;
        char *end;

        size = strlen(s);

        if (!size)
                return s;

        end = s + size - 1;
        while (end >= s && isspace(*end))
                end--;
        *(end + 1) = '\0';

        while (*s && isspace(*s))
                s++;

        return s;
}

// Printa a array de professores
void print_professor_array(void){
  for(size_t i = 1; i < PROFESSOR_size; ++i) {
    printf("P%zu : E%d -> ", i, professor_array[i].signed_to);
    
    for(size_t j = 0; j < 4; ++j) {
      printf("E%d, ", professor_array[i].pref[j]);
    }
    printf("\n");
  }
}

// Printa a array de escolas
void print_escola_array(void){
  printf("------------------------------------------------\n");
  printf("Alocacao de professores\n");
  printf("\n");
  int professor_counter = 0; 
  for(size_t i = 1; i < ESCOLA_size; ++i) {
    printf("E%zu -> ", i);
    
    for(size_t j = 0; j < 2; ++j) {
      int sin = escola_array[i].signed_to[j];
      if (sin == 0){
        continue;
      } 
      professor_counter += 1;
      printf("P%d ", sin);
    }
    printf("\n");
  }
  printf("------------------------------------------------\n");
  printf("Numero de professores alocados estável máximo: %d\n", professor_counter);
  printf("\n");
}

// Cria as estruturas de dados através da string
void generate_graph(char* content){
  char *curLine = content;
  while(curLine){
    char *nextLine = strchr(curLine, '\n');
    if (nextLine) *nextLine = '\0';  // temporarily terminate the current line

    char *trim_line = strstrip(curLine);

    if ((strlen(trim_line) != 0) && (trim_line[0] != '/')) {
      trim_line += 1;
      
      if (trim_line[0] == 'P') {
        // Reading index
        trim_line += 1;
        char *v_pointer = strchr(trim_line, ',');
        *v_pointer = '\0';
        int index = atoi(trim_line);

        // Creating professor
        Professor professor;
        professor.pref[0] = 0;
        professor.pref[1] = 0;
        professor.pref[2] = 0;
        professor.pref[3] = 0;
        professor.signed_to = 0;

        // Reading cap
        trim_line = v_pointer + 1;
        professor.cap = atoi(trim_line);

        // Reading pref
        trim_line = strchr(trim_line, '(');
        
        int counter = 0;
        while(trim_line[0] != ')') {
          if(trim_line[0] == 'E') {
          ++trim_line;
            professor.pref[counter] = atoi(trim_line);
            ++counter;
          }
          ++trim_line;
        }
          
        professor_array[index] = professor;
      }
      else {
        ++trim_line;
        int index = atoi(trim_line);

        // Creating escola
        Escola escola;
        escola.pref[0] = 0;
        escola.pref[1] = 0;
        escola.signed_to[0]= 0;
        escola.signed_to[1]= 0;

        int counter = 0;
        while(trim_line[0] != '\0') {
          if(trim_line[0] == '(') {
            ++trim_line;
            escola.pref[counter] = atoi(trim_line);
            ++counter;
          }
          ++trim_line;
        }

        escola.size = counter;
        escola_array[index] = escola;
      }
    }

    if (nextLine) *nextLine = '\n';  // then restore newline-char, just to be tidy    
    curLine = nextLine ? (nextLine+1) : NULL;
  }

  return;
}

// Retorna o primeiro elemento que não é zero
int get_first(int array[4]) {
  for (int i = 0 ; i < 4; i++) {
    if (array[i] != 0) return array[i];
  }
  return 0;
}

// Faz o primeiro elemento que nao é zero ser zero
void delete_first(int *array) {
  for (int i = 0; i < 4; i++)  {
    if (array[i] != 0) {
      array[i] = 0;
      return ;
    }
  }
  return ;
}

// Olha se tem espaço dentro de escola
bool check_if_escola_has_espace(Escola *escola, Professor *professor) {
  int size = escola->size;
  
  for (int i=0; i < size; i++) {
    if (escola->signed_to[i] == 0 && professor->cap >= escola->pref[i]) return true;
  }
  return false;
}

// Verica se o professor tem a capacidade de trabalhar na escola
bool can_work_in_school(Escola *escola, Professor *professor) {
  int prof_cap = professor->cap;
  int escola_size = escola->size;

  for (int i = 0; i < escola_size; i++) {
      if(prof_cap >= escola->pref[i]) return true;
  }
  return false;
  
}

// Verifica se o novo professor é melhor que algum dos antigos
bool check_if_new_professor_is_better(Escola *escola, Professor *professor) {
  for (int i =0; i < escola->size; i++) {
    int old_index = escola->signed_to[i];
    Professor *old_professor = &professor_array[old_index];
    if (professor->cap < escola->pref[i]) continue;
    if (professor->cap < old_professor->cap) return true;
  }
  return false;
}

// Verifica se o novo professor é melhor que o professor na posicao do index
bool check_if_new_professor_is_better_index(Escola *escola, Professor *professor, int index) {
  int old_index = escola->signed_to[index];
  Professor *old_professor = &professor_array[old_index];
  if (professor->cap < escola->pref[index]) return false;
  if (professor->cap < old_professor->cap) return true;
  return false;
}

// Faz o match máximo e estavel do grafico
void match_graphs(void) {
  // Inicializa a fila
  for(int i = 1; i < PROFESSOR_size; i++) {
    push(i);
  }
  // Algorimtmo de match estavel
  while(queue_counter > 0) {
    int current_professor_index = pop();
    Professor *current_professor = &professor_array[current_professor_index];

    int first_choice_index = get_first(current_professor->pref);

    if (first_choice_index != 0) {
      Escola *first_choice = &escola_array[first_choice_index];
      
      if(!can_work_in_school(first_choice, current_professor)) {
        delete_first(current_professor->pref);
        push(current_professor_index);
      }

      else if(check_if_escola_has_espace(first_choice, current_professor)){
        if (first_choice->signed_to[0] == 0 && current_professor->cap >= first_choice->pref[0]) {
          first_choice->signed_to[0] = current_professor_index;
          current_professor->signed_to = first_choice_index;
        }
        else {
          first_choice->signed_to[1] = current_professor_index;
          current_professor->signed_to = first_choice_index;
        }
      }
      else if(check_if_new_professor_is_better(first_choice, current_professor)) {
        if (check_if_new_professor_is_better_index(first_choice, current_professor, 0)) {
          int old_professor_index = first_choice->signed_to[0];
          first_choice->signed_to[0] = current_professor_index;
          current_professor->signed_to = first_choice_index;
          Professor *old_professor = &professor_array[old_professor_index];
          old_professor->signed_to = 0;
          push(old_professor_index);
        }
        else {
          int old_professor_index = first_choice->signed_to[1];
          first_choice->signed_to[1] = current_professor_index;
          current_professor->signed_to = first_choice_index;
          Professor *old_professor = &professor_array[old_professor_index];
          old_professor->signed_to = 0;
          push(old_professor_index);
        }
      }
      else {
        delete_first(current_professor->pref);
        push(current_professor_index);
      }
    }
  }

  // Cria a lista de capaciadades
  create_cap_list();

  // Adicionando um professor para todas as escolas
  for (int j = 3; j >= 1; j--) {
    int counter = 0;
    for (int i = 1; i < ESCOLA_size; i++) {
      Escola *escola = &escola_array[i];

      if (escola->signed_to[0] == 0 && escola->signed_to[1] == 0 && escola->pref[0] == j) {
        int professor_index = CAP_LIST[j][counter];
        Professor *prof = &professor_array[professor_index];
        if (prof->signed_to != 0) continue;
        escola->signed_to[0] = professor_index;
        prof->signed_to = i;
        counter += 1;
      }
    }
  }

  // Adiciona o resto dos professores onde possivel
  for (int k = 3; k >= 1; k--) {
    for (int j = 1; j < PROFESSOR_size; j++) {
      Professor *professor = &professor_array[j];
      bool has_been_signed = false;
      if (professor->signed_to == 0 && professor->cap == k) {
        for (int i = 1; i < ESCOLA_size; i++) {
          if (has_been_signed) break;
          Escola *escola = &escola_array[i];
          for (int z = 0; z < escola->size; z++) {
            if (escola->signed_to[z] == 0 && escola->pref[z] == k) {
              escola->signed_to[z] = j;
              professor->signed_to = i;
              has_been_signed = true;
            }
          }
        }
      }
    }
  }
}

int main(int argc, char **argv){
  if (argc < 2) {
    fprintf(stderr, "ERROR: not enough arguments provided\n");
    usage(stderr);
    exit(1);
  }

  char *input_file_path = argv[1];

  size_t size;
  char *content = strstrip(slurp_file(input_file_path, &size));
  
  if (content == NULL) {
    fprintf(stderr, "ERROR: not possible to open input file: %s\n", input_file_path);
    usage(stderr);
    exit(1);
  }

  generate_graph(content);
  match_graphs();
  print_escola_array();

  free(content);

  return 0;
}
