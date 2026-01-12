/*
 * menu.c
 *
 *  Created on: Jan 12, 2026
 *      Author: Rahul B.
 */

#include "menu.h"
#include "lcd.h"
#include "keypad.h"
#include "user_auth.h"
#include "room_control.h"
#include <stdio.h>
#include <string.h>

/* ===== FORWARD DECLARATIONS OF ACTION FUNCTIONS ===== */
// Help & System Info
void Action_ShowAbout(void);
void Action_ShowUptime(void);
void Action_ShowErrorLogs(void);

// Hall Controls
void Action_Hall_MainLight(void);
void Action_Hall_CeilingFan(void);
void Action_Hall_TV(void);

// Kitchen Controls
void Action_Kitchen_Light(void);
void Action_Kitchen_Exhaust(void);
void Action_Kitchen_Coffee(void);

// Master Bedroom Controls
void Action_MBed_NightLamp(void);
void Action_MBed_AC(void);
void Action_MBed_Blinds(void);
void Action_MBed_Fan(void);

// Guest Bedroom Controls
void Action_GBed_NightLamp(void);
void Action_GBed_AC(void);
void Action_GBed_Blinds(void);
void Action_GBed_Fan(void);

// Garden Controls
void Action_Garden_Sprinkler(void);
void Action_Garden_Light(void);
void Action_Garden_Fountain(void);

// User Admin
void Action_RegisterUser(void);
void Action_DeleteUser(void);
void Action_ListUsers(void);
void Action_ChangeAdminPIN(void);
void Action_FactoryReset(void);

/* ===== MENU TREE STRUCTURE ===== */

// === Hall Sub-menu ===
static Menu_t hall_tv = {
    .name = "TV System",
    .child = NULL,
    .action_function = Action_Hall_TV,
    .requires_permission = ROOM_HALL
};

static Menu_t hall_fan = {
    .name = "Ceiling Fan",
    .next = &hall_tv,
    .child = NULL,
    .action_function = Action_Hall_CeilingFan,
    .requires_permission = ROOM_HALL
};

static Menu_t hall_light = {
    .name = "Main Light",
    .next = &hall_fan,
    .child = NULL,
    .action_function = Action_Hall_MainLight,
    .requires_permission = ROOM_HALL
};

static Menu_t hall_menu = {
    .name = "Hall",
    .child = &hall_light,
    .requires_permission = ROOM_HALL
};

// === Kitchen Sub-menu ===
static Menu_t kitchen_coffee = {
    .name = "Coffee Maker",
    .child = NULL,
    .action_function = Action_Kitchen_Coffee,
    .requires_permission = ROOM_KITCHEN
};

static Menu_t kitchen_exhaust = {
    .name = "Exhaust Fan",
    .next = &kitchen_coffee,
    .child = NULL,
    .action_function = Action_Kitchen_Exhaust,
    .requires_permission = ROOM_KITCHEN
};

static Menu_t kitchen_light = {
    .name = "Ceiling Light",
    .next = &kitchen_exhaust,
    .child = NULL,
    .action_function = Action_Kitchen_Light,
    .requires_permission = ROOM_KITCHEN
};

static Menu_t kitchen_menu = {
    .name = "Kitchen",
    .child = &kitchen_light,
    .requires_permission = ROOM_KITCHEN
};

// === Master Bedroom Sub-menu ===
static Menu_t mbed_fan = {
    .name = "Ceiling Fan",
    .child = NULL,
    .action_function = Action_MBed_Fan,
    .requires_permission = ROOM_MASTER_BED
};

static Menu_t mbed_blinds = {
    .name = "Blinds",
    .next = &mbed_fan,
    .child = NULL,
    .action_function = Action_MBed_Blinds,
    .requires_permission = ROOM_MASTER_BED
};

static Menu_t mbed_ac = {
    .name = "AC Control",
    .next = &mbed_blinds,
    .child = NULL,
    .action_function = Action_MBed_AC,
    .requires_permission = ROOM_MASTER_BED
};

static Menu_t mbed_lamp = {
    .name = "Night Lamp",
    .next = &mbed_ac,
    .child = NULL,
    .action_function = Action_MBed_NightLamp,
    .requires_permission = ROOM_MASTER_BED
};

static Menu_t mbed_menu = {
    .name = "Master Bedroom",
    .child = &mbed_lamp,
    .requires_permission = ROOM_MASTER_BED
};

