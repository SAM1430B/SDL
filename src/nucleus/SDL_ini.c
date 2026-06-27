// Nucleus Co-Op INI file
#include "SDL_ini.h"
#include "../SDL_internal.h"
#include "../joystick/SDL_joystick_c.h"

// Track the chronological order of connected gamepads
static SDL_JoystickID s_SeenJoysticks[64];
static int s_NumSeenJoysticks = 0;

static int GetChronologicalGamepadIndex(SDL_JoystickID instance_id)
{
    // Check if an index has already been assigned to this specific joystick ID
    for (int i = 0; i < s_NumSeenJoysticks; ++i) {
        if (s_SeenJoysticks[i] == instance_id) {
            return i;
        }
    }

    // If unassigned, allocate the next available chronological index
    if (s_NumSeenJoysticks < 64) {
        s_SeenJoysticks[s_NumSeenJoysticks] = instance_id;
        return s_NumSeenJoysticks++;
    }

    return -1;
}

int SDL_GetPlayerIndexFromINI(SDL_JoystickID instance_id)
{
    int player_index = -2; // Default to Auto-assign
    size_t size = 0;

    int current_gamepad_index = GetChronologicalGamepadIndex(instance_id);
    if (current_gamepad_index < 0) {
        return player_index;
    }

    // Load the configuration file from the working directory
    char *data = (char *)SDL_LoadFile("SDL2.ini", &size);

    if (!data) {
        return player_index; // Configuration file not found
    }

    // Whitelist enforcement: If the configuration file exists, restrict access unless explicitly permitted
    player_index = -1;
    SDL_bool in_player_section = SDL_FALSE;

    char *line = data;
    while (line && *line) {
        char *next_line = SDL_strchr(line, '\n');
        if (next_line) {
            *next_line = '\0';
        }

        char *cr = SDL_strchr(line, '\r');
        if (cr) {
            *cr = '\0';
        }

        char *trimmed = line;
        while (*trimmed == ' ' || *trimmed == '\t') {
            trimmed++;
        }

        // Evaluate section headers
        if (trimmed[0] == '[') {
            if (SDL_strncmp(trimmed, "[PlayerIndex]", 13) == 0) {
                in_player_section = SDL_TRUE;
            } else {
                in_player_section = SDL_FALSE;
            }
        // Process key-value pairs within the target section, ignoring comments
        } else if (trimmed[0] != '#' && trimmed[0] != ';' && trimmed[0] != '\0') {
            if (in_player_section) {
                int parsed_player_idx = -2;
                int parsed_gamepad_idx = -1;

                if (SDL_sscanf(trimmed, "Player%d=%d", &parsed_player_idx, &parsed_gamepad_idx) == 2 ||
                    SDL_sscanf(trimmed, "Player%d = %d", &parsed_player_idx, &parsed_gamepad_idx) == 2) {

                    if (parsed_gamepad_idx == current_gamepad_index) {
                        player_index = parsed_player_idx;
                        break;
                    }
                }
            }
        }

        if (next_line) {
            line = next_line + 1;
        } else {
            break;
        }
    }

    SDL_free(data);
    return player_index;
}
