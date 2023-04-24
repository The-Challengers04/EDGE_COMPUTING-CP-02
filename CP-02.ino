#include <LiquidCrystal.h>
#include <dht.h>
#define ledGreen 10
#define ledYellow 9
#define ledRed 8
#define buz 7
#define tempPin A0
#define umid A1
#define ldr A2


int ldrAlerta = 400; // Indice que define quando entra no nivel de alerta 
int ldrProblema = 570; // indice que define quando entra no nivel de problema

float tempMin = 10.0; // valor minimo do padrao de temperatura do sistema
float tempMax = 15.0;// valor maximo do padrao de temperatura do sistema

float umidadeMin = 60.0;// valor minimo do padrao de umidade do sistema
float umidadeMax = 80.0;// valor maximo do padrao de umidade do sistema

bool ldrProblemMed = false; // variavel para saber o status do ldr
bool ldrProblemMax = false; // variavel para saber o status do ldr
bool tempProblem = false; // variavel para saber o status do tmp
bool umidadeProblem = false; // variavel para saber o status do sensor de temperatura

float umidade = 0; // variavel para armazenar o valor do sensor de umidade
float temp = 0; // variavel para armazenar o valor do sensor de temperatura
int valueLDR = 0; // variavel para armazenar o valor do sensor de luz

LiquidCrystal lcd_1(12, 11, 5, 4, 3, 2); //Iniciando o LCD
dht DHT; // Iniciando o DHT

#define DHT11_PIN 6

