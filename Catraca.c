#include <Catraca.h>

#define Ratchet  PIN_C0

BYTE CountDigit = 0;
BYTE ScreenStatus = 0;

BYTE str2byte(char *str) {
    BYTE num = 0;
    for (BYTE c = 0; c < 3; c++) {
        num *= 10;
        num += str[c] - '0';
    }return num;
}

char PassKeys[PASS_LENGTH + 1];

void SetMenu (BYTE Status){
   CountDigit = 0;
   ScreenStatus = Status;

   if (Status == 0){ // MENU INICIAL                                                                                                Principal
      printf(lcd_escreve, "\fEstabelecimento\nSenha: ");
   }else if (Status == 1){ // ADM VERIFICAÇÃO
         printf(lcd_escreve, "\fEntrar como ADM?\n*-Entrar");
   }else if (Status == 2){ // MENU ADM
      printf(lcd_escreve, "\f1-Add  3-Ver\n"); 
      printf(lcd_escreve, "2-Del  4-Enviar");
   }else if (Status == 3){ // ADM Adiociona
      printf(lcd_escreve, "\fNumero de Usuário\nID: "); //Cadastro Usuario
   }else if (Status == 4){ // ADM Deleta
      printf(lcd_escreve, "\fDeletar Usuário\nID: "); //Deletar Usuario
   }else if (Status == 5){ // ADM Ver
      printf(lcd_escreve, "\fVer o Usuário\n1- ID    2- Pass"); //Visualizar Usuario
   }else if (Status == 6){ // ADM Enviar
      printf(lcd_escreve, "\fEnviando\nNão desligue "); // Implementar o banco de Dados aqui
   }else if (Status == 7){ // ADM Ver o ID
      printf(lcd_escreve, "\fVisualizar ID do Usuário\nID: "); 
   }else if (Status == 8){ // ADM Ver a senha
      printf(lcd_escreve, "\fVisualizar Senha do Usuário\nPass: "); 
   }else if (Status == 9){ // Senha do ADM
      printf(lcd_escreve, "\fSenha do ADM\n ");
   }else if (Status == 10){ // ADM adiciona a senha
      printf(lcd_escreve, "\fRegristar Usuário\nPass: ");
   }
}


void AdmMenu (char Key){
   if (Key == '1'){       //Add
      SetMenu(3);
   }else if (Key == '2'){ // Del
      SetMenu(4);
   }else if (Key == '3'){ // Ver
      SetMenu(5);
   }else if (Key == '4'){ // Enviar
      SetMenu(6);
   }else if (Key == '#'){ // Retornar a tela principal
      SetMenu(0);
   }else{ // Mensagem de erro e retornar a espera para presionar a tecla
      printf(lcd_escreve, "\fInvalido!");
      delay_ms(1500);
      SetMenu(2);
   }
}


BOOLEAN KeyPassword (char Key, BYTE Status) {
   if (Key == '#'){
      SetMenu(Status);
   } else if (CountDigit <= PASS_LENGTH){
      lcd_escreve(key);
      if(CountDigit == PASS_LENGTH){
         delay_ms(500);
         return TRUE;
      }
   }
   return FALSE;
}


void Password (char Key){
   if (KeyPassword(Key,ScreenStatus)){
      search_user(PassKeys);
      if (user.mem_address == USER_NOT_FOUND){
         printf(lcd_escreve, "\fSennha\nIncorreta");
         delay_ms(2000);
      }else {
         get_user();
         if (user.status) {
            printf(lcd_escreve, "\fBem Vindo!\nID %03d",user.ID);
            output_high(Ratchet);
         } else {
            printf(lcd_escreve, "\fProcurar o ADM\nID %03d",user.ID);
         } 
         delay_ms(5000);
         output_low(Ratchet);
      }
   SetMenu(0);
   }
}


void KeysProcess_MainMenu (char Key){ // ScreenStatus == 0
   PassKeys[CountDigit++] = Key;
   if (Key == '#'){
      SetMenu(0);
      return;
   }else if (Key == '*'){
      SetMenu(1);
      return;
   }else {
      Password(Key);
   }
}


void KeysProcess_CheckADMMode (char Key){ // ScreenStatus == 1
   PassKeys[CountDigit++] = Key;
   if(Key == '*'){
      SetMenu(9);
      CountDigit = 0;
   }else{
      SetMenu(0);
   }
}


void KeysProcess_ADMMenu (char Key){ // ScreenStatus == 2
   PassKeys[CountDigit++] = Key;
   AdmMenu(Key);
}


void KeysProcess_ADMAddID (char Key){ // ScreenStatus == 3
   PassKeys[CountDigit++] = Key;
   lcd_escreve(Key);
   if(Key == '#'){
      SetMenu(2);
   }else if(Key == '*'){
      SetMenu(3);
   }else if(CountDigit == ID_LENGTH){
      user.ID = str2byte(PassKeys);
      search_user(user.ID);
      if (user.mem_address == USER_NOT_FOUND){
         delay_ms(500);
         SetMenu(10);
      }else{
         delay_ms(500);
         printf(lcd_escreve, "\fID ou Senha\nExistente"); //Senha ja existe
         delay_ms(2000);
         SetMenu(3);
      }
   }   
}


