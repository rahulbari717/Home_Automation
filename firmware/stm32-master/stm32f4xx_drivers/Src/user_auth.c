/*
 * user_auth.c
 *
 *  Created on: Jan 12, 2026
 *      Author: Rahul B.
 */

#include "user_auth.h"
#include <string.h>

/* ===== PRIVATE VARIABLES ===== */
static User_t users[MAX_USERS];
static uint8_t user_count = 0;
static int8_t current_user_id = -1;
static uint8_t login_attempts = 0;

/* ===== PRIVATE FUNCTIONS ===== */

static int8_t FindUserByPin(const char *pin)
{
    for(uint8_t i = 0; i < MAX_USERS; i++) {
        if(users[i].isActive && strcmp(users[i].pin, pin) == 0) {
            return i;
        }
    }
    return -1;
}

/* ===== PUBLIC API IMPLEMENTATION ===== */

void UserAuth_Init(void)
{
    // Clear all users
    memset(users, 0, sizeof(users));
    user_count = 0;
    current_user_id = -1;
    login_attempts = 0;

    // Create default admin user
    strcpy(users[0].username, "Admin");
    strcpy(users[0].pin, "1234");
    users[0].id = 0;
    users[0].isAdmin = 1;
    users[0].isActive = 1;
    users[0].roomPermissions = ROOM_ALL;

    user_count = 1;
}

int8_t UserAuth_Login(const char *pin)
{
    int8_t user_id = FindUserByPin(pin);

    if(user_id >= 0) {
        current_user_id = user_id;
        login_attempts = 0;
        return user_id;
    }

    return -1;
}

int8_t UserAuth_RegisterUser(const char *username, const char *pin,
                             uint8_t isAdmin, uint32_t permissions)
{
    // Check if current user is admin
    if(current_user_id < 0 || !users[current_user_id].isAdmin) {
        return -1; // Not authorized
    }

    // Check if max users reached
    if(user_count >= MAX_USERS) {
        return -1;
    }

    // Find next available slot
    for(uint8_t i = 0; i < MAX_USERS; i++) {
        if(!users[i].isActive) {
            users[i].id = i;
            strncpy(users[i].username, username, USERNAME_LENGTH - 1);
            users[i].username[USERNAME_LENGTH - 1] = '\0';
            strncpy(users[i].pin, pin, PIN_LENGTH);
            users[i].pin[PIN_LENGTH] = '\0';
            users[i].isAdmin = isAdmin;
            users[i].isActive = 1;
            users[i].roomPermissions = permissions;

            user_count++;
            return i;
        }
    }

    return -1;
}

int8_t UserAuth_DeleteUser(uint8_t user_id)
{
    // Check if current user is admin
    if(current_user_id < 0 || !users[current_user_id].isAdmin) {
        return -1;
    }

    // Cannot delete yourself or user 0 (default admin)
    if(user_id == current_user_id || user_id == 0) {
        return -1;
    }

    if(user_id < MAX_USERS && users[user_id].isActive) {
        users[user_id].isActive = 0;
        user_count--;
        return 0;
    }

    return -1;
}

User_t* UserAuth_GetUser(uint8_t user_id)
{
    if(user_id < MAX_USERS && users[user_id].isActive) {
        return &users[user_id];
    }
    return NULL;
}

int8_t UserAuth_GetCurrentUser(void)
{
    return current_user_id;
}

uint8_t UserAuth_HasRoomPermission(uint8_t user_id, RoomPermission_t room)
{
    if(user_id < MAX_USERS && users[user_id].isActive) {
        // Admins have all permissions
        if(users[user_id].isAdmin) {
            return 1;
        }
        return (users[user_id].roomPermissions & room) != 0;
    }
    return 0;
}

uint8_t UserAuth_GetUserCount(void)
{
    return user_count;
}

void UserAuth_FactoryReset(void)
{
    UserAuth_Init(); // Reinitialize with default admin only
}

void UserAuth_Logout(void)
{
    current_user_id = -1;
}

uint8_t UserAuth_HandleFailedLogin(void)
{
    login_attempts++;

    if(login_attempts >= MAX_LOGIN_ATTEMPTS) {
        return 1; // Locked out
    }

    return 0;
}

void UserAuth_ClearLoginAttempts(void)
{
    login_attempts = 0;
}
