#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include "hash_utils.h"


/**
 * PROCESSO COORDENADOR - Mini-Projeto 1: Quebra de Senhas Paralelo
 * 
 * Este programa coordena múltiplos workers para quebrar senhas MD5 em paralelo.
 * O MD5 JÁ ESTÁ IMPLEMENTADO - você deve focar na paralelização (fork/exec/wait).
 * 
 * Uso: ./coordinator <hash_md5> <tamanho> <charset> <num_workers>
 * 
 * Exemplo: ./coordinator "900150983cd24fb0d6963f7d28e17f72" 3 "abc" 4
 * 
 * SEU TRABALHO: Implementar os TODOs marcados abaixo
 */

#define MAX_WORKERS 16
#define RESULT_FILE "password_found.txt"

/**
 * Calcula o tamanho total do espaço de busca
 * 
 * @param charset_len Tamanho do conjunto de caracteres
 * @param password_len Comprimento da senha
 * @return Número total de combinações possíveis
 */
long long calculate_search_space(int charset_len, int password_len) {
    long long total = 1;
    for (int i = 0; i < password_len; i++) {
        total *= charset_len;
    }
    return total;
}

/**
 * Converte um índice numérico para uma senha
 * Usado para definir os limites de cada worker
 * 
 * @param index Índice numérico da senha
 * @param charset Conjunto de caracteres
 * @param charset_len Tamanho do conjunto
 * @param password_len Comprimento da senha
 * @param output Buffer para armazenar a senha gerada
 */
void index_to_password(long long index, const char *charset, int charset_len, int password_len, char *output) {
    for (int i = password_len - 1; i >= 0; i--) {
        output[i] = charset[index % charset_len];
        index /= charset_len;
    }
    output[password_len] = '\0';
}

/**
 * Função principal do coordenador
 */
