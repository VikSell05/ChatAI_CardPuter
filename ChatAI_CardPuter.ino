#include <M5Cardputer.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include "arduino_secrets.h"

//const char* ssid = SECRET_SSID;
//const char* password = SECRET_PSW;
const char* apiKey = SECRET_API;

M5Canvas canvas(&M5Cardputer.Display);
String inputText = ""; 

void setup() {
    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true);
    M5Cardputer.Display.setRotation(1);
    
    canvas.createSprite(240, 135);
    canvas.setTextSize(1);
    canvas.setTextScroll(true);
    
    // Istruzioni a schermo per il Captive Portal
    canvas.fillSprite(TFT_BLACK);
    canvas.setTextColor(TFT_YELLOW);
    canvas.setCursor(10, 10);
    canvas.println("=== AI CHAT SETUP ===");
    canvas.setTextColor(TFT_WHITE);
    canvas.println("\nTentativo di connessione...");
    canvas.println("Se fallisce, usa il telefono:");
    canvas.setTextColor(TFT_CYAN);
    canvas.println("1. Connettiti alla rete:");
    canvas.println("   'Cardputer_AI'");
    canvas.println("2. Inserisci SSID e Password");
    canvas.pushSprite(0, 0);
    
    WiFiManager wm;
    
    // wm.resetSettings(); // Scommenta e carica il codice una volta se vuoi resettare le reti salvate
    
    // autoConnect prova a connettersi. Se fallisce, crea la rete "Cardputer_AI" e blocca il codice qui 
    // finché non inserisci i dati dal telefono.
    bool res = wm.autoConnect("Cardputer_AI");
    
    if (!res) {
        canvas.fillSprite(TFT_BLACK);
        canvas.setCursor(0, 0);
        canvas.setTextColor(TFT_RED);
        canvas.println("\nTimeout o errore WiFi.");
        canvas.println("Riavvio in corso...");
        canvas.pushSprite(0, 0);
        delay(3000);
        esp_restart();
    }
    
    // Connessione avvenuta con successo (da reti salvate o appena inserite)
    canvas.fillSprite(TFT_BLACK); 
    canvas.setCursor(0, 0);
    canvas.setTextColor(TFT_GREEN);
    canvas.println("WiFi Connesso!");
    canvas.setTextColor(TFT_WHITE);
    canvas.print("IP: ");
    canvas.println(WiFi.localIP());
    canvas.setTextColor(TFT_CYAN);
    canvas.print("\nTu: ");
    canvas.pushSprite(0, 0);
}

// Funzione per scrivere senza andare a capo a meno che non sia richiesto
void writeToCanvas(String text, uint16_t color = TFT_WHITE, bool newLine = false) {
    canvas.setTextColor(color);
    if (newLine) canvas.println(text);
    else canvas.print(text);
    canvas.pushSprite(0, 0);
}

void askAI(String prompt) {
    writeToCanvas("\nGemini sta pensando...", TFT_YELLOW, true);
    
    HTTPClient http;
    String url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash:generateContent?key=" + String(apiKey);
    
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    
    DynamicJsonDocument doc(2048);

    
    JsonObject system_instruction = doc.createNestedObject("system_instruction");
    JsonArray si_parts = system_instruction.createNestedArray("parts");
    JsonObject si_part0 = si_parts.createNestedObject();
  
    si_part0["text"] = "Rispondi SEMPRE in italiano. Sii estremamente sintetico (max 15-20 parole) perché l'utente legge su uno schermo minuscolo. Evita introduzioni inutili.";

    
    JsonArray contents = doc.createNestedArray("contents");
    JsonObject contentObj = contents.createNestedObject();
    JsonArray parts = contentObj.createNestedArray("parts");
    JsonObject partObj = parts.createNestedObject();
    partObj["text"] = prompt;

    String requestBody;
    serializeJson(doc, requestBody);

    int httpResponseCode = http.POST(requestBody);
    
    if (httpResponseCode == 200) {
        String response = http.getString();
        DynamicJsonDocument resDoc(8192);
        deserializeJson(resDoc, response);
        
        if (resDoc["candidates"]) {
            const char* aiText = resDoc["candidates"][0]["content"]["parts"][0]["text"];
            writeToCanvas("\nGemini: ", TFT_GREEN, true);
            writeToCanvas(String(aiText), TFT_WHITE, true);
        } else {
            writeToCanvas("Errore: Risposta vuota.", TFT_RED, true);
        }
    } else {
        writeToCanvas("Errore HTTP: " + String(httpResponseCode), TFT_RED, true);
    }
    http.end();
    writeToCanvas("\nTu: ", TFT_CYAN, false);
}

void loop() {
    M5Cardputer.update();

    if (M5Cardputer.BtnA.wasPressed()) {
        esp_restart();
    }

    if (M5Cardputer.Keyboard.isChange()) {
    if (M5Cardputer.Keyboard.isPressed()) {
        auto status = M5Cardputer.Keyboard.keysState();
        
        if (status.enter) { 
            if (inputText.length() > 0) {
                askAI(inputText);
                inputText = ""; 
            }
        } 
        else if (status.del) {
            if (inputText.length() > 0) {
                inputText.remove(inputText.length() - 1);
                
                int x = canvas.getCursorX();
                int y = canvas.getCursorY();
                
                // Cursor managment (font text size 1 wide 6 pixel, hight 8)
                if (x < 6) { 
                    x = 240 - 6; // Back to endline
                    y -= 8;      // Back to upper line
                } else {
                    x -= 6;      // Backspace 1 char
                }
                
                // Black rectang to graficaly delete char
                canvas.fillRect(x, y, 6, 8, TFT_BLACK); 
                canvas.setCursor(x, y);
                canvas.pushSprite(0, 0);
            }
        } 
        else {
            for (auto i : status.word) {
                inputText += i;
                canvas.setTextColor(TFT_CYAN);
                canvas.print(i); 
            }
            canvas.pushSprite(0, 0); 
        }
    }
}
}
