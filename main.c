#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>


typedef struct {
  int cap;
  int pref[4];
  int signed_to;
} Professor;


typedef struct {
  int size;
  int pref[2];
  int signed_to[2];
} Escola;

#define PROFESSOR_size 101
#define ESCOLA_size 51

Professor professor_array[PROFESSOR_size] = {0};
Escola escola_array[ESCOLA_size] = {0};


void usage(FILE *stream)
{
    fprintf(stream, "USAGE: program_name <input_file_path>\n");
}

char *slurp_file(const char *file_path, size_t *size)
{
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

char *strstrip(char *s)
{
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

void print_professor_array(void)
{
  for(size_t i = 1; i < PROFESSOR_size; ++i) {
    printf("P%zu (%d) -> ", i, professor_array[i].cap);
    
    for(size_t j = 0; j < 4; ++j) {
      printf("E%d, ", professor_array[i].pref[j]);
    }
    printf("\n");
  }
}

void print_escola_array(void)
{
  int professor_counter = 0; 
  for(size_t i = 1; i < ESCOLA_size; ++i) {
    printf("E%zu(%d)(%d) -> ", i, escola_array[i].pref[0], escola_array[i].pref[1]);
    
    for(size_t j = 0; j < 2; ++j) {
      int sin = escola_array[i].signed_to[j];
      if (sin == 0){
        printf("0, ");
        continue;
      } 
      professor_counter += 1;
      printf("%d(%d), ", sin, professor_array[sin].cap);
    }
    printf("\n");
  }
  printf("Number of professors: %d\n", professor_counter);
}


void generate_graph(char* content)
{
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


// Count non zero elements in array
int array_size(int *array, int size) {
  int counter = 0;

  for(int i = 0; i < size; i++) {
    if (array[i] != 0){
      counter++;
    }
  }
  return counter;
}
#define STACK_SIZE 101
int STACK[STACK_SIZE] = {0};
int stack_counter = 0;

void push(int a) {
  assert(stack_counter < STACK_SIZE);
  STACK[stack_counter] = a;
  stack_counter += 1;
}

int pop(void) {
  assert(stack_counter > 0);
  stack_counter -= 1; 
  return STACK[stack_counter];
}

bool prefer_new(Escola *escola, Professor *new_professor, Professor *old_professor, int n) {

  bool new = new_professor->cap >= escola->pref[n];
  bool old = old_professor->cap >= escola->pref[n];

  if (!old && new) {
    return true;
  }

  if (new && new_professor->cap < old_professor->cap) {
    return true;
  }

  return false;
}
int CAP_LIST_COUNTER[4] = {0};
int CAP_LIST[4][PROFESSOR_size] = {0};
int ESCOLA_COUNTER[ESCOLA_size] = {0};

void create_cap_list(void) {
  for(int i = 1; i < PROFESSOR_size; i++) {
    if (professor_array[i].signed_to != 0) continue;
    int cap = professor_array[i].cap;
    CAP_LIST[cap][CAP_LIST_COUNTER[cap]] = i;
    CAP_LIST_COUNTER[cap] += 1;
  }
}

void initialize_escola_counter(void) {
  for (int i = 1; i < ESCOLA_size; i++) {
    ESCOLA_COUNTER[i] = 0;
  }
}

void intitizalize_stack_with_escolas(void) {
  for(int i = 1; i < ESCOLA_size; i++) {
    push(i);
  }
}

int get_escola_minor_prefer_capacity(Escola *escola) {
  int first = escola->pref[0];
  int second = escola->pref[1];

  if (second == 0) return 0;
  if (first < second) return 0;
  return 1;
}

int get_index_in_professor_pref(int escola, int pref[4]) {
  for (int i = 0; i < 4 ; i++) {
    if (pref[i] == escola) return i + 1;
  }
  return 0;
}

bool prefer_new_escola(int new_escola_index, Professor *professor) {
  int old_escola_index = professor->signed_to;
  
  int old_order = get_index_in_professor_pref(old_escola_index, professor->pref);
  int new_order = get_index_in_professor_pref(new_escola_index, professor->pref);

  if (new_order == 0) return false;
  if (old_order == 0) return true;
  if (new_order < old_order) return true;
  return false;
}


void match_graphs2(void) {
  // Initializing Stuff
  create_cap_list();
  initialize_escola_counter();
  intitizalize_stack_with_escolas();

  while(stack_counter > 0) {
    int escola_index = pop();

    Escola *escola = &escola_array[escola_index];
    int minor_prefer_cap = get_escola_minor_prefer_capacity(escola);
    int prefer_cap = escola->pref[minor_prefer_cap];
    int escola_counter = ESCOLA_COUNTER[escola_index];
    int prefer_p_index = CAP_LIST[prefer_cap][escola_counter];
    
    bool found = false;
    while (prefer_cap <= 3) {
      escola_counter = ESCOLA_COUNTER[escola_index];
      prefer_p_index = CAP_LIST[prefer_cap][escola_counter];
      if (prefer_p_index == 0) {
        prefer_cap += 1;
        ESCOLA_COUNTER[escola_index] = 0;
        prefer_p_index = CAP_LIST[prefer_cap][escola_counter];
        if (prefer_cap > 3) continue;
      }

      Professor *prefer_professor = &professor_array[prefer_p_index];

      if (prefer_professor->signed_to == 0) {
        prefer_professor->signed_to = escola_index;
        escola->signed_to[minor_prefer_cap] = prefer_p_index;
        found = true;
      }
      else if (prefer_new_escola(escola_index, prefer_professor)) {
        int old_escola_index = prefer_professor-> signed_to;
        prefer_professor->signed_to = escola_index;
        escola->signed_to[minor_prefer_cap] = prefer_p_index;

        Escola *old_escola = &escola_array[old_escola_index];
        old_escola->signed_to[0] = 0;
        old_escola->signed_to[1] = 0;

        push(old_escola_index);
        found = true;
      }
      else {
        ESCOLA_COUNTER[escola_index] += 1;
      }
    }
  }
}

// Get first non zero element
int get_first(int array[4]) {
  for (int i = 0 ; i < 4; i++) {
    if (array[i] != 0) return array[i];
  }
  return 0;
}

void delete_first(int *array) {
  for (int i = 0; i < 4; i++)  {
    if (array[i] != 0) {
      array[i] = 0;
      return ;
    }
  }
  return ;
}

bool check_if_escola_has_espace(Escola *escola, Professor *professor) {
  int size = escola->size;
  
  for (int i=0; i < size; i++) {
    if (escola->signed_to[i] == 0 && professor->cap >= escola->pref[i]) return true;
  }
  return false;
}


bool can_work_in_school(Escola *escola, Professor *professor) {
  int prof_cap = professor->cap;
  int escola_size = escola->size;

  for (int i = 0; i < escola_size; i++) {
      if(prof_cap >= escola->pref[i]) return true;
  }
  return false;
  
}

bool check_if_new_professor_is_better(Escola *escola, Professor *professor) {
  for (int i =0; i < escola->size; i++) {
    int old_index = escola->signed_to[i];
    Professor *old_professor = &professor_array[old_index];
    if (professor->cap < escola->pref[i]) continue;
    if (professor->cap < old_professor->cap) return true;
  }
  return false;
}

bool check_if_new_professor_is_better_index(Escola *escola, Professor *professor, int index) {
  int old_index = escola->signed_to[index];
  Professor *old_professor = &professor_array[old_index];
  if (professor->cap < escola->pref[index]) return false;
  if (professor->cap < old_professor->cap) return true;
  return false;
}

void match_graphs(void) {
  for(int i = PROFESSOR_size - 1; i >=1 ;i--) {
    push(i);
  }

  while(stack_counter > 0) {
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

  // Adicionando um professor para todas as escolas
  // Checking if hava enough professor with 3 certificates
  // int n_3_escolas_certificates = 0;
  // for (int i = 1; i < ESCOLA_size; i++) {
  //   Escola *escola = &escola_array[i];
  //   if (escola->signed_to[0] == 0 && escola->pref[0] == 3) {
  //        n_3_escolas_certificates += 1;
  //   }
  // }
  // int n_3_professor_certificates = 0;
  // for (int i = 1; i < PROFESSOR_size; i++) {
  //   Professor *professor = &professor_array[i];
  //   if (professor->signed_to == 0 && professor->cap == 3) {
  //        n_3_professor_certificates += 1;
  //   }
  // }
  // if (n_3_escolas_certificates > n_3_professor_certificates) {
  //   // Adding new professor 3
  //   for (int i = 1; i < ESCOLA_size; i++) {
  //     Escola *escola = &escola_array[i];
  //     if (escola->signed_to[0] != 0 && escola->pref[0] == 3 &&
  //         escola->signed_to[1] != 0 && escola->pref[1] == 3) {
  //         int professor_index = escola->signed_to[1];
  //         escola->signed_to[1] = 0;
  //         Professor *professor = &professor_array[professor_index];
  //         professor->signed_to = 0;
  //     }
  //   }
  // }

  // Alocando para as escolas vazias
  // Creating list of professor with capacities
  create_cap_list();

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

  printf("-------------------------------\n\n");
  int aux_counter = 0;
  for (int i = 1 ; i < PROFESSOR_size; i++) {
    Professor *professor = &professor_array[i];
    if (professor->signed_to == 0) {
      aux_counter += 1;
      printf("P%d(%d)\n", i, professor->cap);
    }
  }
  printf("Numero de professor sem trabalho%d\n", aux_counter);


}

int main(int argc, char **argv)
{
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
  // print_professor_array();

  printf("---------------\n");
  print_escola_array();

  free(content);

  return 0;
}
