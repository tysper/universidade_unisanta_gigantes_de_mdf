# Gigantes de MDF - Trabalho de Programação de Hardware 2024 Unisanta

<div style="display:flex;flex-direction:column;justify-content:center;align-items:center;">
    <img src="./scripts/documentacao/html/logo.jpg" style="width:200px;">
</div>

## Introdução

Este projeto integra os conhecimentos adquiridos na disciplina de Programação de Hardware do curso de Engenharia. Ele tem como objetivo principal o desenvolvimento de um robô funcional, com foco na programação do microcontrolador **ATMega328(p)(PB)**, utilizando periféricos como **PWM** e timers, além da implementação de um circuito eletrônico completo. A abordagem do projeto prioriza o aprendizado prático e evita o uso de soluções pré-prontas, garantindo que os participantes desenvolvam competências fundamentais na área.

## Objetivo do Projeto

Construir um robô funcional capaz de:
- Movimentar-se autonomamente utilizando motores controlados via PWM.
- Operar uma "arma laser" que dispara feixes de luz vermelha por 1 segundo, controlada por temporizadores.
- Detectar impactos de laser recebidos e reagir com ações programadas, como giros no eixo e decremento de LEDs que indicam "vidas".
- Integrar todos os componentes eletrônicos, incluindo sensores, LEDs e botões, em um circuito projetado manualmente.

## Requisitos Técnicos

### Mecânica
- Estrutura com dimensões máximas de **200x200x200 mm**.
- Sistema de movimentação em 4, 3 ou 2 rodas, permitindo rotações no próprio eixo.
- "Arma laser" centralizada e receptor de luz posicionado lateralmente.

### Eletrônica
- Circuitos para controle de motores projetados manualmente (sem shields prontos).
- Laser com potência máxima de **300 mW** e receptor de luz com difusor de 20 mm.
- LEDs indicativos para "vidas", com controle de estado por GPIO e um botão para reset.

### Programação
- Uso obrigatório de timers para controle do laser.
- Interrupções para funcionalidade do botão de reset.
- Controle dos motores baseado em PWM.
- Reação programada ao impacto de laser: desativação temporária do robô e decremento de LEDs.

## Estrutura de Desenvolvimento

###  1. Protótipo Funcional - 5 de outubro de 2024 ✅
O protótipo inicial do robô foi concluído, com movimentação básica utilizando os motores conectados a baterias. A estrutura mecânica foi montada conforme as especificações e demonstrada com sucesso.

###  2. Circuito Eletrônico e Funcionalidades Básicas - 31 de outubro de 2024 ✅
O circuito eletrônico foi projetado e integrado ao microcontrolador **ATMega328(p)(PB)**. Todas as funcionalidades básicas, incluindo controle dos motores, operação da "arma laser" e detecção do alvo foram implementadas, mas houveram alguns problemas que impediram a apresentação ao professor.

### 3. Apresentação Final - 23 de novembro de 2024 ⚠️


## Ferramentas e Requisitos Adicionais
- O código fonte foi desenvolvido exclusivamente, sem o uso de bibliotecas prontas, exceto aquelas previamente aprovadas pelo professor.
- A documentação do software foi elaborada utilizando a ferramenta **Doxygen**.
- O repositório contém todos os arquivos do projeto, incluindo esquemas eletrônicos, código-fonte e documentação detalhada.

## Avaliação
O projeto foi avaliado com base nos seguintes critérios:
- Conformidade com os requisitos técnicos e funcionais.
- Qualidade e organização do código-fonte.
- Apresentação e documentação do projeto.
