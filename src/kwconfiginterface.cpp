#include "kwconfiginterface.h"

void KWConfigInterface::begin(int ns){
  passwordstrings.end();
  favouritestrings.end();
  if(ns==1)passwordstrings.begin("passstr", false);
  if(ns==2)favouritestrings.begin("favstr", false);
}

std::vector<String> KWConfigInterface::getKeys(int ns){
  if(ns<1 || ns>2) return {};
  begin(ns);
  std::vector<String> keys;
  String keysstring = ns==1?passwordstrings.getString("_KEYS_"):favouritestrings.getString("_KEYS_");
  while(keysstring.length())
  {
    int index = keysstring.indexOf('|');
    if(index == -1){
      if(keysstring.length())keys.push_back(keysstring);
      break;
    }
    String token = keysstring.substring(0, index);
    if(token.length()) keys.push_back(token);
    keysstring = keysstring.substring(index + 1);
  }
  return keys;
}

String KWConfigInterface::getPasswordString(const char* key)
{
    begin(1);

    // _KEYS_ is metadata and is intentionally not encrypted.
    if(strcmp(key, "_KEYS_") == 0)
        return passwordstrings.getString("_KEYS_");

    // Internal encryption data must not be exposed through the normal interface.
    if(strcmp(key, "*ENC_SALT*") == 0 || strcmp(key, "*ENC_TEST*") == 0)
        return "";

    // The configuration must be unlocked before encrypted values can be read.
    if(!unlocked)
        return "KeyWizard locked";

    size_t whatsLeft = passwordstrings.freeEntries();
    Serial.printf("Entry slots left for passwords:  %lu \n", (unsigned long)whatsLeft);

    String encrypted = passwordstrings.getString(key);

    // Preserve your special default values.
    if(encrypted.isEmpty())
    {
        if(strcmp(key, "unlockpin") == 0)
        {
            if(setPasswordString("unlockpin", "000000"))
                return "000000";

            return "No value stored for that key";
        }

        if(strcmp(key, "ap-password") == 0)
        {
            if(setPasswordString("ap-password", "00000000"))
                return "00000000";

            return "No value stored for that key";
        }

        return "No value stored for that key";
    }

    String value = decryptString(encrypted);

    if(value.isEmpty())
        return "Unable to decrypt value";

    return value;
}

int keysoverride=0;
bool KWConfigInterface::setPasswordString(const char* key, String value)
{
    begin(1);

    if(strcmp(key, "*ENC_SALT*") == 0 ||
       strcmp(key, "*ENC_TEST*") == 0 ||
       String(key).indexOf('|') != -1)
    {
        Serial.println("Invalid Key!");
        return false;
    }

    if(!unlocked)
    {
        Serial.println("KeyWizard locked!");
        return false;
    }

    if(strcmp(key, "_KEYS_") == 0 ){
        if(!keysoverride){
            Serial.println("Beware that a formatting error of _KEYS_ might block access to all strings. Enter again to execute change.");
            keysoverride++;
            return false;
        }else{
            if(passwordstrings.putString("_KEYS_", value)){
                Serial.println("Overriding _KEYS_ successful.");
                return true;
            }else{
                Serial.println("Overriding _KEYS_ failed.");
                return false;
            }
        }
    }

    String keys = passwordstrings.getString("_KEYS_");

    if(!keys.length() || !keys.endsWith("|"))
        keys.concat('|');

    String token = "|" + String(key) + "|";

    if(keys.indexOf(token) != -1)
    {
        Serial.println("Key already exists!");
        return false;
    }

    String encrypted = encryptString(value);

    if(encrypted.isEmpty())
    {
        Serial.println("Encryption failed!");
        return false;
    }

    if(passwordstrings.putString(key, encrypted))
    {
        keys.concat(String(key));
        keys.concat('|');

        if(passwordstrings.putString("_KEYS_", keys))
            return true;

        // Don't leave the password behind if updating _KEYS_ failed.
        passwordstrings.remove(key);
    }

    return false;
}