void setup()
{
  pinMode(ledGreen, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(buz, OUTPUT);
  lcd_1.begin(16, 2); // Set up the number of columns and rows on the LCD.
  Serial.begin(9600); // Iniciando o Monitor Serial
  pinMode(tempPin, INPUT);// TEMP
  pinMode(umid, INPUT);// UMIDADE
  pinMode(ldr, INPUT);// LDR
  digitalWrite(ledGreen, LOW);
  digitalWrite(ledYellow, LOW);
  digitalWrite(ledRed, LOW);
}

void loop()
{
  //Reiniciando variaiveis
    valueLDR = 0;
    temp = 0;
    umidade = 0;
  // =========== LUZ =============
  //Lendo Luz
    for(int i = 0; i < 5; i++)
    {
      valueLDR += analogRead(ldr); // Armazenando o valor do LDR
      delay(333);
    }
    valueLDR = valueLDR / 5; //Pegando Media de Luz
  	//Imprimindo o valor do LDR no serial monitor
  	Serial.print("LDR: ");
  	Serial.println(valueLDR);
  
  //Validação
  	LimparLCD();
    // Esta dentro dos padroes
    if(valueLDR < ldrAlerta) 
    {
      ldrProblemMed = false;
	  ldrProblemMax = false;
      problemLevel();//Verificando o status do Sistema
      lcd_1.setCursor(0, 0);
  	  lcd_1.print("Luz OK");
    }

    //Esta em nivel de alerta
    else if(valueLDR >= ldrAlerta && valueLDR < ldrProblema)
    {
      ldrProblemMed = true;
	  ldrProblemMax = false;
      problemLevel();//Verificando o status do Sistema
      lcd_1.setCursor(0, 0);
  	  lcd_1.print("Ambiente a meia");
      lcd_1.setCursor(0, 1);
  	  lcd_1.print("luz");
    }

    //Esta em nivel de problema
    else
    {
      ldrProblemMed = false;
	  ldrProblemMax = true;
      problemLevel();//Verificando o status do Sistema
      lcd_1.setCursor(0, 0);
  	  lcd_1.print("Ambiente muito");
      lcd_1.setCursor(0, 1);
  	  lcd_1.print("claro");
    }
  
  // =========== TEMPERATURA ===============
  //Lendo Temperatura
    for(int i = 0; i < 5; i++)
    {
      int chk = DHT.read11(DHT11_PIN);
      temp += DHT.temperature;
      delay(333);
    }
    temp = temp / 5; // Pegando a media das medidas obtidas do sensor de temperatura
  	//Imprimindo o valor do TMP no serial monitor
  	Serial.print("Temperatura: ");
    Serial.print(temp); Serial.println(" C");	//Mostra no Monitor Serial a temperatura em ºC + unidade
  	
  	//Validação  
  	LimparLCD();
    // Esta dentro dos padroes
    if(temp > tempMin && temp < tempMax) 
    {
      tempProblem = false;
      problemLevel();//Verificando o status do Sistema
      lcd_1.setCursor(0, 0);
  	  lcd_1.print("Temperatura OK");
      lcd_1.setCursor(0, 1);
  	  lcd_1.print(temp);
      lcd_1.setCursor(2, 1);
      lcd_1.write(B10110010);
  	  lcd_1.print("C  ");
    }

    //Temperatura abaixo dos padrões
    else if(temp < tempMin)
    {
      tempProblem = true;
      problemLevel();//Verificando o status do Sistema
      lcd_1.setCursor(0, 0);
  	  lcd_1.print("Temp. Baixa");
      lcd_1.setCursor(0, 1);
  	  lcd_1.print(temp);
      lcd_1.setCursor(2, 1);
  	  lcd_1.write(B10110010);
  	  lcd_1.print("C  ");
    }

    //Temperatura acima dos padrões
    else
    {
      tempProblem = true;
      problemLevel();//Verificando o status do Sistema
      lcd_1.setCursor(0, 0);
  	  lcd_1.print("Temp. Alta");
      lcd_1.setCursor(0, 1);
  	  lcd_1.print(temp);
      lcd_1.write(B10110010);
  	  lcd_1.print("C  ");
    }
  
  //=========== UMIDADE =============
  //Lendo Umidade
    for(int i = 0; i < 5; i++)
    {
      int chk = DHT.read11(DHT11_PIN);
      umidade += DHT.humidity;
      delay(333);
    }
    umidade = umidade / 5; //Pegando a media dos valores obtidos do sensor de umidade
  	
  	//Imprimindo o valor do sensor de umidade no serial monitor
  	Serial.print("Umidade: ");
    Serial.print(umidade);
    Serial.println("%  ");
  	//Validação
  	LimparLCD();
    // Esta dentro dos padroes
    if(umidade > umidadeMin  && umidade < umidadeMax) 
    {
      umidadeProblem = false;
      problemLevel();//Verificando o status do Sistema
      lcd_1.setCursor(0, 0);
  	  lcd_1.print("Umidade OK");
      lcd_1.setCursor(0, 1);
  	  lcd_1.print(umidade);
      lcd_1.setCursor(2, 1);
  	  lcd_1.print("%    ");
    }

    //Umidade abaixo dos padrões
    else if(umidade < umidadeMin)
    {
      umidadeProblem = true;
      problemLevel();//Verificando o status do Sistema
      lcd_1.setCursor(0, 0);
  	  lcd_1.print("Umidade. Baixa");
      lcd_1.setCursor(0, 1);
  	  lcd_1.print(umidade);
      if(umidade < 9.99)
      {
      	lcd_1.setCursor(1, 1);
      }
      else
      {
        lcd_1.setCursor(2, 1);
      }
  	  lcd_1.print("%   ");
    }

    //Umidade acima dos padrões
    else
    {
      umidadeProblem = true;
      problemLevel();//Verificando o status do Sistema
      lcd_1.setCursor(0, 0);
  	  lcd_1.print("Umidade. Alta");
      lcd_1.setCursor(0, 1);
  	  lcd_1.print(umidade);
      lcd_1.setCursor(2, 1);
  	  lcd_1.print("%   ");
    }
}




// =======METODO LED============
void problemLevel()
{
  
  if(ldrProblemMed) // Ambiente em meia luz
  {
    digitalWrite(ledGreen, LOW); // Apaga o LED Verde
    digitalWrite(ledYellow, HIGH); // Acende o LED Amarelo
  }
  
  if(ldrProblemMax) // Ambiente claro
  {
    digitalWrite(ledGreen, LOW); // Apaga o LED Verde
    digitalWrite(ledRed, HIGH); // Acende o LED Vermelho
    tone(buz, 500);
  }
  
  
  if(tempProblem) // Temperatura fora do padrão estabelecido
  {
    digitalWrite(ledGreen, LOW); // Apaga o LED Verde
    digitalWrite(ledYellow, HIGH); // Acende o LED Amarelo
    tone(buz, 500);
  }
  
  if(umidadeProblem)// Umidade fora do padrão estabelecido
  {
    digitalWrite(ledGreen, LOW); // Apaga o LED Verde
    digitalWrite(ledRed, HIGH); // Acende o LED Vermelho
    tone(buz, 500);
  }
  
  if(!ldrProblemMed && !tempProblem) // Reset do led Amarelo
  {
    digitalWrite(ledYellow, LOW); // Apaga o LED Amarelo
  }
  
  if(!ldrProblemMax && !umidadeProblem)// Reset do led Vermelho
  {
    digitalWrite(ledRed, LOW); // Apaga o LED Vermelho
  }
  
  if(!tempProblem && !umidadeProblem && !ldrProblemMax) // Reset do Buzina
  {
    noTone(buz); // Desligar Buzina
  }
  
  if(!umidadeProblem && !tempProblem && !ldrProblemMed && !ldrProblemMax) // Sistema sem problemas
  {
    digitalWrite(ledGreen, HIGH); // Acende o LED Verde
  }
  
}

void LimparLCD() // Metodos usado para limpar a escrita do LCD para escrever um novo conteudo
{
  //Imprimindo no LCD
    lcd_1.setCursor(0, 0);
  	lcd_1.print("                ");
  	lcd_1.setCursor(0, 1);
  	lcd_1.print("                ");
}