// === Guest Bedroom Sub-menu ===
static Menu_t gbed_fan = {
    .name = "Ceiling Fan",
    .child = NULL,
    .action_function = Action_GBed_Fan,
    .requires_permission = ROOM_GUEST_BED
};

static Menu_t gbed_blinds = {
    .name = "Blinds",
    .next = &gbed_fan,
    .child = NULL,
    .action_function = Action_GBed_Blinds,
    .requires_permission = ROOM_GUEST_BED
};

static Menu_t gbed_ac = {
    .name = "AC Control",
    .next = &gbed_blinds,
    .child = NULL,
    .action_function = Action_GBed_AC,
    .requires_permission = ROOM_GUEST_BED
};

static Menu_t gbed_lamp = {
    .name = "Night Lamp",
    .next = &gbed_ac,
    .child = NULL,
    .action_function = Action_GBed_NightLamp,
    .requires_permission = ROOM_GUEST_BED
};

static Menu_t gbed_menu = {
    .name = "Guest Bedroom",
    .child = &gbed_lamp,
    .requires_permission = ROOM_GUEST_BED
};

// === Garden Sub-menu ===
static Menu_t garden_fountain = {
    .name = "Fountain",
    .child = NULL,
    .action_function = Action_Garden_Fountain,
    .requires_permission = ROOM_GARDEN
};

static Menu_t garden_light = {
    .name = "Porch Light",
    .next = &garden_fountain,
    .child = NULL,
    .action_function = Action_Garden_Light,
    .requires_permission = ROOM_GARDEN
};

static Menu_t garden_sprinkler = {
    .name = "Sprinkler",
    .next = &garden_light,
    .child = NULL,
    .action_function = Action_Garden_Sprinkler,
    .requires_permission = ROOM_GARDEN
};

static Menu_t garden_menu = {
    .name = "Garden",
    .child = &garden_sprinkler,
    .requires_permission = ROOM_GARDEN
};

// === Room Control Main Menu ===
static Menu_t room_garden = {
    .name = "Garden",
    .child = &garden_sprinkler,
    .requires_permission = ROOM_GARDEN
};

static Menu_t room_gbed = {
    .name = "Guest Bedroom",
    .next = &room_garden,
    .child = &gbed_lamp,
    .requires_permission = ROOM_GUEST_BED
};

static Menu_t room_mbed = {
    .name = "Master Bedroom",
    .next = &room_gbed,
    .child = &mbed_lamp,
    .requires_permission = ROOM_MASTER_BED
};

static Menu_t room_kitchen = {
    .name = "Kitchen",
    .next = &room_mbed,
    .child = &kitchen_light,
    .requires_permission = ROOM_KITCHEN
};

static Menu_t room_hall = {
    .name = "Hall",
    .next = &room_kitchen,
    .child = &hall_light,
    .requires_permission = ROOM_HALL
};

static Menu_t room_control = {
    .name = "Room Control",
    .child = &room_hall
};

/*
 * menu.c (Part 2/2) - Continue from Part 1
 * Add this to the end of Part 1 code
 */

// === User Admin Sub-menu ===
static Menu_t user_factory_reset = {
    .name = "Factory Reset",
    .child = NULL,
    .action_function = Action_FactoryReset,
    .requires_permission = 0
};

static Menu_t user_change_pin = {
    .name = "Change PIN",
    .next = &user_factory_reset,
    .child = NULL,
    .action_function = Action_ChangeAdminPIN,
    .requires_permission = 0
};

static Menu_t user_list = {
    .name = "List Users",
    .next = &user_change_pin,
    .child = NULL,
    .action_function = Action_ListUsers,
    .requires_permission = 0
};

static Menu_t user_delete = {
    .name = "Delete User",
    .next = &user_list,
    .child = NULL,
    .action_function = Action_DeleteUser,
    .requires_permission = 0
};

static Menu_t user_register = {
    .name = "Register User",
    .next = &user_delete,
    .child = NULL,
    .action_function = Action_RegisterUser,
    .requires_permission = 0
};

static Menu_t user_admin = {
    .name = "User Admin",
    .child = &user_register
};

// === Help & System Info Sub-menu ===
static Menu_t help_errors = {
    .name = "Error Logs",
    .child = NULL,
    .action_function = Action_ShowErrorLogs,
    .requires_permission = 0
};

static Menu_t help_uptime = {
    .name = "Uptime Stats",
    .next = &help_errors,
    .child = NULL,
    .action_function = Action_ShowUptime,
    .requires_permission = 0
};

