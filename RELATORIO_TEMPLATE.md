# Relatório: Mini-Projeto 1 - Quebra-Senhas Paralelo

**Aluno(s):** Carolina Lee (10440304), Pedro Gabriel Guimarães Fernandes (10437465), Tarik Ferreira (10435895), Pedro Casas Pequeno Júnior (10437031)  
---

## 1. Estratégia de Paralelização


**Como você dividiu o espaço de busca entre os workers?**

O nosso algoritmo nos usamos 3 variaveis long long para guardar o total de posibilidades, o número de senhas por worker e os que sobram da divisão,
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

[Explique em um parágrafo como você criou os processos, passou argumentos e esperou pela conclusão]

**Código do fork/exec:**
```c
// Cole aqui seu loop de criação de workers
```

---

## 3. Comunicação Entre Processos

**Como você garantiu que apenas um worker escrevesse o resultado?**

[Explique como você implementou uma escrita atômica e como isso evita condições de corrida]
Leia sobre condições de corrida (aqui)[https://pt.stackoverflow.com/questions/159342/o-que-%C3%A9-uma-condi%C3%A7%C3%A3o-de-corrida]

**Como o coordinator consegue ler o resultado?**

[Explique como o coordinator lê o arquivo de resultado e faz o parse da informação]

---

## 4. Análise de Performance
Complete a tabela com tempos reais de execução:
O speedup é o tempo do teste com 1 worker dividido pelo tempo com 4 workers.

| Teste | 1 Worker | 2 Workers | 4 Workers | Speedup (4w) |
|-------|----------|-----------|-----------|--------------|
| Hash: 202cb962ac59075b964b07152d234b70<br>Charset: "0123456789"<br>Tamanho: 3<br>Senha: "123" | ___s | ___s | ___s | ___ |
| Hash: 5d41402abc4b2a76b9719d911017c592<br>Charset: "abcdefghijklmnopqrstuvwxyz"<br>Tamanho: 5<br>Senha: "hello" | ___s | ___s | ___s | ___ |

**O speedup foi linear? Por quê?**
[Analise se dobrar workers realmente dobrou a velocidade e explique o overhead de criar processos]

---

## 5. Desafios e Aprendizados
**Qual foi o maior desafio técnico que você enfrentou?**
[Descreva um problema e como resolveu. Ex: "Tive dificuldade com o incremento de senha, mas resolvi tratando-o como um contador em base variável"]

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
