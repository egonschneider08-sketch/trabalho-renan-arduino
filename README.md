Controle Inteligente de Caixa D'água (SmartTank Controller)

Este projeto é um sistema automatizado baseado em Arduino desenvolvido para monitorar o nível de uma caixa d'água, calcular o consumo de fluido e controlar automaticamente o acionamento de uma bomba e de uma válvula de passagem. O projeto conta com salvamento de dados na memória permanente (EEPROM) e interface visual completa.

Componentes Usados

Para montar este projeto fisicamente, você precisará dos seguintes hardwares:

1x Placa Arduino (Uno, Nano ou Mega).
1x Potenciômetro de 10kΩ (Utilizado para simular a variação do sensor de nível da água).
1x Display LCD 16x2 com Módulo I2C (Facilita a ligação usando apenas 4 fios).
1x Micro Servo Motor (Ex: SG90, atuando como a válvula de passagem de água).
1x Motor CC (Pode ser simulada por um LED no seu protótipo caso se interrese pelo nosso projeto).
1x Transistor (TIP-120) (Necessário para acionar o Motor CC de forma segura).
5x LEDs (Para a barra gráfica indicadora de nível).
5x Resistores de 220Ω ou 330Ω (Para proteger os LEDs).
Protoboard e Jumpers (Para as conexões).

🔌 Esquema de Ligações (Pinout)

| Componente | Pino do Arduino | Função no Sistema |
| Potenciômetro | `A0` | Simula a leitura do nível da água (0 a 1023). |
| Motor CC | `D9` | Aciona a bomba de enchimento. |
| Servo Motor | `D10` | Abre ou fecha a válvula de passagem. |
| LED 1 | `D2` | Acende quando o nível atinge 10%. |
| LED 2 | `D3` | Acende quando o nível atinge 30%. |
| LED 3 | `D4` | Acende quando o nível atinge 50%. |
| LED 4 | `D5` | Acende quando o nível atinge 70%. |
| LED 5 | `D6` | Acende quando o nível atinge 90%. |
| Display LCD (SDA)| `A4` (ou pino SDA padrão) | Via de dados do barramento I2C. |
| Display LCD (SCL)| `A5` (ou pino SCL padrão) | Via de clock do barramento I2C. |

⚙️ Como o Código Funciona

O código foi construído utilizando lógicas de não-bloqueio (usando `millis()` em vez de `delay()`) para garantir que o sistema responda rapidamente enquanto atualiza telas e salva dados. 

Aqui está o passo a passo da lógica do sistema:

1. Leitura do Nível de Água:** O sistema lê continuamente o valor do pino `A0` (onde está o potenciômetro). A função `map()` converte a leitura crua (que vai de 0 a 1023) em uma porcentagem fácil de entender (0 a 100%).
2. Automação da Bomba e Válvula:** O controle funciona com uma lógica de limites. Se o nível da água cair para 10% ou menos, o sistema liga o pino `D9` (bomba) e move o servo motor para 90 graus (simulando a abertura de uma válvula). Quando o nível atinge 90% ou mais, a bomba é desligada e o servo volta para 0 graus (válvula fechada).
3. Indicadores Visuais (LEDs): Os 5 LEDs funcionam como uma barra de progresso. O Arduino verifica o nível atual da água e envia um sinal `HIGH` apenas para os LEDs cujos níveis já foram atingidos (10%, 30%, 50%, 70% e 90%).
4. Cálculo de Consumo:** O algoritmo compara o nível atual com o nível medido na rodada anterior. Se o nível atual for maior (indicando que a caixa encheu ou que houve fluxo de água), ele calcula a diferença, multiplica por um fator (neste caso, 100) e soma isso à variável de consumo total.
5. Interface do Display LCD: Usando a função `millis()`, o código cria um temporizador que alterna o conteúdo da tela do LCD a cada 2 segundos:
   Tela 0: Exibe o Nível atual em porcentagem e o status da Bomba ("Ligado" ou "Desligado").
   Tela 1: Exibe o Consumo Total acumulado em Litros.
6. Memória Permanente (EEPROM): Para evitar que o consumo total zere caso falte energia, o sistema salva o valor na memória EEPROM. Para não desgastar a memória do Arduino (que tem um limite de gravações), o salvamento só ocorre a cada 5 segundos e apenas se o valor de consumo tiver sido alterado. Ao religar o Arduino, a função `setup()` lê esse endereço da memória e recupera o último valor salvo.

Dependências

Para que o código compile corretamente na IDE do Arduino, instale a seguinte biblioteca através do Gerenciador de Bibliotecas:

`LiquidCrystal_I2C` (Desenvolvida por Frank de Brabander).

> Nota: As bibliotecas `Wire.h`, `Servo.h` e `EEPROM.h` já são nativas da IDE do Arduino e não precisam de instalação extra.
