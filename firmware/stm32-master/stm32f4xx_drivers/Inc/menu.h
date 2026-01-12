/*
 * menu.h
 *
 *  Created on: Jan 12, 2026
 *      Author: Rahul B.
 *  Description: Hierarchical menu system using tree structure
 */

#ifndef INC_MENU_H_
#define INC_MENU_H_

#include <stdint.h>

/* ===== MENU NODE STRUCTURE ===== */
typedef struct MenuNode {
    const char *name;                   // Display text
    struct MenuNode *parent;            // Previous menu level
    struct MenuNode *child;             // First submenu
    struct MenuNode *next;              // Next sibling menu item
    struct MenuNode *prev;              // Previous sibling menu item
    void (*action_function)(void);      // Action to execute (for leaf nodes)
    uint8_t requires_permission;        // Room permission required (0 = none)
} Menu_t;

/* ===== API FUNCTIONS ===== */

/**
 * @brief Initialize menu system
 */
void Menu_Init(void);

/**
 * @brief Run the main menu navigation loop
 */
void Menu_Run(void);

/**
 * @brief Navigate to parent menu
 */
void Menu_GoBack(void);

/**
 * @brief Navigate to child menu or execute action
 */
void Menu_Enter(void);

/**
 * @brief Move cursor up (previous item)
 */
void Menu_MoveUp(void);

/**
 * @brief Move cursor down (next item)
 */
void Menu_MoveDown(void);

/**
 * @brief Render current menu on LCD
 */
void Menu_Display(void);

/**
 * @brief Get current menu node
 * @return Pointer to current menu node
 */
Menu_t* Menu_GetCurrent(void);

/**
 * @brief Handle logout
 */
void Menu_HandleLogout(void);

#endif /* INC_MENU_H_ */
