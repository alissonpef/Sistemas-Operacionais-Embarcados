# 🚁 RTOS para Controle de Drone - PIC18F4520

## 📖 Descrição do Projeto

Sistema Operacional de Tempo Real (RTOS) educacional implementado para microcontrolador PIC18F, com aplicação prática de **controle de drone com 4 motores**. Este projeto demonstra conceitos fundamentais de sistemas operacionais embarcados, incluindo multitarefa, escalonamento por prioridade, sincronização e comunicação entre processos.

---

## ✨ Características Principais

### 🎯 Funcionalidades do RTOS
- ✅ **Multitarefa Preemptiva** com até 5 tarefas simultâneas
- ✅ **Escalonamento por Prioridade** (ou Round-Robin)
- ✅ **Troca de Contexto** (Context Switching) via assembly
- ✅ **Sincronização:** Semáforos e Mutex
- ✅ **IPC:** Pipes (Filas de Mensagens)
- ✅ **Alocação Dinâmica de Memória** (Heap de 512 bytes)
- ✅ **Gerenciamento de Delays** e estados de tarefa

### 🚁 Aplicação: Controle de Drone
- ✅ **4 Motores Independentes:**
  - Motor 1 e 2: PWM por hardware (CCP1/CCP2)
  - Motor 3 e 4: PWM por software (GPIO)
- ✅ **Sensores:**
  - Giroscópio (ADC Canal 0)
  - Acelerômetro (ADC Canal 1)
  - Bateria (ADC Canal 2)
- ✅ **Modos de Operação:**
  - Normal: Velocidade dinâmica baseada nos sensores
  - Emergência: Velocidade fixa quando bateria < 200

---

## 🏗️ Arquitetura do Sistema

### Módulos do RTOS

```
kernel.c/h          → Núcleo do sistema (TCB, troca de contexto)
scheduler.c/h       → Escalonador (RR e Prioridade)
syscall.c/h         → Chamadas do sistema (create, delay, yield)
sync.c/h            → Semáforos e Mutex
pipe.c/h            → Comunicação entre tarefas (pipes)
mem.c/h             → Gerenciamento de memória dinâmica
hardware.c/h        → Interrupções e Timer0 (tick do sistema)
io.c/h              → Periféricos (ADC, PWM)
user_app.c/h        → Aplicação do usuário (drone)
```

### Tarefas da Aplicação

| Prioridade | Tarefa                        | Período | Função                           |
|------------|-------------------------------|---------|----------------------------------|
| P5 (Alta)  | `tarefa_controle_central`     | 100 ms  | Processa dados e calcula motores |
| P4         | `tarefa_controle_motores`     | 50 ms   | Aciona PWM dos 4 motores         |
| P3         | `tarefa_leitura_sensores`     | 150 ms  | Lê ADC (giroscópio, acelerômetro)|
| P2         | `tarefa_monitoramento_bateria`| 250 ms  | Lê nível da bateria              |
| P1 (Baixa) | `os_idle_task`                | -       | Pisca LED (sistema funcionando)  |

---

## 🔧 Hardware Utilizado

### Microcontrolador
- **Modelo:** PIC18F4520 (ou família PIC18F)
- **Clock:** 20 MHz
- **Memória:** RAM ~1.5KB, Flash ~32KB

### Mapeamento de Pinos

#### Saídas (Motores):
```
RC2 (CCP1)  → Motor 1 (PWM Hardware 10-bit)
RC1 (CCP2)  → Motor 2 (PWM Hardware 10-bit)
RD0 (GPIO)  → Motor 3 (PWM Software)
RD1 (GPIO)  → Motor 4 (PWM Software)
RD3 (GPIO)  → LED Status (Idle Task)
```

#### Entradas (Sensores):
```
AN0  → Giroscópio
AN1  → Acelerômetro
AN2  → Bateria
```

---

## 🚀 Como Compilar

### Requisitos
- **MPLAB X IDE** (v5.x ou superior)
- **XC8 Compiler** (v2.x ou superior)
- **Proteus** (para simulação - opcional)

### Passos

1. **Abrir o projeto:**
   ```
   MPLAB X → File → Open Project
   Selecionar a pasta do projeto
   ```

2. **Configurar linker (importante!):**
   ```
   Project Properties → XC8 Linker → Additional Options
   Adicionar: -mstack=software
   ```

3. **Compilar:**
   ```
   Build → Clean and Build Main Project
   ```

4. **Gravar no microcontrolador** ou **Simular no Proteus:**
   ```
   Proteus → Abrir: Simulacao/bf161rtos_simulation.pdsprj
   ```

