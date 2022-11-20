#include <Arduino.h>
#include <aes.h>

uint_8t AES_key[16] = {'0', '0', '0', '0', '7', '0', '1', '5', '0', '0', '0', '0', '7', '0', '1', '5'};
uint_8t plaintext[16] = {'0', '0', '0', '0', '7', '0', '1', '5', '0', '0', '0', '0', '7', '0', '1', '5'};
uint_8t ciphertext[16];
length_type key_length = 128;
aes_context AES_ctx;
void AES_encrypt_block()
{
  Serial.println("Start");
  for (int i = 0; i < 16; i++)
  {
    Serial.println(AES_key[i], 10);
    // Serial.flush();
  }
  Serial.println("Stop");
  // Serial.flush();
  aes_set_key(AES_key, key_length, &AES_ctx);
  Serial.println("Stop2");
  // Serial.flush();
  unsigned long ms1 = micros();
  aes_encrypt(plaintext, ciphertext, &AES_ctx);
  unsigned long ms2 = micros();
  Serial.println(ms1);
  Serial.println(ms2);
  Serial.print("Encryption took: ");
  Serial.println(ms2 - ms1);
  Serial.println("\n============================================================\n");
  Serial.print("\n\nPLAIN :");
  for (int i = 0; i < 16; i++)
  {
    Serial.print(plaintext[i], 10);
    Serial.print(" ");
  }
  Serial.print("\nCIPHER:");
  for (int i = 0; i < 16; i++)
  {
    Serial.print(ciphertext[i], 10);
    Serial.print(" ");
  }
  Serial.println("\n============================================================\n");
}
int AES_finished = 1;
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Setup finished");
  Serial.println("\n============================================================\n");
}

void loop()
{
  delay(1000);
  if (AES_finished == 1)
  {
    AES_finished = 0;
    AES_encrypt_block();
  }
}