void KeysProcess_ADMDelet (char Key){ // ScreenStatus == 4
   PassKeys[CountDigit++] = Key;
   lcd_escreve(key);
   if(Key == '#'){
      SetMenu(2);
   }else{
      if (CountDigit == ID_LENGTH){
      user.ID = str2byte(PassKeys);
      if (del_user(user.ID)){
         printf(lcd_escreve, "\fExclusão feita\ncom sucesso"); //exclusão realizada com sucesso
         delay_ms(2000);                                           
         SetMenu(4);
      }else{
         printf(lcd_escreve, "\fUsuário não\nEncontrado"); 
         delay_ms(2000);
         SetMenu(4);
         }
      }
   }
}


void KeysProcess_ADMView (char Key){ // ScreenStatus == 5
   PassKeys[CountDigit++] = Key;
   if(Key == '#'){
      SetMenu(2);
   }else{
      if (Key == '1'){
         SetMenu(7);
      }else if (Key == '2'){
         SetMenu(8);
      }else{
         printf(lcd_escreve, "\fInvalido!"); 
         SetMenu(5);
      }
   }
}


void KeysProcess_ADMUpload (){ // ScreenStatus == 6
   delay_ms(1000);
   SetMenu(2);
}


void KeysProcess_ADMViewID (char Key){ // ScreenStatus == 7
   PassKeys[CountDigit++] = Key;
   lcd_escreve(Key);
   if(Key == '#'){
      SetMenu(2);
   }else if(Key == '*'){
      SetMenu(7);
   }else{
      if (CountDigit == ID_LENGTH){
         user.ID = str2byte(PassKeys);
         search_user(user.ID);
         if (user.mem_address == USER_NOT_FOUND){
            printf(lcd_escreve,"\fUsuário não\nEncontrado"); 
            delay_ms(2000);
            SetMenu(5);
         }else{
            get_user();
            printf(lcd_escreve, "\fID %03d",user.ID);
            printf(lcd_escreve, " Status %u",user.status);
            printf(lcd_escreve, "\nPass %s",user.pass);
            delay_ms(5000);
            SetMenu(5);
         }
      }

   }
}


void KeysProcess_ADMViewPass (char Key){ // ScreenStatus == 8
   PassKeys[CountDigit++] = Key;
   lcd_escreve(key);
   if(Key == '#'){
      SetMenu(2);
   }else if(Key == '*'){
      SetMenu(7);
   }else{
      if (CountDigit == PASS_LENGTH){
         user.pass = PassKeys;
         search_user(user.pass);
         if (user.mem_address == USER_NOT_FOUND){
            printf(lcd_escreve,"\fUsuário não\nEncontrado"); 
            delay_ms(2000);
            SetMenu(5);
         }else{
            get_user();
            printf(lcd_escreve, "\fID %03d",user.ID);
            printf(lcd_escreve, " Status %u",user.status);
            printf(lcd_escreve, "\nPass %s",user.pass);
            delay_ms(5000);
            SetMenu(5);
         }
      }
   }
   //ATUALIZAR ISSO 
}


void KeysProcess_KeyADMcheck (char Key){ // ScreenStatus == 9
   PassKeys[CountDigit++] = Key;
   lcd_escreve("X");
   if(Key == '#'){
      SetMenu(0);
   }else if (CountDigit == PASS_LENGTH){
      if(verify_adm_pass(PassKeys)){
         SetMenu(2);
      }else{
         printf(lcd_escreve, "\fSenha do ADM\nestá incorreta");
         delay_ms(2000);
         SetMenu(0);
      }
   }
}


void KeysProcess_ADMAddPass (char Key){ // ScreenStatus == 10
   PassKeys[CountDigit++] = Key;
   lcd_escreve(Key);
   if(Key == '#'){
      SetMenu(3);
   }else if(CountDigit == PASS_LENGTH){
      user.status = TRUE;
      user.pass = PassKeys;
      add_user();
      delay_ms(500);
      printf(lcd_escreve, "\fUsuário foi\nAdicionado"); // usuário adicionado com sucesso
      delay_ms(1000);
      SetMenu(2);
   }
}



void main(){
   init_registro_eeprom();
   

   lcd_ini();
   
   kbd_init();
   port_b_pullups(TRUE);
   set_tris_b(0xF0);

   SetMenu(0);

   //clear_mem();
   set_adm_pass((char*) "1506");


   while(TRUE){
      char Key = returnTecla();

      if (Key) {
         if (ScreenStatus == 0){
            KeysProcess_MainMenu(Key);
         }else if (ScreenStatus == 1){
            KeysProcess_CheckADMMode(Key);
         }else if (ScreenStatus == 2){
            KeysProcess_ADMMenu(Key);
         }else if (ScreenStatus == 3){
            KeysProcess_ADMAddID(Key);
         }else if (ScreenStatus == 4){
            KeysProcess_ADMDelet(Key);
         }else if (ScreenStatus == 5){
            KeysProcess_ADMView(Key);
         }else if (ScreenStatus == 6){
            KeysProcess_ADMUpload();
         }else if (ScreenStatus == 7){
            KeysProcess_ADMViewID(Key);
         }else if (ScreenStatus == 8){
            KeysProcess_ADMViewPass(Key);
         }else if (ScreenStatus == 9){
            KeysProcess_KeyADMcheck(Key);
         }else if (ScreenStatus == 10){
            KeysProcess_ADMAddPass(Key);
         }
      }
      delay_ms(1);
   }
   
}

