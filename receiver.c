RECEIVER 
 
#include <SPI.h> 
#include <RF24.h> 
#include <AESLib.h> 
 
#define CE_PIN 7  // Adjust according to RF module pins 
#define CSN_PIN 8 
 
RF24 radio(CE_PIN, CSN_PIN);  // Create an RF24 object 
 
const byte address[6] = "00001";  // Address for RF communication 
 
AESLib aesLib;  // AES library instance 
 
String polybiusTable[5][5] = { 
  {"A", "B", "C", "D", "E"}, 
  {"F", "G", "H", "I", "K"}, 
  {"L", "M", "N", "O", "P"}, 
  {"Q", "R", "S", "T", "U"}, 
  {"V", "W", "X", "Y", "Z"} 
}; 
 
// AES Decryption Key (128-bit key for AES-128) 
byte aes_key[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x3f, 
0x67, 0x1c, 0x77, 0xe5, 0x19}; 
 
// AES Initialization Vector (for CBC mode) 
byte aes_iv[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 
0x0B, 0x0C, 0x0D, 0x0E, 0x0F}; 
 
String polybiusDecode(String encodedMessage) { 
  String decoded = ""; 
  for (int i = 0; i < encodedMessage.length(); i += 2) { 
    int row = encodedMessage[i] - '1'; 
    int col = encodedMessage[i + 1] - '1'; 
    decoded += polybiusTable[row][col]; 
  } 
  return decoded; 
} 
 
 
 
27  
void setup() { 
  Serial.begin(9600); 
  radio.begin(); 
  radio.openReadingPipe(0, address); 
  radio.setPALevel(RF24_PA_MIN);  // Set power level 
  radio.startListening();  // Set to receiver mode 
} 
 
void loop() { 
  if (radio.available()) { 
    byte encrypted_data[16]; 
    radio.read(&encrypted_data, sizeof(encrypted_data));  // Receive encrypted message 
     
    // Buffer for decrypted data 
    byte decrypted_data[16]; 
 
    // Decrypt the message using AES (128-bit key, CBC mode) 
    aesLib.decrypt(encrypted_data, sizeof(encrypted_data), decrypted_data, aes_key, 128, 
aes_iv); 
 
    // Decode the decrypted message 
    String decodedMessage = polybiusDecode(String((char*)decrypted_data)); 
    Serial.println("Decoded Message: " + decodedMessage); 
  } 
  delay(1000);  // Wait before checking again 
}
