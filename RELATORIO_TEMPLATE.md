# Relatório: Mini-Projeto 1 - Quebra-Senhas Paralelo

**Aluno(s):** Carolina Lee (10440304), Pedro Gabriel Guimarães Fernandes (10437465), Tarik Ferreira (10435895), Pedro Casas Pequeno Júnior (10437031)  
---

## 1. Estratégia de Paralelização


**Como você dividiu o espaço de busca entre os workers?**

No nosso algoritmo, usamos 3 variaveis long long para guardar o total de posibilidades, o número de senhas por worker e os que sobram da divisão,
primeiro achamos  total de possibilidades usando a calculate_search_space para calcular a quantidade de senhas possiveis para o charset e tamanho de senha escolhido,
depois pegamos esse total e dividimos pelo número de workers, assim obtendo quantas senhas cada worker deve procurar, porém como essa divisão pode dar algumas sobras
também guardamos o resto dessa divisão para dividir essas senhas que sobraram, entre alguns workers quando assimilamos a quantidade de senhas que eles iram que procurar ao criar eles.

**Código relevante:** Cole aqui a parte do coordinator.c onde você calcula a divisão:
```c
// Cole seu código de divisão aqui
```
    long long total_possibilites = calculate_search_space(charset_len, password_len);
    long long passwords_per_worker = total_possibilites / num_workers;
    long long remaining = total_possibilites % num_workers; 
---

---

## 2. Implementação das System Calls

**Descreva como você usou fork(), execl() e wait() no coordinator:**

Os processos são criados usando um loop que percorre o número de workers desejados. Para cada worker, calcula-se o intervalo de senhas que ele deve testar, convertendo os índices de início e fim em strings de senha usando a função index_to_password. Em seguida, fork() é usado para criar um processo filho; no filho (pid == 0), chamamos execl() para executar o programa worker, passando como argumentos o hash alvo, as senhas de início e fim, o charset, o tamanho da senha e o ID do worker. O processo pai (pid > 0) armazena o PID do filho em um array para controle. Após criar todos os filhos, o pai entra em um loop com wait(), que aguarda cada filho terminar, captura seu status de saída e identifica qual worker terminou com base no PID, permitindo contar e monitorar a conclusão de todos os workers de forma organizada, evitando processos zumbis.


**Código do fork/exec:**
```c
// IMPLEMENTE AQUI: Loop para criar workers
    for (int i = 0; i < num_workers; i++) {
        // TODO: Calcular intervalo de senhas para este worker
        long long comeco_intervalo = i * passwords_per_worker;
        long long fim_intervalo = comeco_intervalo + (passwords_per_worker - 1);
        if (i < remaining) {
            fim_intervalo++;
        }
        // TODO: Converter indices para senhas de inicio e fim
        char comeco_password[password_len + 1];
        char fim_password[password_len + 1];
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
            char passlen_str[16], workerid_str[16];
            sprintf(passlen_str, "%d", password_len);
            sprintf(workerid_str, "%d", i);
            // TODO 6: No processo filho: usar execl() para executar worker
            execl("./worker", "worker", target_hash, comeco_password, fim_password, charset, passlen_str, workerid_str, (char*)NULL);
            exit(0);
        } else {
            // APENAS O PAI EXECUTA AQUI
            // TODO 5: No processo pai: armazenar PID
            workers[i]=pid;
        }
    }
```

---

## 3. Comunicação Entre Processos

**Como você garantiu que apenas um worker escrevesse o resultado?**

