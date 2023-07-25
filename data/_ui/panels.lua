function drawMenuPanel()
	if imgui.BeginPanel("MainMenu", 900, 300) then
		if imgui.BeginChild("MainMenu-left", 600, 300) then
			if imgui.Button("Continue") then
				SendEvent("ContinueGame");
			end
			if imgui.Button("LoadGame") then
				SendEvent("LoadGame");
			end
			if imgui.Button("Preferences") then
				SendEvent("Preferences");
			end
			if imgui.Button("Quit") then
				SendEvent("Quit");
			end
		end
		imgui.EndChild(); -- API quirk: always call End child

		imgui.EndPanel()
	end
end
