#include <Arduino.h>
#include <aes.h>
#include <LowPower.h>
uint_8t AppSKey[N_BLOCK] = {0x30, 0x30, 0x30, 0x30, 0x37, 0x30, 0x31, 0x35, 0x30, 0x30, 0x30, 0x30, 0x37, 0x30, 0x31, 0x35};
// PUT YOUR PAYLOAD BYTES HERE
uint_8t payloadText[] = {0x30, 0x30, 0x30, 0x30, 0x37, 0x30, 0x31, 0x35, 0x30, 0x30, 0x30, 0x30, 0x37, 0x30, 0x31, 0x35};
uint_8t AESIterationPlaintextBlock[N_BLOCK];
uint_8t AESIterationCiphertextBlock[N_BLOCK];
uint_8t AESIterationPlaintextBlockConst1[5] = {0x01, 0x00, 0x00, 0x00, 0x00};
uint_8t AESIterationPlaintextBlockConst2 = 0x00;
uint_8t direction = 0x00; // let direction flag be 0x00 -> uplink
uint_8t deviceAdress[4] = {0x30, 0x31, 0x32, 0x33};
uint_8t frameCount[4] = {0x00, 0x00, 0x00, 0x01}; // number of communication frames in current direction, let it be 1
length_type AESKeyLength = 128;
aes_context AESContext;
uint_8t zeroPaddingByte = 0x00;
boolean encryptionFinished = false;
void payloadEncryption()
{
  // set AES key and initialize context
  aes_set_key(AppSKey, AESKeyLength, &AESContext);
  // block size = 16bytes -> number of blocks -> ceil((payload size in bytes) / 16)
  int payloadTextBytes = sizeof(payloadText);
  int blockNumbers = (payloadTextBytes + 16 - 1) / 16; // https://stackoverflow.com/questions/2745074/fast-ceiling-of-an-integer-division-in-c-c
  uint_8t payload[blockNumbers * N_BLOCK];
  memcpy(payload, payloadText, payloadTextBytes);
  // add zero padding if necessary
  if (payloadTextBytes % N_BLOCK != 0)
  {
    int paddingBytesNeeded = N_BLOCK - payloadTextBytes % N_BLOCK;
    for (int i = 0; i < paddingBytesNeeded; i++)
    {
      memcpy(payload + payloadTextBytes + i, &zeroPaddingByte, 1);
    }
  }
  uint_8t encryptedPayload[N_BLOCK * blockNumbers];
  for (int i = 1; i <= blockNumbers; i++)
  {
    uint_8t byteCastedi = i;
    // AESIterationPlaintextBlock = AESIterationPlaintextBlockConst1 || direction || deviceAdress || frameCount || AESIterationPlaintextBlockConst2 || byteCastedi = 16 bytes
    memcpy(AESIterationPlaintextBlock, AESIterationPlaintextBlockConst1, 5);
    memcpy(AESIterationPlaintextBlock + 5, &direction, 1);
    memcpy(AESIterationPlaintextBlock + 6, deviceAdress, 4);
    memcpy(AESIterationPlaintextBlock + 10, frameCount, 4);
    memcpy(AESIterationPlaintextBlock + 14, &AESIterationPlaintextBlockConst2, 1);
    memcpy(AESIterationPlaintextBlock + 15, &byteCastedi, 1);
    digitalWrite(5, HIGH);
    aes_encrypt(AESIterationPlaintextBlock, AESIterationCiphertextBlock, &AESContext);
    digitalWrite(5, LOW);
    // XOR current payload block and AESIterationCiphertextBlock
    uint_8t currentPayloadBlock[N_BLOCK];
    memcpy(currentPayloadBlock, payload + (i - 1) * N_BLOCK, N_BLOCK);
    xor_block(currentPayloadBlock, AESIterationCiphertextBlock);
    // copy to encryptedPayload block
    memcpy(encryptedPayload + (i - 1) * N_BLOCK, currentPayloadBlock, N_BLOCK);
  }
}
void setup()
{
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop()
{
  payloadEncryption();
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}