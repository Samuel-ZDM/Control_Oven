#include <LiquidCrystal.h>
#include "defineFunc.h"


float sensorValue = 0;
String buf;

int buttonState = 0; //Para desligar o sistema
int butUpState; //Controle de clique
int lastStateUp = 0; //Controle de clique
int debouceTest = 0; //Tempo de clique
bool valueOk = false; //Controle de clique
int lastStateDown = 0; //Controle de clique
int debouceDown = 0; //Controle de clique

float valueSet = 24.5; //Valor de Set point iniciado
float valueHist = 1.0; //Valor de Histerese iniciado

int stage = 0; //Controle dos estágios de seleção

/*
 * Variáveis para desligar o programa. Após clicar no botão Up
*/
unsigned long tempo_atual = 0;
int aciona = false;
long tempo_acionado = 2000;
unsigned long ultimo_tempo = 0;
/*
   Variáveis para configura de do Display lcd
*/
const int rs = 12, en = 11, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
/**********************************************************/


void setup() {

  Serial.begin(9600);
  lcd.begin(16, 2);

  pinMode(controllerOven, OUTPUT);

  /*
   * Configurando botões
  */
  pinMode(butSelec, INPUT_PULLUP);
  pinMode(butUp, INPUT_PULLUP);
  pinMode(butDw, INPUT_PULLUP);
  pinMode(butOk, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(2), buttonPressed1, RISING); //Interrupção no botão select
  
  lcd.display();

}

void loop() {

  Serial.println("Teste");

  if (buttonState == 0) //Caso o sistema não esteja desligado. Obs: só não entrará aqui se Up for apertado por 2 segs
  {
    /*
     * Leitura do sensor 
     */
    sensorValue = 0;
    for (int i = 0; i <= 49; i++) 
    {
      sensorValue += (float(analogRead(sensorPin)) * 5 / (1023)) / 0.01;
      delay(10);

    }

    sensorValue = sensorValue / 50;
    Serial.println(sensorValue);
    /************************************************************************/
    
    mensageLCD(); //Mensagem inicial LCD

    /*
     * Controle para mostrar se o sensor está ligado ou desligado
     */
    if (sensorValue <= (valueSet - valueHist))
    {
      digitalWrite(controllerOven, HIGH);
      lastStateDown = 1;
      delay(10);
    } else if (sensorValue >= ( valueSet + valueHist))
    {
      digitalWrite(controllerOven, LOW);
      lastStateDown = 0;
      delay(10);
    }
  }

  /****************************************************************************/


  /*
   * Espera o botão Up ser clicado por 2 segs ou mais para desligar o sistema
   */
  if (digitalRead(butUp) == HIGH)
  {
    Serial.println("mensage");
    tempo_atual = millis();
    while (digitalRead(butUp) == HIGH);
    ultimo_tempo = millis();

    if ( ultimo_tempo - tempo_atual >= tempo_acionado)
    {
      digitalWrite(controllerOven, LOW);
      ultimo_tempo = tempo_atual;
      aciona = false;
      buttonState = 1;
      lastStateDown = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Sistema");
      lcd.setCursor(7, 1);
      lcd.print("Desligado");

    }
  }
  /*************************************************************/
  
/*
 * Espera o botão Down ser clicado por 2 segs ou mais para ligar o sistema
*/
    if (digitalRead(butDw) == HIGH)
  {
    Serial.println("mensage");
    tempo_atual = millis();
    while (digitalRead(butDw) == HIGH);
    ultimo_tempo = millis();

    if ( ultimo_tempo - tempo_atual >= tempo_acionado)
    {
      digitalWrite(controllerOven, LOW);
      ultimo_tempo = tempo_atual;
      aciona = false;
      buttonState = 0;
      lastStateDown = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Sistema");
      lcd.setCursor(7, 1);
      lcd.print("Ligado");
      delay(500);
      lcd.clear();

    }
  }

  /**********************************************************************************/


  /*
   * Espera o botão select ser clicado para entrar em modo de configuração
   */
  while (valueOk)
  {


    lcd.setCursor(0, 0);
    lcd.print("Config Mode");
    lcd.setCursor(11, 0);
    lcd.print("  ");
    lcd.setCursor(0, 1);
    lcd.print("Up temp");
    lcd.setCursor(7, 1);
    lcd.print(" ");
    lcd.setCursor(8, 1);
    lcd.print("Dwn Hist");

    if (digitalRead(butUp) == HIGH) //Botão Up para entrar 
    {
      lcd.clear();
      confgTempe(); //Função para configurar temperatura
    }
    if (digitalRead(butDw) == HIGH)//Botão Down para entrar
    {
      lcd.clear();
      confgHist(); //Função para configurar histerese
    }
    if (digitalRead(butOk) == HIGH)//Botão Ok para confirmar alteração
    {
      valueOk = 0;
      lcd.clear();
      break;
    }
  }

  /**************************************************************************************/




}

