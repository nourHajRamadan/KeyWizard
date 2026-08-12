#include "kwcli.h"

KWCLI::KWCLI(KWConfigInterface* cfg)
{
    config = cfg;
    active = false;
    unlocked = false;
}

void KWCLI::begin()
{
    inputBuffer = "";
}

void KWCLI::setActive(bool state)
{
    active = state;
    Serial.println("Entering Configuration Mode...");
    Serial.println("Enter unlock pin:");
    Serial.println("Use: unlock [6-DIGIT-PIN]");
}

bool KWCLI::isActive()
{
    return active;
}

void KWCLI::process()
{
    if (!active)
        return;

    while (Serial.available())
    {
        char c = Serial.read();

        if (c == '\r')
            continue;

        if (c == '\n')
        {
            processCommand(inputBuffer,2);
            inputBuffer = "";
            continue;
        }

        inputBuffer += c;
    }
}

int failedtrials=16;
void KWCLI::processCommand(String command, int mode)
{
    command.trim();

    // ---------------------------------------------------------
    // HELP
    // ---------------------------------------------------------

    if(command == "help")
    {
        Serial.println("░█░█░█▀▀░█░█░█░█░▀█▀░▀▀█░█▀█░█▀▄░█▀▄");
        Serial.println("░█▀▄░█▀▀░░█░░█▄█░░█░░▄▀░░█▀█░█▀▄░█░█");
        Serial.println("░▀░▀░▀▀▀░░▀░░▀░▀░▀▀▀░▀▀▀░▀░▀░▀░▀░▀▀░");
        Serial.println();
        Serial.println("Commands:");
        Serial.println("init [6-DIGIT-PIN]");
        Serial.println("unlock [6-DIGIT-PIN]");
        Serial.println("change-pin [6-DIGIT-PIN]");
        Serial.println("help");
        Serial.println("get [OPTIONS] [KEY]");
        Serial.println("set [OPTIONS] [KEY] [VALUE]");
        Serial.println("remove all | [OPTIONS] [KEY]");
        Serial.println("OPTIONS: password | favourite | ap-password");
        Serial.println("exit");
        return;
    }

    // ---------------------------------------------------------
    // INIT
    // ---------------------------------------------------------

    if(command.startsWith("init "))
    {
        String pin = command.substring(5);
        pin.trim();

        if(pin.length() != 6)
        {
            Serial.println("PIN must be exactly 6 characters long!");
            return;
        }

        for(int i = 0; i < 6; i++) { 
            if(!isDigit(pin[i])){ 
                Serial.println("PIN must contain digits only!"); 
                return; 
            } 
        }

        if(config->initializeEncryption(pin))
        {
            Serial.println("Encryption initialized successfully.");
            Serial.println("KeyWizard unlocked.");
        }
        else
        {
            Serial.println("Encryption initialization failed.");
        }

        return;
    }

    // ---------------------------------------------------------
    // UNLOCK
    // ---------------------------------------------------------

    if(command.startsWith("unlock "))
    {
        String pin = command.substring(7);
        pin.trim();

        if(pin.length() != 6)
        {
            Serial.println("PIN must be exactly 6 characters long!");
            return;
        }

        Serial.println("Please wait...");
        if(config->unlock(pin))
        {
            Serial.println("KeyWizard unlocked.");
            processCommand("help");
        }
        else
        {
            Serial.println("False PIN. Try again.");
            if(mode==2){
                Serial.print("Trials left: ");
                failedtrials--;
                Serial.println(failedtrials);
                if(!failedtrials){
                    config->clearNVS();
                }
            }
        }

        return;
    }

    if(command.startsWith("change-pin "))
    {
        if(!config->isUnlocked())
        {
            Serial.println("KeyWizard is locked.");
            Serial.println("Use: unlock [6-DIGIT-PIN]");
            return;
        }

        Serial.println("Please Wait...");
        String newPassword = command.substring(11);
        newPassword.trim();

        if(newPassword.length() != 6)
        {
            Serial.println("Password must be 6 characters long!");
            return;
        }

        for(int i = 0; i < 6; i++) { 
            if(!isDigit(newPassword[i])){ 
                Serial.println("PIN must contain digits only!"); 
                return; 
            } 
        }

        if(config->changePin(newPassword))
        {
            Serial.println("Pin changed successfully.");
        }
        else
        {
            Serial.println("Failed to change pin.");
        }

        return;
    }
    // ---------------------------------------------------------
    // EXIT
    // ---------------------------------------------------------

    if(command == "exit")
    {
        active = false;

        Serial.println("Restarting KeyWizard...");

        ESP.restart();

        return;
    }

    // ---------------------------------------------------------
    // EVERYTHING BELOW THIS POINT REQUIRES UNLOCKING
    // ---------------------------------------------------------

    if(!config->isUnlocked())
    {
        Serial.println("KeyWizard is locked.");
        Serial.println("Use: unlock [6-DIGIT-PIN]");

        return;
    }

    // ---------------------------------------------------------
    // GET
    // ---------------------------------------------------------

    if(command.startsWith("get "))
    {
        command = command.substring(4);

        if(command.startsWith("ap-password"))
        {
            Serial.println(config->getPasswordString("ap-password"));
            return;
        }

        if(command.startsWith("password "))
        {
            String key = command.substring(9);

            if(key.length() == 0)
            {
                Serial.println("Invalid command.");
                return;
            }

            Serial.println(config->getPasswordString(key.c_str()));
            return;
        }

        if(command.startsWith("favourite "))
        {
            String key = command.substring(10);

            if(key.length() == 0)
            {
                Serial.println("Invalid command.");
                return;
            }

            Serial.println(config->getFavouriteString(key.c_str()));
            return;
        }

        Serial.println("Invalid get command.");
        return;
    }

    // ---------------------------------------------------------
    // SET
    // ---------------------------------------------------------

    if(command.startsWith("set "))
    {
        command = command.substring(4);

        // -------------------------
        // AP PASSWORD
        // -------------------------

        if(command.startsWith("ap-password "))
        {
            String value = command.substring(12);

            if(value.length() == 0)
            {
                Serial.println("Invalid password.");
                return;
            }

            bool ok = config->setPasswordString("ap-password", value);

            if(ok)
                Serial.println("Setting ap-password successful");
            else
                Serial.println("Setting ap-password failed");

            return;
        }

        // -------------------------
        // PASSWORD
        // -------------------------

        if(command.startsWith("password "))
        {
            String keyAndValue = command.substring(9);

            int space = keyAndValue.indexOf(' ');

            if(space == -1)
            {
                Serial.println("Invalid command.");
                return;
            }

            String key = keyAndValue.substring(0, space);
            String value = keyAndValue.substring(space + 1);

            if(key.length() == 0 || value.length() == 0)
            {
                Serial.println("Invalid command.");
                return;
            }

            bool ok = config->setPasswordString(key.c_str(), value);

            if(ok)
                Serial.println("Setting password string successful");
            else
                Serial.println("Setting password string failed");

            return;
        }

        // -------------------------
        // FAVOURITE
        // -------------------------

        if(command.startsWith("favourite "))
        {
            String keyAndValue = command.substring(10);

            int space = keyAndValue.indexOf(' ');

            if(space == -1)
            {
                Serial.println("Invalid command.");
                return;
            }

            String key = keyAndValue.substring(0, space);
            String value = keyAndValue.substring(space + 1);

            if(key.length() == 0 || value.length() == 0)
            {
                Serial.println("Invalid command.");
                return;
            }

            bool ok = config->setFavouriteString(key.c_str(), value);

            if(ok)
                Serial.println("Setting favourite string successful");
            else
                Serial.println("Setting favourite string failed");

            return;
        }

        Serial.println("Invalid set command.");
        return;
    }

    // ---------------------------------------------------------
    // REMOVE
    // ---------------------------------------------------------

    if(command.startsWith("remove "))
    {
        command = command.substring(7);

        // -------------------------
        // REMOVE EVERYTHING
        // -------------------------

        if(command == "all")
        {
            bool ok = config->clearNVS();

            if(ok)
                Serial.println("Removing all configurations successful");
            else
                Serial.println("Removing all configurations failed");

            return;
        }

        // -------------------------
        // AP PASSWORD
        // -------------------------

        if(command == "ap-password")
        {
            bool ok = config->removePasswordString("ap-password");

            if(ok)
                Serial.println("Removing ap-password successful");
            else
                Serial.println("Removing ap-password failed");

            return;
        }

        // -------------------------
        // PASSWORD
        // -------------------------

        if(command.startsWith("password "))
        {
            String key = command.substring(9);

            if(key.length() == 0)
            {
                Serial.println("Invalid command.");
                return;
            }

            bool ok = config->removePasswordString(key.c_str());

            if(ok)
                Serial.println("Removing password string successful");
            else
                Serial.println("Removing password string failed");

            return;
        }

        // -------------------------
        // FAVOURITE
        // -------------------------

        if(command.startsWith("favourite "))
        {
            String key = command.substring(10);

            if(key.length() == 0)
            {
                Serial.println("Invalid command.");
                return;
            }

            bool ok = config->removeFavouriteString(key.c_str());

            if(ok)
                Serial.println("Removing favourite string successful");
            else
                Serial.println("Removing favourite string failed");

            return;
        }

        Serial.println("Invalid remove command.");
        return;
    }

    // ---------------------------------------------------------
    // UNKNOWN COMMAND
    // ---------------------------------------------------------

    Serial.println("Unknown command");
}

