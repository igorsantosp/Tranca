#include <EEPROM.h>
#define MAX  4 //Definiçao do tamanho max da senha
char recebido[MAX]; //Array que armazena a senha recebida
int ind = 0; //indice do array
boolean superUser = false; //Modo super usuário sendo inicializado como falso
byte entrada1 = 2; //Definindo a porta que trabalhará como a input da ponte h
byte entrada2 = 6; //Definindo a segunda porta que trabalhará como a input da ponte h
byte enable = 4; //Definindo a porta que habilita a ponte h (enable)
char senha[MAX]; //Array que armazena a senha gravada na EEPROM

//Método responsável por parar o motor
void paraMotor()
{
  digitalWrite(entrada1, LOW);
  digitalWrite(entrada2, LOW);
  delay(500);
}

//Método responsável por abrir a tranca
void abreTranca()
{
  //Serial.println("Abrindo tranca");
  digitalWrite(entrada1, LOW);
  digitalWrite(entrada2, HIGH);
  delay(490);
}

//Método responsável por fechar a tranca
void fechaTranca()
{
  digitalWrite(entrada1, HIGH);
  digitalWrite(entrada2, LOW);
  delay(490);
}


//Método responsável por gravar a senha
void gravaSenha(char sen[MAX])
{
  for (int i = 0; i < MAX; i++) {
    EEPROM.write(i + MAX, sen[i]);
  }
}


//Método responsável por comparar a senha recebida com a senha armazenada na EEPROM
boolean comparaSenha(char senha[MAX]) {
  char b[MAX];
  for (int i = 0; i < MAX; i++) {
    EEPROM.get(i + MAX, b[i]);
  }
  return strcmp(senha, b) == 0;
}


//Método responsável por comparar a senha recebida com a senha do super usuário armazenada na EEPROM
boolean comparaSuperSenha(char superSenha[MAX]) {
  char c[MAX];
  for (int i = 0; i < MAX; i++) {
    EEPROM.get(i, c[i]);
  }
  return strcmp(c, superSenha) == 0;
}

//Método setup, o serial é inicializado e a ponte h é ativada.
void setup() {
  Serial.begin(9600);
  digitalWrite(enable, HIGH);
}

void loop() {

  while (!Serial.available()) {
    //loop vazio que espera o buffer da serial ficar pronto
  }

  char a = Serial.read();
  //Se o caracter lido não for fim de senha, não for menor que o número máximo de digitos de senha e for diferente do caracter de fechar a porta, lê o próximo caractere
  if (a != 'p' && ind < MAX && a != 'c') {
    recebido[ind] = a;
    ind++;
  }
  else {
    ind = 0;
    //Checa se o caracter recebido é caracter de fim de senha
    if (a == 'p') {
      //Se o modo super usuário estiver ativo, grava a senha recebida, envia o caracter de senha gravada, sai do modo super usuário e envia um caracter que indica essa saída
      if (superUser) {
        gravaSenha(recebido);
        Serial.println("g");
        superUser = false;
        Serial.println("n");
      }
      else {
        //Compara o array de caracteres recebidos com a senha super usuário, se verdadeiro, entra em modo super usuário
        if (comparaSuperSenha(recebido)) {
          superUser = true;
          Serial.println("s");
        }
        else {
          //Compara o array de caracteres recebidos com a senha comum, se verdadeiro, abre a porta.
          if (comparaSenha(recebido))
          {
            abreTranca();
            Serial.println("a");
            paraMotor();
          }
          else {
            //Se a senha recebida não for de usuário ou super usuário, envia o caracter de erro
            Serial.println("e");
          }
        }
      }

    }
    else {
      //Se as condições acima não forem supridas, testa de o caracter de fechar a tranca foi recebido, se sim, fecha a tranca, para o motor e reseta o índice do array de caracteres recebidos
      if (a == 'c') {
        fechaTranca();
        Serial.println("f");
        paraMotor();
        ind = 0;
      }
    }
  }


}

