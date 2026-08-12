#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include <vector>
#include <mbedtls/gcm.h>
#include <mbedtls/md.h>
#include <esp_random.h>
#include <mbedtls/base64.h>
#include <PBKDF2_HMACBuilder.h>
#include <SHA2Builder.h>

class KWConfigInterface
{
public:
    void begin(int ns); //1-->passwords 2-->favourites

    std::vector<String> getKeys(int ns);

    String getPasswordString(const char* key);
    bool setPasswordString(const char* key, String value);
    bool removePasswordString(const char* key);

    String getFavouriteString(const char* key);
    bool setFavouriteString(const char* key, String value);
    bool removeFavouriteString(const char* key);

    bool clearNVS();

    bool initializeEncryption(String pin);
    bool unlock(String pin);
    bool changePin(String newPassword);
    bool isUnlocked();

private:
    Preferences passwordstrings;
    Preferences favouritestrings;

    bool unlocked = false;
    uint8_t encryptionKey[32] = {};

    bool deriveKey(String pin, const uint8_t* salt, uint8_t* key);

    String encryptString(String plaintext);
    String decryptString(String ciphertext);
};