static Menu_t help_about = {
    .name = "About Device",
    .next = &help_uptime,
    .child = NULL,
    .action_function = Action_ShowAbout,
    .requires_permission = 0
};

static Menu_t help_menu = {
    .name = "Help & Info",
    .child = &help_about
};

// === MAIN MENU ===
static Menu_t main_user_admin = {
    .name = "User Admin",
    .child = &user_register
};

static Menu_t main_room_control = {
    .name = "Room Control",
    .next = &main_user_admin,
    .child = &room_hall
};

static Menu_t main_help = {
    .name = "Help & Info",
    .next = &main_room_control,
    .child = &help_about
};

static Menu_t root_menu = {
    .name = "MAIN MENU",
    .child = &main_help
};

/* ===== MENU STATE VARIABLES ===== */
static Menu_t *current_menu = &main_help;
static Menu_t *cursor_position = &main_help;

/* ===== NAVIGATION FUNCTIONS ===== */

void Menu_Init(void)
{
    // Set up parent pointers and prev/next links
    // Main menu
    main_help.parent = &root_menu;
    main_help.prev = NULL;
    main_room_control.parent = &root_menu;
    main_room_control.prev = &main_help;
    main_help.next = &main_room_control;
    main_user_admin.parent = &root_menu;
    main_user_admin.prev = &main_room_control;
    main_room_control.next = &main_user_admin;
    main_user_admin.next = NULL;

    // Reset to main menu
    current_menu = &main_help;
    cursor_position = &main_help;
}

void Menu_Display(void)
{
    char line1[17], line2[17];

    // Get current item and next item for display
    if(cursor_position->next != NULL) {
        snprintf(line1, 17, "%s", cursor_position->name);
        snprintf(line2, 17, "%s", cursor_position->next->name);
        LCD_DisplayMenu(line1, line2, 1); // Cursor on line 1
    } else {
        // At last item, show previous and current
        if(cursor_position->prev != NULL) {
            snprintf(line1, 17, "%s", cursor_position->prev->name);
            snprintf(line2, 17, "%s", cursor_position->name);
            LCD_DisplayMenu(line1, line2, 0); // Cursor on line 2
        } else {
            // Only one item
            snprintf(line1, 17, "%s", cursor_position->name);
            LCD_PrintLine(0, line1);
            LCD_PrintLine(1, "");
        }
    }
}

void Menu_MoveUp(void)
{
    if(cursor_position->prev != NULL) {
        cursor_position = cursor_position->prev;
        Menu_Display();
    }
}

void Menu_MoveDown(void)
{
    if(cursor_position->next != NULL) {
        cursor_position = cursor_position->next;
        Menu_Display();
    }
}

void Menu_Enter(void)
{
    // Check permission if required
    if(cursor_position->requires_permission != 0) {
        int8_t user_id = UserAuth_GetCurrentUser();
        if(!UserAuth_HasRoomPermission(user_id, cursor_position->requires_permission)) {
            LCD_Clear();
            LCD_PrintLine(0, "Access Denied!");
            LCD_PrintLine(1, "No Permission");
            LCD_DelayMs(2000);
            Menu_Display();
            return;
        }
    }

    // If has child, navigate down
    if(cursor_position->child != NULL) {
        current_menu = cursor_position;
        cursor_position = cursor_position->child;
        Menu_Display();
    }
    // If has action, execute it
    else if(cursor_position->action_function != NULL) {
        cursor_position->action_function();
        Menu_Display();
    }
}

void Menu_GoBack(void)
{
    if(current_menu->parent != NULL) {
        cursor_position = current_menu;
        current_menu = current_menu->parent;
        Menu_Display();
    }
}

Menu_t* Menu_GetCurrent(void)
{
    return cursor_position;
}

void Menu_HandleLogout(void)
{
    UserAuth_Logout();
    Menu_Init();
}

void Menu_Run(void)
{
    char key;

    Menu_Display();

    while(1) {
        key = Keypad_GetKey();

        if(key != KEY_NONE) {
            switch(key) {
                case KEY_UP:
                    Menu_MoveUp();
                    break;

                case KEY_DOWN:
                    Menu_MoveDown();
                    break;

                case KEY_ENTER:
                    Menu_Enter();
                    break;

                case KEY_BACK:
                    Menu_GoBack();
                    break;

                case KEY_LOGOUT:
                    return; // Exit menu loop to logout

                default:
                    break;
            }
        }

        LCD_DelayMs(10); // Small polling delay
    }
}