bool KWConfigInterface::removePasswordString(const char* key)
{
    begin(1);

    if(!unlocked)
    {
        Serial.println("KeyWizard locked!");
        return false;
    }

    // Internal entries cannot be removed through this function.
    if(strcmp(key, "_KEYS_") == 0 ||
       strcmp(key, "*ENC_SALT*") == 0 ||
       strcmp(key, "*ENC_TEST*") == 0)
    {
        Serial.println("Invalid Key!");
        return false;
    }

    if(passwordstrings.remove(key))
    {
        String keys = passwordstrings.getString("_KEYS_");

        if(!keys.length() || !keys.endsWith("|"))
            keys.concat('|');

        String token = "|" + String(key) + "|";

        int start = keys.indexOf(token) + 1;

        if(start < 1)
            return false;

        int end = start;

        while(end < keys.length())
        {
            if(keys[end] == '|')
                break;

            end++;
        }

        keys.remove(start, end - start + 1);

        passwordstrings.putString("_KEYS_", keys);

        return true;
    }

    return false;
}

String KWConfigInterface::getFavouriteString(const char* key)
{
    begin(2);

    // _KEYS_ is metadata and is intentionally not encrypted.
    if(strcmp(key, "_KEYS_") == 0)
        return favouritestrings.getString("_KEYS_");

    if(!unlocked)
        return "KeyWizard locked";

    size_t whatsLeft = favouritestrings.freeEntries();
    Serial.printf("Entry slots left for favourites:  %lu \n", (unsigned long)whatsLeft);

    String encrypted = favouritestrings.getString(key);

    if(encrypted.isEmpty())
        return "No value stored for that key";

    String value = decryptString(encrypted);

    if(value.isEmpty())
        return "Unable to decrypt value";

    return value;
}
 
bool KWConfigInterface::setFavouriteString(const char* key, String value)
{
    begin(2);

    if(String(key).indexOf('|') != -1)
    {
        Serial.println("Invalid Key!");
        return false;
    }

    if(!unlocked)
    {
        Serial.println("KeyWizard locked!");
        return false;
    }

    if(strcmp(key, "_KEYS_") == 0 ){
        if(!keysoverride){
            Serial.println("Beware that a formatting error of _KEYS_ might block access\nto all strings and require a reset. Enter again to execute change.");
            keysoverride++;
            return false;
        }else{
            if(favouritestrings.putString("_KEYS_", value)){
                Serial.println("Overriding _KEYS_ successful.");
                return true;
            }else{
                Serial.println("Overriding _KEYS_ failed.");
                return false;
            }
        }
    }

    String keys = favouritestrings.getString("_KEYS_");

    if(!keys.length() || !keys.endsWith("|"))
        keys.concat('|');

    String token = "|" + String(key) + "|";

    if(keys.indexOf(token) != -1)
    {
        Serial.println("Key already exists!");
        return false;
    }

    String encrypted = encryptString(value);

    if(encrypted.isEmpty())
    {
        Serial.println("Encryption failed!");
        return false;
    }

    if(favouritestrings.putString(key, encrypted))
    {
        keys.concat(String(key));
        keys.concat('|');

        if(favouritestrings.putString("_KEYS_", keys))
            return true;

        favouritestrings.remove(key);
    }

    return false;
}

bool KWConfigInterface::removeFavouriteString(const char* key)
{
    begin(2);

    if(!unlocked)
    {
        Serial.println("KeyWizard locked!");
        return false;
    }

    if(strcmp(key, "_KEYS_") == 0)
    {
        Serial.println("Invalid Key!");
        return false;
    }

    if(favouritestrings.remove(key))
    {
        String keys = favouritestrings.getString("_KEYS_");

        if(!keys.length() || !keys.endsWith("|"))
            keys.concat('|');

        String token = "|" + String(key) + "|";

        int start = keys.indexOf(token) + 1;

        if(start < 1)
            return false;

        int end = start;

        while(end < keys.length())
        {
            if(keys[end] == '|')
                break;

            end++;
        }

        keys.remove(start, end - start + 1);

        favouritestrings.putString("_KEYS_", keys);

        return true;
    }

    return false;
}

