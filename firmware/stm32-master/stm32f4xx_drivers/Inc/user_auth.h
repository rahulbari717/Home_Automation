/*
 * user_auth.h
 *
 *  Created on: Jan 12, 2026
 *      Author: Rahul B.
 *  Description: User authentication and management
 */

#ifndef INC_USER_AUTH_H_
#define INC_USER_AUTH_H_

#include <stdint.h>
#include "config.h"

/* ===== USER STRUCTURE ===== */
typedef struct {
    uint8_t id;
    char username[USERNAME_LENGTH];
    char pin[PIN_LENGTH + 1];         // 4 digits + null terminator
    uint8_t isAdmin;                  // 1 = Super user, 0 = Normal user
    uint8_t isActive;                 // 1 = Active, 0 = Deleted/Inactive
    uint32_t roomPermissions;         // Bit field for room access (32 rooms max)
} User_t;

/* ===== ROOM PERMISSION BITS ===== */
typedef enum {
    ROOM_HALL           = (1 << 0),
    ROOM_KITCHEN        = (1 << 1),
    ROOM_MASTER_BED     = (1 << 2),
    ROOM_GUEST_BED      = (1 << 3),
    ROOM_STORE          = (1 << 4),
    ROOM_GARDEN         = (1 << 5),
    ROOM_STUDY          = (1 << 6),
    ROOM_PUJA           = (1 << 7),
    ROOM_ALL            = 0xFFFFFFFF
} RoomPermission_t;

/* ===== AUTHENTICATION RESULTS ===== */
typedef enum {
    AUTH_SUCCESS = 0,
    AUTH_INVALID_PIN,
    AUTH_USER_NOT_FOUND,
    AUTH_LOCKOUT
} AuthResult_t;

/* ===== API FUNCTIONS ===== */

/**
 * @brief Initialize user authentication system
 */
void UserAuth_Init(void);

/**
 * @brief Authenticate user with PIN
 * @param pin 4-digit PIN entered by user
 * @return User ID if successful, -1 if failed
 */
int8_t UserAuth_Login(const char *pin);

/**
 * @brief Register new user (admin only)
 * @param username Username
 * @param pin 4-digit PIN
 * @param isAdmin 1 if admin, 0 if normal user
 * @param permissions Room access permissions
 * @return 0 if success, -1 if failed
 */
int8_t UserAuth_RegisterUser(const char *username, const char *pin,
                             uint8_t isAdmin, uint32_t permissions);

/**
 * @brief Delete user (admin only)
 * @param user_id User ID to delete
 * @return 0 if success, -1 if failed
 */
int8_t UserAuth_DeleteUser(uint8_t user_id);

/**
 * @brief Get user by ID
 * @param user_id User ID
 * @return Pointer to user structure, NULL if not found
 */
User_t* UserAuth_GetUser(uint8_t user_id);

/**
 * @brief Get current logged-in user ID
 * @return User ID, or -1 if not logged in
 */
int8_t UserAuth_GetCurrentUser(void);

/**
 * @brief Check if user has permission for a room
 * @param user_id User ID
 * @param room Room permission bit
 * @return 1 if has permission, 0 if not
 */
uint8_t UserAuth_HasRoomPermission(uint8_t user_id, RoomPermission_t room);

/**
 * @brief Get number of active users
 * @return Number of users
 */
uint8_t UserAuth_GetUserCount(void);

/**
 * @brief Factory reset - erase all users except default admin
 */
void UserAuth_FactoryReset(void);

/**
 * @brief Logout current user
 */
void UserAuth_Logout(void);

/**
 * @brief Handle login failures and lockout
 * @return 1 if locked out, 0 if not
 */
uint8_t UserAuth_HandleFailedLogin(void);

/**
 * @brief Clear login attempt counter
 */
void UserAuth_ClearLoginAttempts(void);

#endif /* INC_USER_AUTH_H_ */
