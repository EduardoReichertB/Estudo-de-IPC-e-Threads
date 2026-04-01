# Estudo-de-IPC-e-Threads
Leitura e Processamento de Imagem (arquivos .pgm) utilizando conceitos de Threads e IPC
---
Compilar:
- gcc -o worker main_worker.c -lpthread
- gcc -o sender main_sender.c -lpthread

*Main Sender*: Recebe, lê e envia a imagem.pgm para o Main Worker através de uma named pipe (FIFO).

*Main Worker*: Recebe a imagem.pgm do Main Sender atravás da named pipe (FIFO), processa e salva a imagem processada.
