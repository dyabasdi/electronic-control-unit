#include <SPI.h>

// Define MCP2515 SPI Commands and Registers
#define MCP2515_CS_PIN 10
#define MCP2515_RESET 0xC0
#define MCP2515_WRITE 0x02
#define MCP2515_READ 0x03
#define MCP2515_RTS_TXB0 0x81

// MCP2515 Register Definitions
#define CANCTRL 0x0F
#define CANCTRL_NORMAL 0x00
#define CANSTAT 0x0E
#define TXB0CTRL 0x30
#define TXB0SIDH 0x31
#define TXB0SIDL 0x32
#define TXB0DLC 0x35
#define TXB0DATA 0x36

// Parameters to be transmitted
typedef struct {
  // Message 1:
  uint16_t engineSpeed;      // 2 bytes
  uint8_t lambda;            // 1 byte
  uint8_t oilPress;          // 1 byte
  uint8_t manifoldPress;     // 1 byte
  uint8_t fuelPress;         // 1 byte
  uint8_t ethanolContent;    // 1 byte

  // Message 2:
  uint8_t throttlePos;       // 1 byte
  uint8_t coolantTemp;       // 1 byte
  uint8_t intakeTemp;        // 1 byte
  uint8_t batteryVoltage;    // 1 byte
  uint16_t engineRunTime;    // 2 bytes
} Parameters;

typedef struct {
  uint32_t id;
  uint8_t data[8];
} CAN_Frame;

typedef struct {
  CAN_Frame message1;
  CAN_Frame message2;
} CAN_messages;

// Global Variables
CAN_messages can_messages;
Parameters params;

// Function Prototypes
void MCP2515_reset();
void MCP2515_write_register(uint8_t reg, uint8_t value);
uint8_t MCP2515_read_register(uint8_t reg);
void MCP2515_send_message(uint32_t id, uint8_t *data, uint8_t len);

void initialize_CAN() {
  // Initialize SPI
  SPI.begin();
  pinMode(MCP2515_CS_PIN, OUTPUT);
  digitalWrite(MCP2515_CS_PIN, HIGH);

  // Reset the MCP2515
  MCP2515_reset();

  // Set MCP2515 to Normal Mode
  MCP2515_write_register(CANCTRL, CANCTRL_NORMAL);

  // Check if MCP2515 is in Normal Mode
  if ((MCP2515_read_register(CANSTAT) & 0xE0) == CANCTRL_NORMAL) {
    Serial.println("MCP2515 Initialized Successfully in Normal Mode");
  } else {
    Serial.println("MCP2515 Initialization Failed");
    while (1);
  }

  // Initialize CAN message IDs
  can_messages.message1.id = 0x01;  // ID for message 1
  can_messages.message2.id = 0x02;  // ID for message 2

  // Initialize Parameter Values
  params.engineSpeed = 3000;
  params.lambda = 98;
  params.oilPress = 75;
  params.manifoldPress = 100;
  params.fuelPress = 50;
  params.ethanolContent = 20;
  params.throttlePos = 40;
  params.coolantTemp = 85;
  params.intakeTemp = 30;
  params.batteryVoltage = 12;
  params.engineRunTime = 12345;
}

void send_CAN_messages() {
  // Populate CAN Message 1
  can_messages.message1.data[0] = can_messages.message1.id;
  can_messages.message1.data[1] = params.engineSpeed >> 8;
  can_messages.message1.data[2] = params.engineSpeed & 0xFF;
  can_messages.message1.data[3] = params.lambda;
  can_messages.message1.data[4] = params.oilPress;
  can_messages.message1.data[5] = params.manifoldPress;
  can_messages.message1.data[6] = params.fuelPress;
  can_messages.message1.data[7] = params.ethanolContent;

  // Populate CAN Message 2
  can_messages.message2.data[0] = can_messages.message2.id;
  can_messages.message2.data[1] = params.throttlePos;
  can_messages.message2.data[2] = params.coolantTemp;
  can_messages.message2.data[3] = params.intakeTemp;
  can_messages.message2.data[4] = params.batteryVoltage;
  can_messages.message2.data[5] = params.engineRunTime >> 8;
  can_messages.message2.data[6] = params.engineRunTime & 0xFF;

  // Send CAN Messages
  MCP2515_send_message(can_messages.message1.id, can_messages.message1.data, 8);
  MCP2515_send_message(can_messages.message2.id, can_messages.message2.data, 8);
}

void MCP2515_reset() {
  digitalWrite(MCP2515_CS_PIN, LOW);
  SPI.transfer(0xC0);  // Reset command
  digitalWrite(MCP2515_CS_PIN, HIGH);
}

void MCP2515_write_register(uint8_t reg, uint8_t value) {
  digitalWrite(MCP2515_CS_PIN, LOW);
  SPI.transfer(MCP2515_WRITE);
  SPI.transfer(reg);
  SPI.transfer(value);
  digitalWrite(MCP2515_CS_PIN, HIGH);
}

uint8_t MCP2515_read_register(uint8_t reg) {
  uint8_t value;
  digitalWrite(MCP2515_CS_PIN, LOW);
  SPI.transfer(MCP2515_READ);
  SPI.transfer(reg);
  value = SPI.transfer(0xFF);  // Dummy byte to clock data out
  digitalWrite(MCP2515_CS_PIN, HIGH);
  return value;
}

void MCP2515_send_message(uint32_t id, uint8_t *data, uint8_t len) {
  // Load CAN ID
  MCP2515_write_register(TXB0SIDH, (id >> 3) & 0xFF);  // High 8 bits
  MCP2515_write_register(TXB0SIDL, (id << 5) & 0xE0);  // Low 3 bits

  // Load Data Length
  MCP2515_write_register(TXB0DLC, len & 0x0F);

  // Load Data Bytes
  for (uint8_t i = 0; i < len; i++) {
    MCP2515_write_register(TXB0DATA + i, data[i]);
  }

  // Request to Send
  digitalWrite(MCP2515_CS_PIN, LOW);
  SPI.transfer(MCP2515_RTS_TXB0);
  digitalWrite(MCP2515_CS_PIN, HIGH);
}

int main() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize CAN
  initialize_CAN();

  // Send CAN messages every 100ms
  while (1) {
    send_CAN_messages();
    delay(100);
  }

  return 0;
}
