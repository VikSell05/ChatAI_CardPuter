# 🧠AI Chat for M5Stack Cardputer

A portable AI assistant built for the **M5Stack Cardputer**. This project allows you to chat directly with Google's **Gemini 2.5 Flash** model from a tiny, pocket-sized terminal. It features a smart WiFi setup via smartphone and full mini-keyboard support.

## ✨ Features

* **Direct AI Integration:** Communicates with the Google Generative Language API (Gemini 2.5 Flash) via HTTP POST requests.

* **Smart WiFi Setup (Captive Portal):** No hardcoded WiFi credentials! If the device can't find a known network, it creates its own Access Point (`Cardputer_AI`). You can connect with your smartphone to select your WiFi and enter the password.

* **Fully Functional Keyboard:** Type your prompts naturally. The code includes a custom implementation for the `Backspace` key to smoothly delete characters without graphical glitches.

* **Color-Coded UI:** Easy to read terminal interface (Cyan for the User, Green for Gemini, Red for errors, Yellow for system messages).

* **Optimized for Small Screens:** Instructs the AI to be concise (15-20 words max) to fit the Cardputer's compact display perfectly.

## 🛠️ Hardware Requirements

* [M5Stack Cardputer](https://m5stack.com/products/m5stack-cardputer)

## 📦 Software Dependencies

Make sure to install the following libraries in your Arduino IDE via the **Library Manager**:

* `M5Cardputer` by M5Stack

* `ArduinoJson` by Benoit Blanchon

* `WiFiManager` by tzapu

## 🚀 Setup & Installation

1. **Clone the repository:**

   ```
   git clone https://github.com/yourusername/Cardputer-Gemini-Chat.git
   
   ```

2. **Get your Gemini API Key:**

   * Go to [Google AI Studio](https://aistudio.google.com/app/apikey).

   * Create a new API key.

3. **Configure your Secrets:**

   * In the project folder, create a file named `arduino_secrets.h` (if not already present).

   * Add your Gemini API key like this:

     ```
     #define SECRET_API "YOUR_GEMINI_API_KEY_HERE"
     
     ```

4. **Compile and Upload:**

   * Select the M5Stack Cardputer board in the Arduino IDE.

   * Compile and upload the sketch (`GeminiChat.ino`).

## 📱 How to Use

1. **First Boot (WiFi Configuration):**

   * Turn on the Cardputer.

   * The screen will say "Tentativo di connessione..." (Attempting connection).

   * If it's your first time or the known WiFi is out of range, use your smartphone and connect to the WiFi network named **`Cardputer_AI`**.

   * A captive portal will pop up on your phone. Select your home/hotspot WiFi and enter the password.

   * The Cardputer will save these credentials and connect automatically next time!

2. **Chatting:**

   * Type your question using the Cardputer keyboard.

   * Press `ENTER` to send the prompt.

   * Wait a few seconds for Gemini to "think" and reply.

   * Use `Backspace` (the `Del` key) if you make a typo.

3. **Reboot:**

   * Press the `BtnA` (the small button next to the screen, if configured) or use the power switch to restart the device.

## 📝 License

This project is open-source and available under the [MIT License](LICENSE).

## Next Upgrades

* **Other AI integrations**: possibility to insert personal API keys and integrations with other AI models

* **Language**: initial setups for languages

* **Complexity**: possibility to choose the complexity of AI response