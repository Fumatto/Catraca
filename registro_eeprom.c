#include <2404.c> // driver EEPROM

BYTE qtd_registros;
#define REG_LENGTH  6
#define PASS_LENGTH 4
#define ID_LENGTH   3

#define ADDRESS_LAST_ID     (qtd_registros - 1) * REG_LENGTH
#define USER_NOT_FOUND      1

#define get_ID()  read_ext_eeprom(user.mem_address)

struct {
   EEPROM_ADDRESS mem_address;
   BYTE ID;
   BOOLEAN status;
   char *pass;
} user;

// Inicia a EEPROM e conta os registros salvos
void init_registro_eeprom() {
   init_ext_eeprom();

                  // Ate encontrar ID vazio
   for (user.mem_address = 0; get_ID() != 0xFF; user.mem_address += REG_LENGTH);
   qtd_registros = user.mem_address / REG_LENGTH;
}

void clear_mem() {
   for (EEPROM_ADDRESS i = 0; i < EEPROM_SIZE - PASS_LENGTH; i++) {
      write_ext_eeprom(i, 0xFF);
   }

   qtd_registros = 0;
}


void set_adm_pass(char pass[]) {
   for (BYTE i = 0; i < PASS_LENGTH; i++) {
      write_ext_eeprom(EEPROM_SIZE - 1 - i, pass[i]);
   }
}

BOOLEAN verify_adm_pass(char pass[]) {
   for (BYTE i = 0; i < PASS_LENGTH; i++) {
      if (read_ext_eeprom(EEPROM_SIZE - 1 - i) != pass[i]) {
         return FALSE;
      }
   }

   return TRUE;
}

#define get_status()  read_ext_eeprom(user.mem_address + 1)

char *get_pass() {
   static char pass[PASS_LENGTH + 1];

   for (BYTE i = 0; i < PASS_LENGTH; i++) {
      pass[i] = read_ext_eeprom(user.mem_address + 2 + i);
   }

   return pass;
}

void get_user() {
   user.ID = get_ID();
   user.status = get_status();
   user.pass = get_pass();
}

void search_user(BYTE ID) {
   for (user.mem_address = 0; user.mem_address / REG_LENGTH < qtd_registros; user.mem_address += REG_LENGTH) {
      if (get_ID() == ID) {
         return;
      }
   }

   user.mem_address = USER_NOT_FOUND;
}

void search_user(char pass[]) {
   BOOLEAN founded;

   for (BYTE pos_reg = 0; pos_reg < qtd_registros; pos_reg++) {
      user.mem_address = pos_reg * REG_LENGTH;
      founded = TRUE;

      for (BYTE i = 0; i < PASS_LENGTH; i++) {
         if (read_ext_eeprom(user.mem_address + 2 + i) != pass[i]) {
               founded = FALSE;
               break;
         }
      }

      if (founded) {
         return;
      }
   }

   user.mem_address = USER_NOT_FOUND;
}

void write_user() {
   write_ext_eeprom(user.mem_address, user.ID);
   write_ext_eeprom(user.mem_address + 1, user.status);

   for (BYTE i = 0; i < PASS_LENGTH; i++) {
      write_ext_eeprom(user.mem_address + 2 + i, user.pass[i]);
   }
}

#define mem_full() \
   (qtd_registros + 1) * REG_LENGTH >= EEPROM_SIZE - PASS_LENGTH


void add_user() {
   qtd_registros++;
   user.mem_address = ADDRESS_LAST_ID;
   write_user();
}

void clear_last_registro() {
   for (BYTE i = 0; i < REG_LENGTH; i++) {
      write_ext_eeprom(ADDRESS_LAST_ID + i, 0xFF);
   }

   qtd_registros--;
}

// Retorna FALSE caso o ID nao seja encontrado
BOOLEAN del_user(BYTE ID) {
   search_user(ID);
   if (user.mem_address == USER_NOT_FOUND) return FALSE;
   
   EEPROM_ADDRESS address_user = user.mem_address;
   user.mem_address = ADDRESS_LAST_ID;
   get_user();
   user.mem_address = address_user;
   write_user();
   clear_last_registro();

   return TRUE;
}
