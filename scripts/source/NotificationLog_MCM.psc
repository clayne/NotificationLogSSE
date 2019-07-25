ScriptName NotificationLog_MCM Extends SKI_ConfigBase


String[] g_messages


; Called when the config menu is initialized.
Event OnConfigInit()
	ModName = "$NotificationLog"
EndEvent


; Called when the config menu is closed.
Event OnConfigClose()
EndEvent


; Called when a version update of this script has been detected.
; a_version - The new version.
Event OnVersionUpdate(Int a_version)
EndEvent


; Called when a new page is selected, including the initial empty page.
; a_page - The name of the the current page, or "" if no page is selected.
Event OnPageReset(String a_page)
	If (a_page == "")
		SetCursorFillMode(LEFT_TO_RIGHT)
		g_messages = NotificationLog.GetCachedMessages()
		Int i = 0
		If (g_messages)
			While (i < g_messages.Length)
				AddTextOption(g_messages[i], "")
				i += 1
			EndWhile
		EndIf
	EndIf
EndEvent


; Called when highlighting an option.
; a_option - The option ID.
Event OnOptionHighlight(Int a_option)
	SetInfoText(g_messages[a_option])
EndEvent
