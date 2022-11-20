#include <Arduino.h>
#include <aes.h>

uint_8t AppSKey[N_BLOCK] = {'0', '0', '0', '0', '7', '0', '1', '5', '0', '0', '0', '0', '7', '0', '1', '5'};
uint_8t payload[N_BLOCK] = {'0', '0', '0', '0', '7', '0', '1', '5', '0', '0', '0', '0', '7', '0', '1', '5'};
uint_8t AESIterationPlaintextBlock[N_BLOCK];
uint_8t AESIterationCiphertextBlock[N_BLOCK];
uint_8t AESIterationPlaintextBlockConst1[5] = {0x01, 0x00, 0x00, 0x00, 0x00};
uint_8t AESIterationPlaintextBlockConst2 = 0x00;
uint_8t direction = 0x00; // let direction flag be 0x00 -> uplink
uint8_t deviceAdress[4] = {'0', '1', '2', '3'};
uint8_t frameCount[4] = {0x00, 0x00, 0x00, 0x01}; // number of communication frames in current direction, let it be 1
length_type AESKeyLength = 128;
aes_context AESContext;
boolean encryptionFinished = false;
void payloadEncryption()
{
  unsigned long ms1 = micros();
  // assume no padding needed for payload
  //  set AES key and initialize context
  aes_set_key(AppSKey, AESKeyLength, &AESContext);
  // block size = 16bytes -> number of blocks -> ceil((payload size in bytes) / 16)
  int payloadBytes = sizeof(payload);
  int blockNumbers = (payloadBytes + 16 - 1) / 16; // https://stackoverflow.com/questions/2745074/fast-ceiling-of-an-integer-division-in-c-c
  Serial.println(payloadBytes);
  Serial.println(blockNumbers);
  uint_8t encryptedPayload[N_BLOCK * blockNumbers];
  for (int i = 1; i <= blockNumbers; i++)
  {
    uint_8t byteCastedi = i;
    // AESIterationPlaintextBlock = AESIterationPlaintextBlockConst1 || direction || deviceAdress || frameCount || AESIterationPlaintextBlockConst2 || byteCastedi = 16 bytes
    memcpy(AESIterationPlaintextBlock, AESIterationPlaintextBlockConst1, 5);
    memcpy(AESIterationPlaintextBlock + 5, direction, 1);
    memcpy(AESIterationPlaintextBlock + 6, deviceAdress, 4);
    memcpy(AESIterationPlaintextBlock + 10, frameCount, 4);
    memcpy(AESIterationPlaintextBlock + 14, AESIterationPlaintextBlockConst2, 1);
    memcpy(AESIterationPlaintextBlock + 15, byteCastedi, 1);
    aes_encrypt(AESIterationPlaintextBlock, AESIterationCiphertextBlock, &AESContext);
    // XOR current payload block and AESIterationCiphertextBlock
    uint_8t currentPayloadBlock[N_BLOCK];
    memcpy(currentPayloadBlock, payload + (i - 1) * N_BLOCK, N_BLOCK);
    xor_block(currentPayloadBlock, AESIterationCiphertextBlock);
    // copy to encryptedPayload block
    memcpy(encryptedPayload + (i - 1) * N_BLOCK, currentPayloadBlock, N_BLOCK);
  }
  Serial.print("Encrypted payload:");
  for (int i = 0; i < 16; i++)
  {
    Serial.print(encryptedPayload[i], 10);
    Serial.print(" ");
  }
  Serial.println("\n============================================================\n");
  unsigned long ms2 = micros();
  Serial.println(ms1);
  Serial.println(ms2);
  Serial.print("Encryption took: ");
  Serial.println(ms2 - ms1);
}
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
  if (!encryptionFinished)
  {
    payloadEncryption();
    encryptionFinished = true;
  }
}