#include "trabalhOS.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Dados compartilhados para processamento
PGM g_imagem;
int g_mode; // MODE_NEG ou MODE_SLICE
int g_t1, g_t2;

void aplicar_negativo(void* arg);
void aplicar_negativo_sem_thread(Task* tarefa, PGM* imagem);

int main(int argc, char* argv[]){
  Header cabecalho;

  //abre a fifo
  const char* path = FIFO_PATH;
  mkfifo(path, 0666); //cria a named pipe

  //recebe os dados enviados pelo sender
  int fd;
  fd = open(path, O_RDONLY);
  printf("Recebendo dados...\n");

  read(fd, &cabecalho, sizeof(Header));
  printf("Cabecalho recebido.\n");
  g_imagem.w = cabecalho.w;
  g_imagem.h = cabecalho.h;
  g_imagem.maxv = cabecalho.maxv;
  printf("altura: %d\n largura: %d\n maxv: %d\n", g_imagem.h, g_imagem.w, g_imagem.maxv);

  g_imagem.data = (unsigned char*)malloc(g_imagem.w * g_imagem.h * sizeof(unsigned char));
  size_t tamanho_esperado = g_imagem.w * g_imagem.h;
  printf("Imagem enviada: %ld bytes\n", tamanho_esperado);
  size_t tamanho_lido = 0;
  while(tamanho_lido < tamanho_esperado){
    size_t n = read(fd, g_imagem.data + tamanho_lido, tamanho_esperado - tamanho_lido);
    tamanho_lido += n;
  }
  printf("Imagem recebida: %ld bytes\n", tamanho_lido);

  close(fd); //terminamos de receber as informações

  pthread_t thread[4]; //dividir as tarefas em 4 threads
  Task tarefa[4];

  //divide as 874 linhas da imagem entre as tarefas - DEPOIS CRIAR UMA FUNÇÃO QUE FAÇA AUTOMATICO COM BASE NO TAMANHO DA IMAGEM ENVIADA
  tarefa[0].row_start = 0;
  tarefa[0].row_end = 100;   //218
  tarefa[1].row_start = 100; //218
  tarefa[1].row_end = 200;   //437
  tarefa[2].row_start = 200; //437
  tarefa[2].row_end = 300;   //656
  tarefa[3].row_start = 300; //656
  tarefa[3].row_end = 400;   //874

  for(int i = 0; i < 4; i++){
    pthread_create(&thread[i], NULL, (void *)aplicar_negativo, &tarefa[i]);
    //aplicar_negativo_sem_thread(&tarefa[i], &g_imagem);
  }
  pthread_join(thread[0], NULL);
  pthread_join(thread[1], NULL);
  pthread_join(thread[2], NULL);
  pthread_join(thread[3], NULL);

  pthread_mutex_destroy(&mutex);

  write_PGM("saida.pgm", &g_imagem);

  free(g_imagem.data);

  return 0;
}

void aplicar_negativo(void* arg){
  Task* temp_task = (Task*)arg;
  int i = temp_task->row_start;

  printf("Thread processando linhas %d até %d\n", temp_task->row_start, temp_task->row_end);

  for(i; i < temp_task->row_end; i++){
    for(int j = 0; j < g_imagem.w; j++){
      int pos = i * g_imagem.w + j;
      pthread_mutex_lock(&mutex);
      g_imagem.data[pos] = 255 - g_imagem.data[pos];
      pthread_mutex_unlock(&mutex);
    }
  }
}

void aplicar_negativo_sem_thread(Task* tarefa, PGM* imagem){
  int i = tarefa->row_start;

  printf("Thread processando linhas %d até %d\n", tarefa->row_start, tarefa->row_end);

  for(i; i < tarefa->row_end; i++){
    for(int j = 0; j < g_imagem.w; j++){
      int pos = i * g_imagem.w + j;
      imagem->data[pos] = 255 - imagem->data[pos];
    }
  }
}