---

## 📊 Fluxo de Execução

```
Timer0 (ISR) → Tick do sistema a cada ~ms
    ↓
Scheduler → Escolhe próxima tarefa (por prioridade)
    ↓
Troca de Contexto (SAVE_CONTEXT / RESTORE_CONTEXT)
    ↓
Tarefa executa até:
    • delay (bloqueia temporariamente)
    • yield (cede voluntariamente)
    • sem_wait (aguarda recurso)
    • Novo tick (preempção)
```

---

## 🧪 Como Testar

### Teste Básico (LED Idle)
1. Compilar e gravar
2. Verificar se LED no pino RD3 pisca
3. ✅ Sistema inicializou corretamente

### Teste dos Motores
1. Conectar LEDs nos pinos RC2, RC1, RD0, RD1
2. Conectar potenciômetros nos pinos AN0, AN1, AN2
3. Variar potenciômetro AN1 (acelerômetro)
4. ✅ LEDs devem variar brilho proporcionalmente

### Teste de Emergência
1. Ajustar potenciômetro AN2 (bateria) para < 200
2. ✅ Motores devem fixar velocidade em 50

Consulte **GUIA_DE_TESTES.md** para testes detalhados.

---

## 📚 Documentação Adicional

- **RESUMO_EXECUTIVO.md** → Visão geral das implementações
- **IMPLEMENTACAO_4_MOTORES.md** → Detalhes técnicos dos motores
- **DIAGRAMA_SISTEMA.md** → Diagramas visuais do sistema
- **GUIA_DE_TESTES.md** → Procedimentos de teste e troubleshooting

---

## 🎓 Conceitos Demonstrados

### Sistemas Operacionais
- [x] Escalonamento (Scheduling)
- [x] Troca de Contexto (Context Switching)
- [x] Estados de Processo (READY, RUNNING, WAITING)
- [x] Sincronização (Semáforos, Mutex)
- [x] Comunicação entre Processos (IPC - Pipes)
- [x] Gerenciamento de Memória (Heap)

### Sistemas Embarcados
- [x] Controle de Periféricos (ADC, PWM, Timers)
- [x] Interrupções (ISR)
- [x] Real-Time Constraints
- [x] Resource Management
- [x] Hardware Abstraction

---

## 🔮 Melhorias Futuras

### Prioridade Alta
- [ ] PWM por software real (Timer1) para motores 3 e 4
- [ ] Controle PID para estabilização
- [ ] Detecção de falhas de motor

### Prioridade Média
- [ ] Telemetria via UART
- [ ] Comandos remotos (Bluetooth/RF)
- [ ] Log de eventos em memória

### Prioridade Baixa
- [ ] Interface gráfica (LCD)
- [ ] Gravação de voo (black box)
- [ ] Controle por gestos

---

## 📊 Status do Projeto

### ✅ Implementado (v1.0)
- [x] Kernel completo
- [x] Escalonador por prioridade
- [x] 4 motores funcionais
- [x] Sincronização e IPC
- [x] Gerenciamento de memória
- [x] Aplicação de drone completa
- [x] Documentação extensiva

### 🚧 Em Desenvolvimento
- Nenhum item no momento (projeto completo)

---

## 🐛 Problemas Conhecidos

Nenhum problema crítico identificado. Consulte **GUIA_DE_TESTES.md** seção Troubleshooting para problemas comuns.

---

## 👥 Contribuidores

- **Desenvolvido por:** Alisson
- **Disciplina:** Sistemas Operacionais Embarcados 2025.2
- **Implementação dos 4 motores:** GitHub Copilot AI

---

## 📄 Licença

Este projeto é educacional e de código aberto. Livre para uso acadêmico.

---

## 📞 Suporte

Para dúvidas sobre:
- **Compilação:** Consulte seção "Como Compilar"
- **Testes:** Leia GUIA_DE_TESTES.md
- **Funcionamento:** Veja DIAGRAMA_SISTEMA.md
- **Motores:** Consulte IMPLEMENTACAO_4_MOTORES.md

---

## 🎉 Agradecimentos

Projeto desenvolvido como parte da disciplina de Sistemas Operacionais Embarcados, demonstrando a aplicação prática de conceitos de RTOS em sistemas de controle crítico.

**Status:** ✅ **PROJETO COMPLETO E FUNCIONAL**

---

**Última atualização:** 24 de Outubro de 2025  
**Versão:** 1.0 - Implementação Completa dos 4 Motores