bool KWConfigInterface::clearNVS(){
  bool ok=true;
  begin(1);
  ok&=passwordstrings.clear();
  begin(2);
  ok&=favouritestrings.clear();

  unlocked = false;
  memset(encryptionKey, 0, sizeof(encryptionKey));

  return ok;
}

//encryption authored by chatgpt
static String base64Encode(const uint8_t* data, size_t length)
{
    size_t outputLength = 0;

    mbedtls_base64_encode(nullptr, 0, &outputLength, data, length);

    unsigned char* output = new unsigned char[outputLength + 1];

    if(mbedtls_base64_encode(output, outputLength, &outputLength, data, length) != 0)
    {
        delete[] output;
        return "";
    }

    output[outputLength] = '\0';

    String result = String((char*)output);

    delete[] output;

    return result;
}

static bool base64Decode(String input, uint8_t*& output, size_t& outputLength)
{
    size_t requiredLength = 0;

    int result = mbedtls_base64_decode(
        nullptr,
        0,
        &requiredLength,
        (const unsigned char*)input.c_str(),
        input.length()
    );

    if(result != MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL)
        return false;

    output = new uint8_t[requiredLength];

    result = mbedtls_base64_decode(
        output,
        requiredLength,
        &outputLength,
        (const unsigned char*)input.c_str(),
        input.length()
    );

    if(result != 0)
    {
        delete[] output;
        output = nullptr;
        outputLength = 0;
        return false;
    }

    return true;
}

bool KWConfigInterface::deriveKey(String password, const uint8_t* salt, uint8_t* key)
{
    SHA256Builder sha256;

    PBKDF2_HMACBuilder pbkdf2(&sha256, password);

    pbkdf2.setSalt(salt, 16);
    pbkdf2.setIterations(100000);

    pbkdf2.calculate();
    pbkdf2.getBytes(key);

    return true;
}

String KWConfigInterface::encryptString(String plaintext)
{
    if(!unlocked)
        return "";

    const size_t ivLength = 12;
    const size_t tagLength = 16;

    uint8_t iv[ivLength];

    for(size_t i = 0; i < ivLength; i++)
        iv[i] = (uint8_t)esp_random();

    size_t plaintextLength = plaintext.length();

    uint8_t* ciphertext = new uint8_t[plaintextLength];

    uint8_t tag[tagLength];

    mbedtls_gcm_context gcm;

    mbedtls_gcm_init(&gcm);

    if(mbedtls_gcm_setkey(&gcm, MBEDTLS_CIPHER_ID_AES, encryptionKey, 256) != 0)
    {
        mbedtls_gcm_free(&gcm);
        delete[] ciphertext;
        return "";
    }

    int result = mbedtls_gcm_crypt_and_tag(
        &gcm,
        MBEDTLS_GCM_ENCRYPT,
        plaintextLength,
        iv,
        ivLength,
        nullptr,
        0,
        (const unsigned char*)plaintext.c_str(),
        ciphertext,
        tagLength,
        tag
    );

    mbedtls_gcm_free(&gcm);

    if(result != 0)
    {
        delete[] ciphertext;
        return "";
    }

    size_t totalLength = ivLength + tagLength + plaintextLength;

    uint8_t* combined = new uint8_t[totalLength];

    memcpy(combined, iv, ivLength);
    memcpy(combined + ivLength, tag, tagLength);
    memcpy(combined + ivLength + tagLength, ciphertext, plaintextLength);

    String resultString = base64Encode(combined, totalLength);

    delete[] ciphertext;
    delete[] combined;

    return resultString;
}

