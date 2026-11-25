/*
 =======================================================================================
  Name        : sdl_erm.c
  Description : Error manager message box through SDL (USE_SDL)

  The SDL counterpart of Erm_fn_iMessageBox in ERM/CpaError.c (its Win32 body is the
  #else of a USE_SDL fence there), as the original engine keeps one Erm_fn_iMessageBox
  per platform (its console builds print the title and the text and return 0).

  The style and the result keep the Win32 message box encoding on every target: the
  callers pass numbers (0x40035: retry and cancel, exclamation icon, topmost; 0x40010:
  OK, stop icon) and test the result as numbers (2: cancel). The values are spelled
  out here as ERM_C_* (no Win32 names outside the Win32 branch).

  No request window: SDL shows the box from the calling thread, which is the engine's
  only thread with USE_SDL. Should SDL fail to show it, the title and the text go to
  stderr and the result is 0, as on the original's consoles.
 =======================================================================================
 */
#ifdef USE_SDL

#include <stdio.h>
#include <SDL3/SDL.h>

#include "ERM/CpaError.h"

/* style: buttons (low four bits) and icon (next four bits) */
#define ERM_C_uiStyleButtons           0x0000000Fu
#define ERM_C_uiStyleIcon              0x000000F0u
#define ERM_C_uiStyleIconStop          0x00000010u
#define ERM_C_uiStyleIconExclamation   0x00000030u
#define ERM_C_uiStyleIconInformation   0x00000040u

/* results */
#define ERM_C_iResultOk                1
#define ERM_C_iResultCancel            2
#define ERM_C_iResultAbort             3
#define ERM_C_iResultRetry             4
#define ERM_C_iResultIgnore            5
#define ERM_C_iResultYes               6
#define ERM_C_iResultNo                7

#define ERM_C_ucMaxButtons             3

/* buttons of each style value 0..5 (OK, OK/cancel, abort/retry/ignore, yes/no/cancel,
   yes/no, retry/cancel), in display order; 0 ends a list */
static const int gs_a_iErmButtonResult[6][ERM_C_ucMaxButtons] =
{
    { ERM_C_iResultOk,     0,                   0                    },
    { ERM_C_iResultOk,     ERM_C_iResultCancel, 0                    },
    { ERM_C_iResultAbort,  ERM_C_iResultRetry,  ERM_C_iResultIgnore  },
    { ERM_C_iResultYes,    ERM_C_iResultNo,     ERM_C_iResultCancel  },
    { ERM_C_iResultYes,    ERM_C_iResultNo,     0                    },
    { ERM_C_iResultRetry,  ERM_C_iResultCancel, 0                    }
};

static const char *Erm_fn_szButtonText(int iResult)
{
    switch (iResult)
    {
    case ERM_C_iResultOk:     return "OK";
    case ERM_C_iResultCancel: return "Cancel";
    case ERM_C_iResultAbort:  return "Abort";
    case ERM_C_iResultRetry:  return "Retry";
    case ERM_C_iResultIgnore: return "Ignore";
    case ERM_C_iResultYes:    return "Yes";
    default:                  return "No";
    }
}

int Erm_fn_iMessageBox(char *_szText, char *_szTitle, unsigned int _uiStyle)
{
    SDL_MessageBoxButtonData a_stButton[ERM_C_ucMaxButtons];
    SDL_MessageBoxData       stBox;
    unsigned int             uiButtons = _uiStyle & ERM_C_uiStyleButtons;
    int                      iNbButtons = 0;
    int                      iResult;
    int                      i;

    if (uiButtons >= sizeof(gs_a_iErmButtonResult) / sizeof(gs_a_iErmButtonResult[0]))
        uiButtons = 0;

    for (i = 0; i < ERM_C_ucMaxButtons && gs_a_iErmButtonResult[uiButtons][i] != 0; i++)
    {
        a_stButton[i].flags    = 0;
        a_stButton[i].buttonID = gs_a_iErmButtonResult[uiButtons][i];
        a_stButton[i].text     = Erm_fn_szButtonText(gs_a_iErmButtonResult[uiButtons][i]);
        iNbButtons++;
    }
    /* Return takes the first button; Escape takes cancel, or the only button */
    a_stButton[0].flags |= SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
    for (i = 0; i < iNbButtons; i++)
    {
        if (a_stButton[i].buttonID == ERM_C_iResultCancel || iNbButtons == 1)
            a_stButton[i].flags |= SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
    }

    SDL_zero(stBox);
    switch (_uiStyle & ERM_C_uiStyleIcon)
    {
    case ERM_C_uiStyleIconStop:        stBox.flags = SDL_MESSAGEBOX_ERROR;       break;
    case ERM_C_uiStyleIconExclamation: stBox.flags = SDL_MESSAGEBOX_WARNING;     break;
    case ERM_C_uiStyleIconInformation: stBox.flags = SDL_MESSAGEBOX_INFORMATION; break;
    default:                           break;
    }
    stBox.flags     |= SDL_MESSAGEBOX_BUTTONS_LEFT_TO_RIGHT;
    stBox.window     = SDL_GetKeyboardFocus();
    stBox.title      = _szTitle;
    stBox.message    = _szText;
    stBox.numbuttons = iNbButtons;
    stBox.buttons    = a_stButton;

    if (!SDL_ShowMessageBox(&stBox, &iResult))
    {
        fprintf(stderr, "title=%s text=%s\n", _szTitle, _szText);
        return 0;
    }
    return iResult;
}

#endif /* USE_SDL */
