print("Lua UI script loaded")

buttonLabel = "Yes we have a Button!"

function drawMenuPanel()
	imgui.BeginFullscreen("MenuPanel")
	if imgui.Button(buttonLabel) then
		buttonLabel = "Clicked!"
	end
	imgui.EndFullscreen()
end
