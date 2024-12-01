#include <TFT_eSPI.h>
#include <lvgl.h>
#include <TLC59116.h>
#include <Wire.h>
#include <stdio.h>

#include <BLEDevice.h>
#include <BLEService.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <PeakDetection.h> 

PeakDetection peakDetection; 

#define ARRAY_SIZE 40

lv_obj_t *ui_Label1;

// BLE definitions
BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;

#define SERVICE_UUID "adf2a6e6-9b6d-4b5f-a487-77e21aafbc88"  // UUID for the Heart Rate service
#define CHARACTERISTIC_UUID "00002a37-0000-1000-8000-00805f9b34fb"                           // UUID for the Heart Rate Measurement characteristic

// Callback for device connection events
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    Serial.println("Bluetooth connected");
    deviceConnected = true;
  };

  void onDisconnect(BLEServer *pServer) {
    Serial.println("Bluetooth disconnected");
    deviceConnected = false;
    
    delay(100);  // Allow time for central to clean up the connection, then reconnect
    BLEDevice::startAdvertising();
    Serial.println("Advertising restarted.");
  }
};

// Chart series array
static lv_coord_t ui_Chart1_series_1_array[ARRAY_SIZE] = { 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

lv_chart_series_t *ui_Chart1_series_1; // Chart series pointer
lv_obj_t *ui_Chart1;                  // Chart object


TFT_eSPI tft = TFT_eSPI(); // Create an instance of TFT_eSPI

/*LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes*/
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t * px_map) {
    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1);

    // Convert px_map to 16-bit color format if necessary
    tft.pushColors((uint16_t *)px_map, 
                   (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1), true);

    tft.endWrite();
    lv_disp_flush_ready(disp); // Notify LVGL that flushing is done
}

/*use Arduinos millis() as tick source*/
static uint32_t my_tick(void)
{
    return millis();
}


TLC59116 board1(0);

int hr_signal;
char label_buffer[10];

unsigned long bpm = 0; // To store the sum of the intervals for averaging
unsigned long previousPeakTime = 0;  // Variable to store the time of the previous peak
unsigned long currentPeakTime = 0;   // Variable to store the time of the current peak
unsigned long peakInterval = 0;      // Interval between peaks

const int numIntervals = 10;  // Number of intervals to average
unsigned long intervals[numIntervals];  // Array to store the last 5 intervals (in bpm)
unsigned long totalBPM = 0;  // Sum of the last 5 bpm values
int bpmIndex = 0;  // Index for storing the bpm values in the array
unsigned long avgBPM = 0;  // Calculated average of the last 5 bpm values

unsigned long previousMillis = 0; // Stores the last time LVGL was updated
const unsigned long interval = 40; // Update interval in milliseconds




void PulseMeasurementTask(void *parameter) {
    while (true) {

        // BPM Algortihm
        double rawData = (double)analogRead(4);
        double value = (double)rawData / 512 - 1;  // Converts the sensor value to a range between -1 and 1. 
        
        peakDetection.add(value);                     // Adds a new data point
        int peak = peakDetection.getPeak();          // 0, 1, or -1 (detects peaks)
        double filtered = peakDetection.getFilt();   // Moving average filter
        
        // send data over bluetooth
        uint8_t bpmData[3];
        bpmData[0] = 0x00;                      // Flags byte, set to 0x00 for 8-bit heart rate format
        bpmData[1] = (uint8_t)rawData;       // Heart rate measurement as a single byte


        

        // When a peak is detected (value 1), calculate the time interval between peaks
        if (peak == 1) {
            currentPeakTime = millis();  // Get the current time when the peak occurs

            // If this is not the first peak, calculate the time interval between peaks
            if (previousPeakTime != 0) {
                peakInterval = currentPeakTime - previousPeakTime ;  // Calculate the interval
                if (peakInterval > 300 && peakInterval < 1000){
                
                bpm = 1/(peakInterval*0.001) * 60; // calculate bpm and print
                Serial.print("Heart rate: ");
                Serial.print(bpm);
                Serial.println(" bpm");

                // Update the total BPM sum and store the current BPM in the array
                totalBPM -= intervals[bpmIndex];  // Remove the oldest BPM value from totalBPM
                intervals[bpmIndex] = bpm;  // Add the new BPM value to the array
                totalBPM += bpm;  // Add the new BPM value to the total sum

                // Move to the next index (circular buffer)
                bpmIndex = (bpmIndex + 1) % numIntervals;

                // Calculate the average BPM of the last 5 values (if all values are set, aka if last value is set)
                if (intervals[numIntervals - 1] != 0) {
                    avgBPM = totalBPM / numIntervals;
                    Serial.print("Average Heart rate (5 readings): ");
                    Serial.print(avgBPM);  // Print the average heart rate
                    Serial.println(" bpm");
                }           
                }
            }
            // Update the previous peak time to the current one
            previousPeakTime = currentPeakTime;

            


        }

        bpmData[2] = (uint8_t)avgBPM;       // Heart rate measurement as a single byte
        
        if (deviceConnected) {
            pCharacteristic->setValue(bpmData, 3);  // Set characteristic value with flags + bpm
            pCharacteristic->notify();              // Notify connected client with BPM data
        }

        vTaskDelay(pdMS_TO_TICKS(0)); // Wait 40ms before updating again
    }
}