String KWConfigInterface::decryptString(String encrypted)
{
    if(!unlocked)
        return "";

    uint8_t* combined = nullptr;
    size_t combinedLength = 0;

    if(!base64Decode(encrypted, combined, combinedLength))
        return "";

    const size_t ivLength = 12;
    const size_t tagLength = 16;

    if(combinedLength < ivLength + tagLength)
    {
        delete[] combined;
        return "";
    }

    uint8_t* iv = combined;
    uint8_t* tag = combined + ivLength;
    uint8_t* ciphertext = combined + ivLength + tagLength;

    size_t ciphertextLength = combinedLength - ivLength - tagLength;

    uint8_t* plaintext = new uint8_t[ciphertextLength + 1];

    mbedtls_gcm_context gcm;

    mbedtls_gcm_init(&gcm);

    if(mbedtls_gcm_setkey(&gcm, MBEDTLS_CIPHER_ID_AES, encryptionKey, 256) != 0)
    {
        mbedtls_gcm_free(&gcm);
        delete[] combined;
        delete[] plaintext;
        return "";
    }

    int result = mbedtls_gcm_auth_decrypt(
        &gcm,
        ciphertextLength,
        iv,
        ivLength,
        nullptr,
        0,
        tag,
        tagLength,
        ciphertext,
        plaintext
    );

    mbedtls_gcm_free(&gcm);

    if(result != 0)
    {
        delete[] combined;
        delete[] plaintext;
        return "";
    }

    plaintext[ciphertextLength] = '\0';

    String resultString = String((char*)plaintext);

    delete[] combined;
    delete[] plaintext;

    return resultString;
}

bool KWConfigInterface::initializeEncryption(String pin)
{
    if(pin.length() != 6)
        return false;

    begin(1);

    if(passwordstrings.isKey("*ENC_TEST*"))
        return false;

    uint8_t salt[16];

    for(int i = 0; i < 16; i++)
        salt[i] = (uint8_t)esp_random();

    if(!deriveKey(pin, salt, encryptionKey))
        return false;

    String saltString = base64Encode(salt, sizeof(salt));

    unlocked = true;

    String verification = encryptString("KeyWizard authentication test");

    if(verification.isEmpty())
    {
        unlocked = false;
        memset(encryptionKey, 0, sizeof(encryptionKey));
        return false;
    }

    if(!passwordstrings.putString("*ENC_SALT*", saltString))
    {
        unlocked = false;
        memset(encryptionKey, 0, sizeof(encryptionKey));
        return false;
    }

    if(!passwordstrings.putString("*ENC_TEST*", verification))
    {
        passwordstrings.remove("*ENC_SALT*");
        unlocked = false;
        memset(encryptionKey, 0, sizeof(encryptionKey));
        return false;
    }

    return true;
}

bool KWConfigInterface::unlock(String pin)
{
    begin(1);

    String saltString = passwordstrings.getString("*ENC_SALT*");
    String verification = passwordstrings.getString("*ENC_TEST*");

    if(saltString.isEmpty() || verification.isEmpty())
        return false;

    uint8_t* salt = nullptr;
    size_t saltLength = 0;

    if(!base64Decode(saltString, salt, saltLength))
        return false;

    if(saltLength != 16)
    {
        delete[] salt;
        return false;
    }

    uint8_t newKey[32];

    if(!deriveKey(pin, salt, newKey))
    {
        delete[] salt;
        return false;
    }

    memcpy(encryptionKey, newKey, sizeof(encryptionKey));

    unlocked = true;

    String decrypted = decryptString(verification);

    if(decrypted != "KeyWizard authentication test")
    {
        unlocked = false;
        memset(encryptionKey, 0, sizeof(encryptionKey));

        delete[] salt;

        return false;
    }

    delete[] salt;

    return true;
}

