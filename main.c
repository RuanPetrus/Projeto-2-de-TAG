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
  for(size_t i = 1; i < ESCOLA_size; ++i) {
    printf("E%zu (%d) -> ", i, escola_array[i].size);
    
    for(size_t j = 0; j < 2; ++j) {
      printf("%d, ", escola_array[i].signed_to[j]);
    }
    printf("\n");
  }
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

// Get first non zero element
int get_first(int *array, int size) {
  for (int i = 0 ; i < size; i++) {
    if (array[i] != 0) return array[i];
  }
  return 0;
}

void delete_first(int *array, int size) {
  for (int i = 0; i < size; i++)  {
    if (array[i] != 0) {
      array[i] = 0;
      return;
    }
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
  bool old = true;
  if (old_professor->cap < escola->pref[n]) {
    old = false;
  }
  bool new = false;
  if (new_professor->cap >= escola->pref[n]) {
    new = true;
  }

  if (!old && new) {
    return true;
  }
  return false;

}

void match_graphs(void) {
  for(int i = PROFESSOR_size - 1; i >=1 ;i--) {
    push(i);
  }

  while(stack_counter > 0) {
    int current_professor_index = pop();
    Professor *current_professor = &professor_array[current_professor_index];

    int first_choice_index = get_first(current_professor->pref, 4);

    if (first_choice_index != 0) {
      Escola *first_choice = &escola_array[first_choice_index];
      
      bool match = false;
      for(int i = 0; i < first_choice->size; i++) {
        if (match) continue;

        int old_professor_index = first_choice->signed_to[i];
        if(old_professor_index == 0){
          first_choice->signed_to[i] = current_professor_index;
          current_professor->signed_to = first_choice_index;
          match = true;

          printf("Estava fazio professor %d - escola %d\n", current_professor_index, first_choice_index);
        }
        else if((i + 1) == first_choice->size && prefer_new(first_choice, 
                           current_professor, 
                           &professor_array[old_professor_index],
                           i)) {
          first_choice->signed_to[i] = current_professor_index;
          current_professor->signed_to = first_choice_index;
          Professor *old_professor = &professor_array[old_professor_index];
          old_professor->signed_to = 0;
          push(old_professor_index);
          match = true;

          printf("Professor %d trocado por %d pela escola %d\n", old_professor_index, current_professor_index, first_choice_index);
        }
      }
      if (!match) {
        push(current_professor_index);
        delete_first(current_professor->pref, 4);
      }
    }
  }
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
