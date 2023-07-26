function drawMenuPanel()
	isLoaded = data["mainmenu.player.loaded"];
	isDead = data["mainmenu.player.dead"];

	if imgui.BeginPanel("MainMenu", 1400, 300) then
		--Column 1: Saved ship image + details
		if imgui.BeginChild("MainMenu-Col1", 100, 300) then
			imgui.Image(data["mainmenu.player.flagship.sprite"], 100, 100)
			imgui.Text(data["mainmenu.player.flagship.name"])
		end
		imgui.EndChild(); -- API quirk: always call End child

		imgui.SameLine();
		
		--Column 2: Saved player details + continue button
		if imgui.BeginChild("MainMenu-Col2", 300, 300) then
			imgui.Text(data["mainmenu.player.name"])
			imgui.Text(data["mainmenu.player.date"])
			imgui.Text(data["mainmenu.player.credits"].." credits")
			imgui.Text(data["mainmenu.player.planet_name"].." - "..data["mainmenu.player.system_name"])
			imgui.Text(data["mainmenu.player.playtime"].." playtime")
			if imgui.Button("Continue") then
				SendEvent("ContinueGame");
			end
		end
		imgui.EndChild(); -- API quirk: always call End child
		
		imgui.SameLine();
		
		--Column 3: empty space to show logo
		imgui.BeginChild("MainMenu-Col3", 200, 300)
		imgui.EndChild(); -- API quirk: always call End child
		
		imgui.SameLine();

		--Column 4: Other buttons
		if imgui.BeginChild("MainMenu-Col4", 200, 300) then
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