bool KWConfigInterface::changePin(String newPassword)
{
    if(!unlocked){
        Serial.println(1);
        return false;
    }
    if(newPassword.length() != 6){
        Serial.println(2);
        return false;
    }
    uint8_t oldKey[32];
    memcpy(oldKey, encryptionKey, sizeof(oldKey));

    std::vector<String> passwordKeys = getKeys(1);
    std::vector<String> passwordValues;

    for(const String& key : passwordKeys)
    {
        String encrypted = passwordstrings.getString(key.c_str());

        if(encrypted.isEmpty()){
            Serial.println(3);
            return false;
        }
        String decrypted = decryptString(encrypted);

        if(decrypted.isEmpty()){
            Serial.println(4);
            return false;
        }
        passwordValues.push_back(decrypted);
    }

    std::vector<String> favouriteKeys = getKeys(2);
    std::vector<String> favouriteValues;

    for(const String& key : favouriteKeys)
    {
        String encrypted = favouritestrings.getString(key.c_str());

        if(encrypted.isEmpty()){
            Serial.println(5);
            return false;
        }
        String decrypted = decryptString(encrypted);

        if(decrypted.isEmpty()){
            Serial.println(6);
            return false;
        }
        favouriteValues.push_back(decrypted);
    }

    uint8_t newSalt[16];

    for(int i = 0; i < 16; i++)
        newSalt[i] = (uint8_t)esp_random();

    uint8_t newKey[32];

    if(!deriveKey(newPassword, newSalt, newKey)){
        Serial.println(7);
        return false;
    }
    memcpy(encryptionKey, newKey, sizeof(encryptionKey));

    std::vector<String> encryptedPasswords;

    for(const String& value : passwordValues)
    {
        String encrypted = encryptString(value);

        if(encrypted.isEmpty())
        {
            memcpy(encryptionKey, oldKey, sizeof(oldKey));
            Serial.println(8);
            return false;
        }

        encryptedPasswords.push_back(encrypted);
    }

    std::vector<String> encryptedFavourites;

    for(const String& value : favouriteValues)
    {
        String encrypted = encryptString(value);

        if(encrypted.isEmpty())
        {
            memcpy(encryptionKey, oldKey, sizeof(oldKey));
            Serial.println(9);
            return false;
        }

        encryptedFavourites.push_back(encrypted);
    }

    String newVerification = encryptString("KeyWizard authentication test");

    if(newVerification.isEmpty())
    {
        memcpy(encryptionKey, oldKey, sizeof(oldKey));
        Serial.println(10);
        return false;
    }

    begin(1);
    for(size_t i = 0; i < passwordKeys.size(); i++)
    {
        if(!passwordstrings.putString(passwordKeys[i].c_str(), encryptedPasswords[i]))
        {
            memcpy(encryptionKey, oldKey, sizeof(oldKey));
            Serial.println(11);
            return false;
        }
    }

    begin(2);
    for(size_t i = 0; i < favouriteKeys.size(); i++)
    {
        if(!favouritestrings.putString(favouriteKeys[i].c_str(), encryptedFavourites[i]))
        {
            memcpy(encryptionKey, oldKey, sizeof(oldKey));
            Serial.println(12);
            return false;
        }
    }

    begin(1);
    String newSaltString = base64Encode(newSalt, sizeof(newSalt));

    if(!passwordstrings.putString("*ENC_SALT*", newSaltString))
    {
        memcpy(encryptionKey, oldKey, sizeof(oldKey));
        Serial.println(13);
        return false;
    }

    if(!passwordstrings.putString("*ENC_TEST*", newVerification))
    {
        memcpy(encryptionKey, oldKey, sizeof(oldKey));
        Serial.println(14);
        return false;
    }

    memset(oldKey, 0, sizeof(oldKey));
    memset(newKey, 0, sizeof(newKey));

    return true;
}

bool KWConfigInterface::isUnlocked()
{
    return unlocked;
}