/* ===== ACTION FUNCTION IMPLEMENTATIONS ===== */

void Action_ShowAbout(void)
{
    LCD_Clear();
    LCD_PrintLine(0, "Smart Home Hub");
    LCD_PrintLine(1, "v1.0 - 2026");
    LCD_DelayMs(3000);
}

void Action_ShowUptime(void)
{
    LCD_Clear();
    LCD_PrintLine(0, "Uptime:");
    // TODO: Implement actual uptime counter
    LCD_PrintLine(1, "00:12:34");
    LCD_DelayMs(3000);
}

void Action_ShowErrorLogs(void)
{
    LCD_Clear();
    LCD_PrintLine(0, "Error Logs:");
    LCD_PrintLine(1, "No Errors");
    LCD_DelayMs(3000);
}

// Room control actions call the room_control.c functions
void Action_Hall_MainLight(void) {
    Room_ToggleDevice(DEVICE_HALL_LIGHT);
}

void Action_Hall_CeilingFan(void) {
    Room_ToggleDevice(DEVICE_HALL_FAN);
}

void Action_Hall_TV(void) {
    Room_ToggleDevice(DEVICE_HALL_TV);
}

void Action_Kitchen_Light(void) {
    Room_ToggleDevice(DEVICE_KITCHEN_LIGHT);
}

void Action_Kitchen_Exhaust(void) {
    Room_ToggleDevice(DEVICE_KITCHEN_EXHAUST);
}

void Action_Kitchen_Coffee(void) {
    Room_ToggleDevice(DEVICE_KITCHEN_COFFEE);
}

void Action_MBed_NightLamp(void) {
    Room_ControlDimmer(DEVICE_MBED_LAMP);
}

void Action_MBed_AC(void) {
    Room_ControlAC(DEVICE_MBED_AC);
}

void Action_MBed_Blinds(void) {
    Room_ToggleDevice(DEVICE_MBED_BLINDS);
}

void Action_MBed_Fan(void) {
    Room_ToggleDevice(DEVICE_MBED_FAN);
}

void Action_GBed_NightLamp(void) {
    Room_ControlDimmer(DEVICE_GBED_LAMP);
}

void Action_GBed_AC(void) {
    Room_ControlAC(DEVICE_GBED_AC);
}

void Action_GBed_Blinds(void) {
    Room_ToggleDevice(DEVICE_GBED_BLINDS);
}

void Action_GBed_Fan(void) {
    Room_ToggleDevice(DEVICE_GBED_FAN);
}

void Action_Garden_Sprinkler(void) {
    Room_ToggleDevice(DEVICE_GARDEN_SPRINKLER);
}

void Action_Garden_Light(void) {
    Room_ToggleDevice(DEVICE_GARDEN_LIGHT);
}

void Action_Garden_Fountain(void) {
    Room_ToggleDevice(DEVICE_GARDEN_FOUNTAIN);
}

// User admin actions - simplified versions
void Action_RegisterUser(void) {
    LCD_Clear();
    LCD_PrintLine(0, "Register User");
    LCD_PrintLine(1, "Enter Username:");
    // TODO: Implement full registration flow
    LCD_DelayMs(2000);
}

void Action_DeleteUser(void) {
    LCD_Clear();
    LCD_PrintLine(0, "Delete User");
    LCD_PrintLine(1, "Select User ID:");
    LCD_DelayMs(2000);
}

void Action_ListUsers(void) {
    LCD_Clear();
    uint8_t count = UserAuth_GetUserCount();
    char buffer[17];
    snprintf(buffer, 17, "Users: %d", count);
    LCD_PrintLine(0, "User List");
    LCD_PrintLine(1, buffer);
    LCD_DelayMs(3000);
}

void Action_ChangeAdminPIN(void) {
    LCD_Clear();
    LCD_PrintLine(0, "Change PIN");
    LCD_PrintLine(1, "Enter New PIN:");
    LCD_DelayMs(2000);
}

void Action_FactoryReset(void) {
    LCD_Clear();
    LCD_PrintLine(0, "Factory Reset?");
    LCD_PrintLine(1, "5=Yes *=Cancel");

    char key = Keypad_WaitForKey();
    if(key == '5') {
        UserAuth_FactoryReset();
        LCD_Clear();
        LCD_PrintLine(0, "Reset Complete!");
        LCD_DelayMs(2000);
    }
}
