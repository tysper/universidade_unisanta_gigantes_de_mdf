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

Diagrama feito no autocad:

<img src="https://i.ibb.co/gz2DRw0/Screenshot-179.png" alt="Screenshot-179" border="0" width="640">
<i>Feito por <a href="https://github.com/tysper">Pedro Nunes</a></i>

### Eletrônica
- Circuitos para controle de motores projetados manualmente (sem shields prontos).
- Laser com potência máxima de **300 mW** e receptor de luz com difusor de 20 mm.
- LEDs indicativos para "vidas", com controle de estado por GPIO e um botão para reset.

<p>Circuito da placa principal desenvolvida para abrigar o microcontrolador ATMega328</p>
<img src="https://i.ibb.co/WPhnfsy/Screenshot-180.png" alt="Screenshot-180" border="0" width="640">
<br>
<p>Circuito da placa de ponte H</p>
<img src="https://i.ibb.co/ftBYP19/Screenshot-181.png" alt="Screenshot-181" border="0"></a><br />
<i>Feito por <a href="https://github.com/gabriele-siilva">Gabriele Silva</a></i>

### Programação
- Uso obrigatório de timers para controle do laser.
- Interrupções para funcionalidade do botão de reset.
- Controle dos motores baseado em PWM.
- Reação programada ao impacto de laser: desativação temporária do robô e decremento de LEDs.

<i>Feito por <a href="https://github.com/gabriele-siilva">Gabriele Silva</a>, <a href="https://github.com/tysper">Pedro Nunes</a>, <a href="https://github.com/arthursilles">Arthur Silles</a> e <a href="https://github.com/imyourmafe">Maria Fernanda</a>.</i>

## Estrutura de Desenvolvimento

###  1. Protótipo Funcional - 5 de outubro de 2024 ✅
O protótipo inicial do robô foi concluído, com movimentação básica utilizando os motores conectados a baterias. A estrutura mecânica foi montada conforme as especificações e demonstrada com sucesso.

###  2. Circuito Eletrônico e Funcionalidades Básicas - 31 de outubro de 2024 ✅
O circuito eletrônico foi projetado e integrado ao microcontrolador **ATMega328(p)(PB)**. Todas as funcionalidades básicas, incluindo controle dos motores, operação da "arma laser" e detecção do alvo foram implementadas, mas houveram alguns problemas que impediram a apresentação ao professor.

### 3. Apresentação Final - 23 de novembro de 2024 ✅
Apresentação concluída com sucesso. O video abaixo demonstra as funcionalidades básicas do projeto:

<video src="https://drive.google.com/file/d/1mjpHybZGSYsJEZXwXcPd9ix581dAYIp9/preview" width="640" height="480" allow="autoplay"></video>


## Ferramentas e Requisitos Adicionais
- O código fonte foi desenvolvido exclusivamente, sem o uso de bibliotecas prontas, exceto aquelas previamente aprovadas pelo professor.
- A documentação do software foi elaborada utilizando a ferramenta **Doxygen**. E pode ser consultada no link: <a href="https://tysper.github.io/universidade_unisanta_gigantes_de_mdf/scripts/documentacao/html/index.html">Documentação - Gigantes de MDF</a>
- O repositório contém todos os arquivos do projeto, incluindo esquemas eletrônicos, código-fonte e documentação detalhada.

## Avaliação
O projeto foi avaliado com base nos seguintes critérios:
- Conformidade com os requisitos técnicos e funcionais.
- Qualidade e organização do código-fonte.
- Apresentação e documentação do projeto.