/*
 * Interrupção do botão select
 */
void buttonPressed1()           
{
  //output = LOW;                //Change Output value to LOW
  lcd.clear();

  valueOk = 1;

}
/*********************************************************************/

/*
 * Mostrar mensagem LCD
 */
void mensageLCD()
{

  lcd.setCursor(0, 0);
  lcd.print("Atual:  ");
  lcd.print(sensorValue);
  lcd.setCursor(0, 1);
  lcd.print("St ");
  lcd.print(valueSet);
  lcd.setCursor(10, 1);
  lcd.setCursor(15, 1);
  lcd.print(" ");
  if (lastStateDown)
  {
    lcd.setCursor(13, 1);
    lcd.print("ON");
  } else {
    lcd.setCursor(13, 1);
    lcd.print("OFF");
  }



}

/*
 * Configurar a temperatura após estar em modo de configuração e botão Up selecionado
 * Up aumenta temperatura
 * Down diminui temperatura
 */

void confgTempe()
{

  Serial.println("Estou no while OK");

  stage = 1;

  lcd.clear();


  lcd.setCursor(0, 0);
  lcd.print("Mode Set OK end");
  lcd.setCursor(1, 1);
  lcd.print("Set Point: ");
  lcd.print(valueSet);

  butUpState = 0;


  while (stage == 1)
  {
    delay(300);
    if (digitalRead(butOk) == HIGH)
      stage = 0;
    if (butUpState)
    {

      delay(50);
      valueSet = valueSet + 0.5;;
      lcd.setCursor(1, 1);
      lcd.print("Set Point: ");
      lcd.print(valueSet);


    } else if (digitalRead(butDw) == HIGH)
    {
      delay(50);
      valueSet = valueSet - 0.5;;
      lcd.setCursor(1, 1);
      lcd.print("Set Point: ");
      lcd.print(valueSet);
    }
    butUpState = digitalRead(butUp);
  }


}
/************************************************************************************/


/*
 * Configurar a histerese após estar em modo de configuração e botão Down selecionado
 * Up aumenta histerese
 * Down diminui histerese
 */

void confgHist()
{

  stage = 2;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mode Set OK end");
  lcd.setCursor(1, 1);
  lcd.print("Set Hist ");
  lcd.print(valueHist);

  debouceDown = 0;

  //delay(2000);
  while (stage == 2)
  {
    delay(300);

    if (digitalRead(butOk) == HIGH)
      stage = 0;
    if (digitalRead(butUp) == HIGH)
    {

      delay(50);
      valueHist = valueHist + 0.5;;
      lcd.setCursor(1, 1);
      lcd.print("Set Hist ");
      lcd.print(valueHist);

      lastStateUp = debouceTest;

    } else if (debouceDown)
    {
      delay(50);
      valueHist = valueHist - 0.5;;
      lcd.setCursor(1, 1);
      lcd.print("Set Hist ");
      lcd.print(valueHist);

    }
    debouceDown = digitalRead(butDw);
  }
}

/************************************************************************************/