void setup() {

    pinMode(4, INPUT);   
    peakDetection.begin(128, 4, 0.75);  // test different values (lag, threshold, influence)

    board1.begin();
    delay(1000);
    board1.analogWrite(3, 100);
    board1.analogWrite(7, 100);

    tft.begin(); // Initialize TFT
    tft.setRotation(3); // Adjust as needed

    // Initialize BLE
    BLEDevice::init("Optical Pulse Oximeter");  // Set device name
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());  // Set callback for connection events

    // Create a Heart Rate service
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Create a Heart Rate Measurement characteristic
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_NOTIFY);

    pCharacteristic->addDescriptor(new BLE2902());
    pService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // Set connection parameters for BLE
    pAdvertising->setMaxPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("BLE Heart Rate Monitor is now advertising...");

   

    lv_init();

    /*Set a tick source so that LVGL will know how much time elapsed. */
    lv_tick_set_cb(my_tick);

    lv_display_t * disp;
    disp = lv_display_create(TFT_HOR_RES, TFT_VER_RES);
    lv_display_set_flush_cb(disp, my_disp_flush);
    lv_display_set_buffers(disp, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);

    

    // Create a style
    
    static lv_style_t style;
    lv_style_init(&style);

    // Set the background color to black
    lv_style_set_bg_color(&style, lv_color_make(0, 0, 0)); 
    lv_style_set_bg_opa(&style, LV_OPA_COVER);             // Full opacity

    // Apply the style to the active screen
    lv_obj_t *screen = lv_scr_act(); // Get the active screen
    lv_obj_add_style(screen, &style, LV_PART_MAIN | LV_STATE_DEFAULT);
    


    ui_Chart1 = lv_chart_create(lv_scr_act());
    lv_obj_set_width(ui_Chart1, 250);
    lv_obj_set_height(ui_Chart1, 100);
    lv_obj_set_x(ui_Chart1, 1);
    lv_obj_set_y(ui_Chart1, 27);
    lv_obj_set_align(ui_Chart1, LV_ALIGN_CENTER);
    lv_chart_set_type(ui_Chart1, LV_CHART_TYPE_LINE);



    lv_chart_set_point_count(ui_Chart1, ARRAY_SIZE);



    lv_obj_set_style_text_opa(ui_Chart1, LV_OPA_TRANSP, LV_PART_ITEMS | LV_STATE_DEFAULT); // Hide labels
    lv_obj_set_style_line_opa(ui_Chart1, LV_OPA_TRANSP, LV_PART_ITEMS | LV_STATE_DEFAULT); // Hide ticks

    ui_Chart1_series_1 = lv_chart_add_series(ui_Chart1, lv_color_hex(0xFF0000),
                                                                 LV_CHART_AXIS_PRIMARY_Y);

     
    lv_chart_set_ext_y_array(ui_Chart1, ui_Chart1_series_1, ui_Chart1_series_1_array);

   

    lv_obj_set_style_line_width(ui_Chart1, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_chart_set_range(ui_Chart1, LV_CHART_AXIS_PRIMARY_Y, 0, 100);

    //lv_obj_set_style_line_color(ui_Chart1, lv_color_make(255, 17, 0), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui_Chart1, lv_color_hex(0xFF4200), LV_PART_ITEMS | LV_STATE_DEFAULT);

    
    lv_obj_set_style_bg_color(ui_Chart1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Chart1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_Chart1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_Chart1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    
    lv_obj_set_style_line_opa(ui_Chart1, 255, LV_PART_ITEMS | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_Chart1, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Chart1, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_line_color(ui_Chart1, lv_color_hex(0x4040FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui_Chart1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    

    ui_Label1 = lv_label_create(lv_scr_act());
    lv_obj_set_width(ui_Label1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label1, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label1, 45);
    lv_obj_set_y(ui_Label1, -65);
    lv_obj_set_align(ui_Label1, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label1, "0");
    lv_obj_set_style_text_font(ui_Label1, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_Label2 = lv_label_create(lv_scr_act());
    lv_obj_set_width(ui_Label2, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label2, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label2, -24);
    lv_obj_set_y(ui_Label2, -65);
    lv_obj_set_align(ui_Label2, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label2, "BPM - ");
    lv_obj_set_style_text_font(ui_Label2, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_text_color(ui_Label1, lv_color_make(255, 255, 255), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_Label2, lv_color_make(255, 255, 255), LV_PART_MAIN | LV_STATE_DEFAULT);


    // Create the LVGL update task
    xTaskCreatePinnedToCore(
        PulseMeasurementTask,  // Task function
        "LVGLTask",      // Task name
        8192,            // Stack size
        NULL,            // Parameter
        1,               // Priority
        NULL, // Task handle
        1                // Core to run on (ESP32-specific)
    );

    
}


void loop() {

    lv_timer_handler(); // Update LVGL tasks
    delay(40);
    

    // Check BLE connection
    if (!deviceConnected) {
        // If not connected, ensure advertising is active
        //if (!BLEDevice::getAdvertising()->isAdvertising()) {
            BLEDevice::startAdvertising();
            Serial.println("Re-advertising since device is not connected.");
        //}
    }

    analogRead(11);

    // Read the analog value and map it to the chart range (0-100)
    hr_signal = analogRead(4);


    // Map the signal to the chart's range (0-100)
    int mapped_signal = map(hr_signal, 1800, 2200, 0, 100);

    

    // Shift the array values to the left
    for (int i = 0; i < ARRAY_SIZE - 1; i++) {
        ui_Chart1_series_1_array[i] = ui_Chart1_series_1_array[i + 1];
    }

    // Append the new value to the end of the array
    ui_Chart1_series_1_array[ARRAY_SIZE - 1] = mapped_signal;

    // Refresh the chart to display the updated data
    lv_chart_refresh(ui_Chart1);

    Serial.println(hr_signal);

    

    board1.analogWrite(3, 255);
    board1.analogWrite(7, 255);

    snprintf(label_buffer, sizeof(label_buffer), "%lu", avgBPM);
    lv_label_set_text(ui_Label1, label_buffer);


}










