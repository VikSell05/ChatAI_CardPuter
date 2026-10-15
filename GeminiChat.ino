#include <M5Cardputer.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "arduino_secrets.h"

const char* ssid = SECRET_SSID;
const char* password = SECRET_PSW;
const char* apiKey = SECRET_API;

M5Canvas canvas(&M5Cardputer.Display);
String inputText = ""; 

void setup() {
    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true);
    M5Cardputer.Display.setRotation(1);
    
    canvas.createSprite(240, 135);
    canvas.setTextSize(1);
    canvas.setTextScroll(true); // Scorrimento attivo
    
    canvas.println("Connessione WiFi...");
    canvas.pushSprite(0, 0);
    
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) { 
        delay(500); 
        canvas.print(".");
        canvas.pushSprite(0, 0);
    }
    
    canvas.println("\nConnesso!");
    canvas.setTextColor(TFT_CYAN);
    canvas.print("Tu: ");
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
            
            if (M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER)) {
                if (inputText.length() > 0) {
                    askAI(inputText);
                    inputText = ""; 
                }
            } 
            else if (M5Cardputer.Keyboard.isKeyPressed(8)) { // Backspace
                if (inputText.length() > 0) {
                    inputText.remove(inputText.length() - 1);
                    writeToCanvas("<", TFT_RED, false); 
                }
            } 
            else {
                
                for (auto i : status.word) {
                    inputText += i;
                    canvas.setTextColor(TFT_CYAN);
                    canvas.print(i); 
                }
                
                if(status.del) {
                    inputText.remove(inputText.length() - 1);
                    canvas.setCursor(canvas.getCursorX()-6, canvas.getCursorY());
                    canvas.print("  ");
                    canvas.setCursor(canvas.getCursorX()-6, canvas.getCursorY());

                }
                canvas.pushSprite(0, 0); 
            }
        }
    }
}