int main(int argc, char *argv[]) {
    // TODO 1: Validar argumentos de entrada
    // Verificar se argc == 5 (programa + 4 argumentos)
    // Se não, imprimir mensagem de uso e sair com código 1
    
    // IMPLEMENTE AQUI: verificação de argc e mensagem de erro
    if (argc != 5) {
        fprintf(stderr, "Uso: %s <hash> <tamanho> <charset> <workers>\n", argv[0]);
        return 1;
    }
    
    // Parsing dos argumentos (após validação)
    const char *target_hash = argv[1];
    int password_len = atoi(argv[2]);
    const char *charset = argv[3];
    int num_workers = atoi(argv[4]);
    int charset_len = strlen(charset);
    
    // TODO: Adicionar validações dos parâmetros
    // - password_len deve estar entre 1 e 10
    if(password_len<1 || password_len>10){
        printf("Senha não segue o tamanho adequado");
        return 1;
    }
    // - num_workers deve estar entre 1 e MAX_WORKERS
    if(num_workers<1 || num_workers>MAX_WORKERS){
        printf("Número de workers segue o tamanho adequado");
        return 1;
    }
    // - charset não pode ser vazio
    if (charset_len < 0) {
        printf("Charset não pode estar vazio");
        return 1;
    }
    
    printf("=== Mini-Projeto 1: Quebra de Senhas Paralelo ===\n");
    printf("Hash MD5 alvo: %s\n", target_hash);
    printf("Tamanho da senha: %d\n", password_len);
    printf("Charset: %s (tamanho: %d)\n", charset, charset_len);
    printf("Número de workers: %d\n", num_workers);
    
    // Calcular espaço de busca total
    long long total_space = calculate_search_space(charset_len, password_len);
    printf("Espaço de busca total: %lld combinações\n\n", total_space);
    
    // Remover arquivo de resultado anterior se existir
    unlink(RESULT_FILE);
    
    // Registrar tempo de início
    // time_t start_time = time(NULL);
    
    // TODO 2: Dividir o espaço de busca entre os workers
    // Calcular quantas senhas cada worker deve verificar
    // DICA: Use divisão inteira e distribua o resto entre os primeiros workers
    
    // IMPLEMENTE AQUI:
    long long total_possibilites = calculate_search_space(charset_len, password_len);
    long long passwords_per_worker = total_possibilites / num_workers;
    long long remaining = total_possibilites % num_workers; //remaining = 3

    // long long passwords_per_worker = ?
    // long long remaining = ?
    
    // Arrays para armazenar PIDs dos workers
    pid_t workers[MAX_WORKERS];
    
    // TODO 3: Criar os processos workers usando fork()
    printf("Iniciando workers...\n");
    
    // IMPLEMENTE AQUI: Loop para criar workers
    for (int i = 0; i < num_workers; i++) {
        // TODO: Calcular intervalo de senhas para este worker
        long long comeco_intervalo = i * passwords_per_worker;
        long long fim_intervalo = comeco_intervalo + (passwords_per_worker - 1);
        if (i < remaining) {
            fim_intervalo++;
        }
        // TODO: Converter indices para senhas de inicio e fim
        char comeco_password[password_len - 1];
        char fim_password[password_len - 1];
        index_to_password(comeco_intervalo, charset, charset_len, password_len, comeco_password);
        index_to_password(fim_intervalo, charset, charset_len, password_len, fim_password);
        // TODO 4: Usar fork() para criar processo filho
        // Lembre-se: fork() retorna 0 no filho, PID no pai, -1 em erro
        pid_t pid = fork();
        if (pid < 0) {
            // TODO 7: Tratar erros de fork() e execl()
            printf("Erro no fork(TODO 7 coordinator)!");//roberto deu erro
            exit(0);
        } else if (pid == 0) {
            // APENAS O FILHO EXECUTA AQUI
            // TODO 6: No processo filho: usar execl() para executar worker
            execl("./worker", "worker", target_hash, comeco_password, fim_password, charset, password_len, i, NULL);
            exit(0);
        } else {
            // APENAS O PAI EXECUTA AQUI
            // TODO 5: No processo pai: armazenar PID
            workers[i]=pid;
        }
    }
    
    printf("\nTodos os workers foram iniciados. Aguardando conclusão...\n");
    
    // TODO 8: Aguardar todos os workers terminarem usando wait()
    // IMPORTANTE: O pai deve aguardar TODOS os filhos para evitar zumbis
    
    // IMPLEMENTE AQUI:
    // - Loop para aguardar cada worker terminar
    // - Usar wait() para capturar status de saída
    // - Identificar qual worker terminou
    // - Verificar se terminou normalmente ou com erro
    // - Contar quantos workers terminaram
    int finalizados = 0;
    while (finalizados < num_workers) {
        int status = 0;
        pid_t pid = wait(&status);

        int define_worker = -1;
        for (int i = 0; i < num_workers; i++) {
            if (workers[i] == pid) {
                define_worker = i;
                break;
            }
        }

        if (WIFEXITED(status)) {
            int saida = WEXITSTATUS(status);
            printf("O mano numero %d tem o seguinte coddigo de saida: %d", define_worker, saida);
        }
        finalizados++;
    }


    // Registrar tempo de fim
    // time_t end_time = time(NULL);
    // double elapsed_time = difftime(end_time, start_time);
    
    printf("\n=== Resultado ===\n");
    
    // TODO 9: Verificar se algum worker encontrou a senha
    // Ler o arquivo password_found.txt se existir
    
    // IMPLEMENTE AQUI:
    // - Abrir arquivo RESULT_FILE para leitura
    // - Ler conteúdo do arquivo
    // - Fazer parse do formato "worker_id:password"
    // - Verificar o hash usando md5_string()
    // - Exibir resultado encontrado
    int fd = open(RESULT_FILE, O_RDONLY);
    if (fd >= 0) {
        char buffer[1024];
        ssize_t bytes_lidos = read(fd, buffer, sizeof(buffer) - 1); // -1 para deixar espaço para '\0'
        close(fd);
    
        if (bytes_lidos > 0) {
            buffer[bytes_lidos] = '\0'; // Garantir que é uma string válida
            
            char *ponteiro = strchr(buffer, ':');
            if (ponteiro != NULL) {
                *ponteiro = '\0'; // Agora seguro fazer isso
                char *worker_id = buffer;
                char *found_password = ponteiro + 1;
                
                char hash_calculado[33];
                md5_string(found_password, hash_calculado);
                
                if (strcmp(hash_calculado, target_hash) == 0) {
                    printf("✅ SENHA ENCONTRADA!\n");
                    printf("Worker ID: %s\n", worker_id);
                    printf("Senha: %s\n", found_password);
                    printf("Hash MD5: %s\n", hash_calculado);
                } else {
                    printf("❌ Falso positivo!\n");
                    printf("Senha reportada: %s\n", found_password);
                }
            } else {
                printf("❌ Formato inválido no arquivo\n");
            }
        } else {
            printf("❌ Arquivo de resultado vazio\n");
        }
    } else {
        printf("❌ Nenhuma senha encontrada\n");
    }
    return 0;
}