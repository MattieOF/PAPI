﻿#pragma once
#include <SDL3/SDL_events.h>

#include "Scancode.h"
#include "MouseButtons.h"

class Input
{
public:
	static void Init();
	static void Shutdown();

	NODISCARD static FORCEINLINE bool IsKeyDown(const Scancode key) { return s_Keys[key]; }
	NODISCARD static FORCEINLINE bool IsKeyDownThisFrame(const Scancode key)
	{
		return s_Keys[key] && !s_PreviousKeys[key];
	}

	NODISCARD static FORCEINLINE bool IsKeyUp(const Scancode key) { return !s_Keys[key]; }
	NODISCARD static FORCEINLINE bool IsKeyUpThisFrame(const Scancode key)
	{
		return !s_Keys[key] && s_PreviousKeys[key];
	}

	NODISCARD static FORCEINLINE bool IsMouseButtonDown(const MouseButton button) { return s_MouseButtons[button]; }
	NODISCARD static FORCEINLINE bool IsMouseButtonDownThisFrame(const MouseButton button)
	{
		return s_MouseButtons[button] && !s_PreviousMouseButtons[button];
	}

	NODISCARD static FORCEINLINE bool IsMouseButtonUp(const MouseButton button) { return !s_MouseButtons[button]; }
	NODISCARD static FORCEINLINE bool IsMouseButtonUpThisFrame(const MouseButton button)
	{
		return !s_MouseButtons[button] && s_PreviousMouseButtons[button];
	}

	NODISCARD static FORCEINLINE glm::vec2 GetMousePosition() { return s_MousePosition; }
	NODISCARD static FORCEINLINE glm::vec2 GetMouseDelta() { return s_MouseDelta; }

	FORCEINLINE static void PreUpdate()
	{
		// Can't use memcpy_s here because it's not available on all compilers :(
		memcpy(s_PreviousKeys, s_Keys, PAPI_KEY_COUNT * sizeof(bool));
		memcpy(s_PreviousMouseButtons, s_MouseButtons, PAPI_MOUSE_BUTTON_COUNT * sizeof(bool));
		s_MouseDelta = glm::vec2(0.0f);
	}

	static void ProcessKeyboardInputEvent(const SDL_KeyboardEvent &event);
	static void ProcessMouseInputEvent(const SDL_MouseButtonEvent &event);
	static void ProcessMouseMotionEvent(const SDL_MouseMotionEvent &event);

protected:
	// Remember to update UpdateKeyArrays() and Init() if changing the size of these arrays from PAPI_KEY_COUNT
	// These arrays have to be the same size!
	static bool      s_Keys[PAPI_KEY_COUNT];
	static bool      s_PreviousKeys[PAPI_KEY_COUNT];
	static bool      s_MouseButtons[PAPI_MOUSE_BUTTON_COUNT];
	static bool      s_PreviousMouseButtons[PAPI_MOUSE_BUTTON_COUNT];
	static glm::vec2 s_MousePosition, s_MouseDelta;
};
