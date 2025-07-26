TRANSMITTER 
 
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
 
// AES Encryption Key (128-bit key for AES-128) 
byte aes_key[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x3f, 
0x67, 0x1c, 0x77, 0xe5, 0x19}; 
 
// AES Initialization Vector (for CBC mode) 
byte aes_iv[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 
0x0B, 0x0C, 0x0D, 0x0E, 0x0F}; 
 
String polybiusEncode(String message) { 
  String encoded = ""; 
  message.toUpperCase(); 
  message.replace('J', 'I');  // Handle 'J' by converting it to 'I' 
 
  for (int i = 0; i < message.length(); i++) { 
    char c = message[i]; 
    for (int row = 0; row < 5; row++) { 
      for (int col = 0; col < 5; col++) { 
        if (polybiusTable[row][col][0] == c) { 
          encoded += String(row + 1) + String(col + 1); 
        } 
25  
      } 
    } 
  } 
  return encoded; 
} 
 
void setup() { 
  Serial.begin(9600); 
  radio.begin(); 
  radio.openWritingPipe(address); 
  radio.setPALevel(RF24_PA_MIN);  // Set power level 
  radio.stopListening();  // Set to transmitter mode 
} 
 
void loop() { 
  String message = "HELLO";  // Sample message 
  String encodedMessage = polybiusEncode(message); 
   
  Serial.println("Encoded Message: " + encodedMessage); 
   
  // Prepare the buffer for AES encryption 
  char aes_data[16] = {0};  // Initialize a buffer for the encrypted data 
  encodedMessage.toCharArray(aes_data, 16);  // Copy the encoded message into the buffer 
   
  // Output buffer to store encrypted data 
  byte encrypted_data[16]; 
 
  // Encrypt the message using AES (128-bit key, CBC mode) 
  aesLib.encrypt((byte*)aes_data, sizeof(aes_data), encrypted_data, aes_key, 128, aes_iv); 
 
  // Send encrypted message 
  bool success = radio.write(&encrypted_data, sizeof(encrypted_data)); 
  if (success) { 
    Serial.println("Message sent successfully!"); 
  } else { 
    Serial.println("Message failed to send."); 
  } 
   
  delay(1000);  // Wait for a second before sending again 
}