[Explique como você implementou uma escrita atômica e como isso evita condições de corrida]
Leia sobre condições de corrida (aqui)[https://pt.stackoverflow.com/questions/159342/o-que-%C3%A9-uma-condi%C3%A7%C3%A3o-de-corrida]

A operação open(), com os parâmetros O_CREAT, O_EXCL e O_WRONLY, faz com que o kernel garante que apenas um processo pode cirar o arquivo com sucesso. Para previnir as condições de corrida, quando os workers tentam salvar os resultados simultaneamente, apenas o primeiro que chamar na função open() terá sucesso.

**Como o coordinator consegue ler o resultado?**

[Explique como o coordinator lê o arquivo de resultado e faz o parse da informação]

O coordinator tenta abrir password_found.txt para leitura, depois, lê todo o conteúdo do arquivo, e então, divide a string usando ":" para separar o ID do worker e a senha encontrada, verifica o hash encontrado com o hash que queremos encontrar, se der certo, imprime o resultado.

---

## 4. Análise de Performance
Complete a tabela com tempos reais de execução:
O speedup é o tempo do teste com 1 worker dividido pelo tempo com 4 workers.

| Teste | 1 Worker | 2 Workers | 4 Workers | Speedup (4w) |
|-------|----------|-----------|-----------|--------------|
| Hash: 202cb962ac59075b964b07152d234b70<br>Charset: "0123456789"<br>Tamanho: 3<br>Senha: "123" | 0m0.006s | 0m0.007s | 0m0.007s | 0.86 |
| Hash: 5d41402abc4b2a76b9719d911017c592<br>Charset: "abcdefghijklmnopqrstuvwxyz"<br>Tamanho: 5<br>Senha: "hello" | 2m2.454s (122.454s) | 4m17.682s (257.682s) | 0m54.947s (54.947s) | 2.23 |

**O speedup foi linear? Por quê?**
[Analise se dobrar workers realmente dobrou a velocidade e explique o overhead de criar processos]
Não, o speedup não foi linear. Para ele ser teriamos que ter um resultad0 4 vezes mais rápido, mas obtivemos: 0.86x (piora no desempenho) e 2.23x (56% da eficiência desejada).
O overhead é o custo adicional envolvido na operação de um sistema, na criação de processos ele representa todo o tempo e recursos gastos para criar e gerenciar esses processos. As vezes ele pode superar o ganho que deveriamos ter com o paralelismo, o que faz com que mesmo com mais workers, o tempo de execução continua o mesmo ou até piora, como é o caso do 123. Como a senha é pequena e o espaço de busca também, o processo de criar tudo, competir pelos recuros da cpu, e a troca de contexto entre os processos, faz com que o ganho de desempenho seja insignificante ou até mesmo nulo. Já no "hello", podemos ver uma melhora já que o espaço de busca é bem maior. Se o overhead ddo gerenciamento de processos for maior que o trabalho, o paralelismo não vale a pena.
---

## 5. Desafios e Aprendizados
**Qual foi o maior desafio técnico que você enfrentou?**
[Descreva um problema e como resolveu. Ex: "Tive dificuldade com o incremento de senha, mas resolvi tratando-o como um contador em base variável"]

Tivemos problema com o wait() sendo bem sincero tivemos dificuldade de entender como ele funcionava, tivemos que recorrer a sites e a documentação para entender como ele funcionava e assim aplica-lo de forma correta. Entendemos que ele era importante para fazer com que os processos sejam sincronizados e para evitar processos zumbis.
---

## Comandos de Teste Utilizados

```bash
# Teste básico
./coordinator "900150983cd24fb0d6963f7d28e17f72" 3 "abc" 2

# Teste de performance
time ./coordinator "202cb962ac59075b964b07152d234b70" 3 "0123456789" 1
time ./coordinator "202cb962ac59075b964b07152d234b70" 3 "0123456789" 4

# Teste com senha maior
time ./coordinator "5d41402abc4b2a76b9719d911017c592" 5 "abcdefghijklmnopqrstuvwxyz" 4
```
---

**Checklist de Entrega:**
- [ ] Código compila sem erros
- [ ] Todos os TODOs foram implementados
- [ ] Testes passam no `./tests/simple_test.sh`
- [ ] Relatório preenchido
