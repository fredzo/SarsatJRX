#include <BluetoothManager.h>

#ifdef BLUETOOTH
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Hardware.h>

#define SERVICE_UUID        "4fcb5a3e-8e91-4a83-9d8b-7c21e0b4a2f1"
#define CHARACTERISTIC_UUID "8a1d0e4c-1b37-4c9e-9fa2-91b143ad7f22"

// Bluetooth status
bool bluetoothStatusChanged = false;
BluetoothStatus bluetoothStatus = BluetoothStatus::OFF;
int bluetoothRssi = 0;
static uint32_t lastBTStatusCheckTime = 0;

#if 1

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;

BLEService *pService = NULL;

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    // Update Bluetooth status
    bluetoothStatus = BluetoothStatus::CONNECTED;
    bluetoothStatusChanged = true;
  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    bluetoothStatus = BluetoothStatus::DISCONNECTED;
    bluetoothStatusChanged = true;
#ifdef SERIAL_OUT
    Serial.println("Disconnected from WiFi access point");
#endif
  }
};


void bluetoothManagerStart()
{
  bluetoothStatus = BluetoothStatus::DISCONNECTED;

  // Initialize BLE device
  BLEDevice::init("SarsatJRX-BLE-Server");

  // Create BLE server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create service
  pService = pServer->createService(SERVICE_UUID);

  // Create characteristic with READ and NOTIFY properties
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  // Add descriptor for enabling notifications on the client
  pCharacteristic->addDescriptor(new BLE2902());

  // Start service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x06);  // Recommended settings
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("BLE server is running, waiting for client connection...");  

  bluetoothStatusChanged = true;
}

String bluetoothManagerGetStatusString()
{
  switch(bluetoothStatus)
  {
    case BluetoothStatus::CONNECTED : 
      return "Connected";
    case BluetoothStatus::OFF:
      return "Bluetooth is off";
    case BluetoothStatus::DISCONNECTED:
    default:
      return "Disconnected";
  }
}

/**
 * Returns true if Bluetooth is connected
 */
BluetoothStatus bluetoothManagerGetStatus()
{
    return bluetoothStatus;  
}

void bluetoothManagerStop()
{
  BLEDevice::stopAdvertising();
  pService->stop();
  pServer->getAdvertising()->stop();
  pServer->removeService(pServer->getServiceByUUID(SERVICE_UUID));
  BLEDevice::deinit(true); 
  bluetoothStatus = BluetoothStatus::OFF;
  pService = NULL;
  pServer = NULL;
}

/**
 * Returns true if Bluetooth is connected
 */
bool bluetoothManagerIsConnected()
{
    return (bluetoothStatus == BluetoothStatus::CONNECTED);
}

#define BLUETOOTH_STATUS_CHECK_PERIOD    2000 // Check every 2 seconds

static int value = 0;

bool bluetoothManagerHandleClient()
{
  // Check for status change
  bool changed = bluetoothStatusChanged;
  long now = millis();
  // TODO handle reconnection attempt

  if(bluetoothStatusChanged || (now-lastBTStatusCheckTime >= BLUETOOTH_STATUS_CHECK_PERIOD))
  {
    lastBTStatusCheckTime = now;
    if(bluetoothStatusChanged)
    {
  #ifdef SERIAL_OUT
      Serial.println("Bluetooth status : " + bluetoothManagerGetStatusString());
  #endif
    }
    if (deviceConnected) 
    {
      value++;
      // Prepare value to send
      String msg = "Counter: " + String(value);
      pCharacteristic->setValue(msg.c_str());

      // Notify client
      pCharacteristic->notify();

      Serial.println("Sent: " + msg);
    }
  }
  bluetoothStatusChanged = false;
  return changed;
}

#endif
#endif
