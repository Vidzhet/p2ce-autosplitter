"use strict";

class Page {
	/** @type {Panel} */
	parent;
	/** @type {Panel} */
	content;
	/** @type {Button} */
	button;
	/** @type {string} */
	name;
	/** @type {string} */
	layoutFile;
}

// eslint-disable-next-line no-var
class MainMenu {

	static panels = {
		/** @type {Movie} @static */
		movie: null,
		/** @type {Panel} @static */
		mainMenu: null,
		/** @type {Panel} @static */
		mainMenuLoading: null,
		/** @type {Panel} @static */
		homeList: null,
		/** @type {Button} @static */
		restartspeedrunbtn: null,
		/** @type {Button} @static */
		restartlevelbtn: null,
		/** @type {Button} @static */
		github: null,
		/** @type {Button} @static */
		dlc: null,

		/** @type {Button} @static */
		continueButton: null,

		/** @type {Panel} @static */
		pagesParent: null,
		/** @type {Panel} @static */
		pagesFooter: null,

		/** @type {Image} @static */
		logo: null,

		/** @type {Button} @static */
		backButton: null,

		/** @type {Button} @static */
		enterButton: null,

		/** @type {Button} @static */
		actionButton1: null,

		/** @type {Button} @static */
		actionButton2: null,
	}

	/** @type {Page} @static */
	static currentPage = null;

	static pages = {
		/** @type {Page} */
		play: {
			parent: null, content: null, button: null,
			name: "Play",
			layoutFile: "file://{resources}/layout/pages/main-menu/play.xml"
		},
		/** @type {Page} */
		gameSaves: {
			parent: null, content: null, button: null,
			name: "GameSaves",
			layoutFile: "file://{resources}/layout/pages/main-menu/gamesaves.xml"
		},
		/** @type {Page} */
		options: {
			parent: null, content: null, button: null,
			name: "Settings",
			layoutFile: "file://{resources}/layout/pages/main-menu/settings.xml"
		},
		/** @type {Page} */
		achievements: {
			parent: null, content: null, button: null,
			name: "Achievements",
			layoutFile: "file://{resources}/layout/pages/main-menu/achievements.xml"
		}
	}

	static loaded = false;

	/** @type {Panel} */
	static panelToReturnTo;

	/** @type {string | null} */
	static failedSaveMapName = null;

	static {
		$.RegisterForUnhandledEvent("ChaosShowMainMenu", this.onShowMainMenu.bind(this));
		$.RegisterForUnhandledEvent("ChaosHideMainMenu", this.onHideMainMenu.bind(this));
		$.RegisterForUnhandledEvent("ChaosShowPauseMenu", this.onShowPauseMenu.bind(this));
		$.RegisterForUnhandledEvent("ChaosHidePauseMenu", this.onHidePauseMenu.bind(this));
		$.RegisterForUnhandledEvent("MainMenuBackHome", this.onBackHome.bind(this));
		$.RegisterForUnhandledEvent("MapLoaded", this.onMapLoaded.bind(this));
		$.RegisterForUnhandledEvent("FocusBackButton", this.onFocusBackButton.bind(this));

		$.RegisterForUnhandledEvent("SetActionButtonEnter", this.onSetActionButtonEnter.bind(this));
		$.RegisterForUnhandledEvent("SetActionButton1", this.onSetActionButton1.bind(this));
		$.RegisterForUnhandledEvent("SetActionButton2", this.onSetActionButton2.bind(this));
		$.RegisterForUnhandledEvent("RemoveActionButtonEnter", this.onRemoveActionButtonEnter.bind(this));
		$.RegisterForUnhandledEvent("RemoveActionButton1", this.onRemoveActionButton1.bind(this));
		$.RegisterForUnhandledEvent("RemoveActionButton2", this.onRemoveActionButton2.bind(this));

		$.RegisterForUnhandledEvent("OnSaveMapVersionMismatch", this.onSaveMapVersionMismatch.bind(this));

		$.DispatchEvent('ChaosHideIntroMovie');
	}

    static onEscapeKeyPressed(_eSource, _nRepeats, _focusPanel) {
		if(this.currentPage != null) {
			this.closePage();
			return;
		}
		if (GameInterfaceAPI.GetGameUIState() === GameUIState.PAUSEMENU) {
			$.DispatchEvent('ChaosMainMenuResumeGame');
        }

    }

	/** @param {ActionControl} action */
	static onSetActionButtonEnter(action) {
		this.panels.enterButton.SetPanelEvent("onactivate", action.activateEvent);
		this.panels.enterButton.style.visibility = "visible";
		$("#ActionButtonEnter_Label").text = $.Localize(action.title);
	}

	/** @param {ActionControl} action */
	static onSetActionButton1(action) {
		this.panels.actionButton1.SetPanelEvent("onactivate", action.activateEvent);
		$("#MainMenu").SetPanelEvent("onmenu1", action.activateEvent);
		this.panels.actionButton1.style.visibility = "visible";
		$("#ActionButton1_Label").text = $.Localize(action.title);
	}

	/** @param {ActionControl} action */
	static onSetActionButton2(action) {
		this.panels.actionButton2.SetPanelEvent("onactivate", action.activateEvent);
		$("#MainMenu").SetPanelEvent("onmenu2", action.activateEvent);
		this.panels.actionButton2.style.visibility = "visible";
		$("#ActionButton2_Label").text = $.Localize(action.title);
	}

	static clearActionBar() {
		this.onRemoveActionButtonEnter();
		this.onRemoveActionButton1();
		this.onRemoveActionButton2();
	}

	static onRemoveActionButtonEnter() {
		this.panels.enterButton.style.visibility = "collapse";
	}

	static onRemoveActionButton1() {
		this.panels.actionButton1.style.visibility = "collapse";
		$("#MainMenu").ClearPanelEvent("onmenu1");
	}

	static onRemoveActionButton2() {
		this.panels.actionButton2.style.visibility = "collapse";
		$("#MainMenu").ClearPanelEvent("onmenu2");
	}

	// Called by the onload event in xml
	static onLoad() {
		this.panels.restartspeedrunbtn = $("#RestartSpeedrun");
		this.panels.restartlevelbtn = $("#RestartLevel");
		this.panels.mainMenu = $("#MainMenuInput");
		this.panels.mainMenuLoading = $("#MainMenuBgMapLoading");
		this.panels.homeList = $("#MainMenuHomeList");
		this.panels.github = $("#GitHub");
		this.panels.dlc = $("#Dlc");
		this.panels.continueButton = $("#MainMenuListButtonContinue");
		this.panels.pagesParent = $("#MainMenuPagesParent");
		this.panels.pagesFooter = $("#MainMenuPagesFooter");
		this.panels.logo = $("#MainMenuLogo");
		this.panels.backButton = $("#ActionRestartSpeedrun");
		this.panels.backButton = $("#ActionButtonBack");
		this.panels.enterButton = $("#ActionButtonEnter");
		this.panels.actionButton1 = $("#ActionButton1");
		this.panels.actionButton2 = $("#ActionButton2");
		this.pages.play.parent = $("#MainMenuPlayParent");
		this.pages.play.button = $("#MainMenuListButtonPlay");
		this.pages.play.button = $("#p2ce-autosplitterSTART");
		this.pages.gameSaves.parent = $("#MainMenuLoadParent");
		this.pages.gameSaves.button = $("#MainMenuListButtonLoad");
		this.pages.options.parent = $("#MainMenuOptionsParent");
		this.pages.options.button = $("#MainMenuListButtonOptions");
		this.pages.achievements.parent = $("#MainMenuAchievementsParent");
		this.pages.achievements.button = $("#MainMenuListButtonAchievements");

		this.setGameVersionLabel();

		const isPlushInstalled = this.isPlushDlcInstalled();
		this.panels.dlc.style.visibility = isPlushInstalled ? "collapse" : "visible";

		this.loaded = true;
	}

	static setGameVersionLabel() {
		const versionCvar = GameInterfaceAPI.GetSettingString("version_revolution");
		$("#VersionInfo").text = parseGameVersion(versionCvar).version;
	}

	// Called via the ChaosShowMainMenu event from C++ whenever the main menu is created (Fired before onload)
    static onShowMainMenu() {
		if(!this.loaded) return;

		if(this.currentPage != null) this.closePage(true);
    }

	/** @param {string} mapName */
	static onSaveMapVersionMismatch(mapName) {
		this.failedSaveMapName = mapName;
	}

	static onHideMainMenu() {
	}
	
    static onShowPauseMenu() {
		if(!this.loaded) return;

		this.panels.mainMenu.AddClass("mainmenu__parent-pausemode");

		if(this.currentPage != null) {
			this.openAnimationsSuppressed = false;
			this.closePage(true);
		}
	}
	
    static onHidePauseMenu() {
		this.panels.mainMenu.RemoveClass("mainmenu__parent-pausemode");
	}

	// Called by JS UI code to navigate back to main home list and close all pages
	static onBackHome() {
		this.closePage();
	}

	static onMainMenuFocused() {
		if (GameInterfaceAPI.GetGameUIState() === GameUIState.PAUSEMENU) {
			$("#MainMenuListButtonContinue").SetFocus();
		} else if(GameInterfaceAPI.GetGameUIState() === GameUIState.MAINMENU) {
			$("#MainMenuListButtonPlay").SetFocus();
		}
	}

	/** @param {Panel} panelToReturnTo */
	static onFocusBackButton(panelToReturnTo) {
		this.panels.backButton.SetFocus();
		this.panelToReturnTo = panelToReturnTo;
	}

	static onBackHomeReturnUp() {
		if(this.panelToReturnTo == null) return;
		this.panelToReturnTo.SetFocus();
	}

	/**
	 * @param {string} mapName
	 * @param {boolean} isBackground
	 * */
	static onMapLoaded(mapName, isBackground) {
		if (!isBackground) return;

		this.panels.mainMenu.AddClass("mainmenu__loaded")
		this.panels.mainMenuLoading.AddClass("hidden");
		this.pages.play.button.SetFocus();

		if (this.failedSaveMapName != null) {
			UiToolkitAPI.ShowGenericPopupTwoOptionsBgStyle(
				$.Localize('#SaveVersionMismatch_Popup_Title'),
				$.Localize('#SaveVersionMismatch_Popup_Description'),
				'warning-popup',
				$.Localize('#SaveVersionMismatch_Popup_Restart'),
				() => {
					GameInterfaceAPI.ConsoleCommand("map " + this.failedSaveMapName);
					this.failedSaveMapName = null;
				},
				$.Localize('#SaveVersionMismatch_Popup_Cancel'),
				() => {
					this.failedSaveMapName = null;
				},
				'blur'
			);
		}
	}
	
	static onSTARTSPEEDRUN() {
		GameInterfaceAPI.ConsoleCommand("map wakeup_trigger");
	}
	static onRESTARTLEVEL() {
		GameInterfaceAPI.ConsoleCommand("restart_level");
	}
	static onQuitButtonPressed() {
		this.showQuitPrompt();
	}
	static onRestartSpeedrunButtonPressed() {
		this.showRestartSpeedrunPrompt();
	}
	static onDisconnectButtonPressed() {
		this.showDisconnectPrompt();
	}
	static onContinueButtonPressed() {
		$.DispatchEvent("ChaosMainMenuResumeGame");
	}
	static onPlayButtonPressed() {
		this.openPage(this.pages.play);
	}
	static onLoadButtonPressed() {
		this.openPage(this.pages.gameSaves);
	}
	static onOptionsButtonPressed() {
		this.openPage(this.pages.options);
	}
	static onAchievementsButtonPressed() {
		//this.openPage(this.pages.achievements);
		SteamOverlayAPI.OpenGameOverlayAchievements();
	}

	static showQuitPrompt() {

		// make sure game is paused so we can see the popup if hit from a keybind in-game
		$.DispatchEvent('ChaosMainMenuPauseGame');

		const message =
`${$.Localize('#Revolution_Quit_Confirm')}
<span class="popup-muted-text">${$.Localize('#Revolution_UnsavedChangesWillBeLost')}</span>`;

		UiToolkitAPI.ShowGenericPopupTwoOptionsBgStyle(
			$.Localize('#Revolution_Quit'),
			message,
			'warning-popup',
			$.Localize('#Revolution_Yes'),
			this.quitGame,
			$.Localize('#Revolution_No'),
			() => {},
			'blur'
		);
	}
	
	static showRestartSpeedrunPrompt() {

		// make sure game is paused so we can see the popup if hit from a keybind in-game
		$.DispatchEvent('ChaosMainMenuPauseGame');

		const message =
`${$.Localize('Are you sure you want to restart the speedrun?')}
<span class="popup-muted-text">${$.Localize('LiveSplit timer will be reset.')}</span>`;

		UiToolkitAPI.ShowGenericPopupTwoOptionsBgStyle(
			$.Localize('Restart Speedrun'),
			message,
			'warning-popup',
			$.Localize('Restart'),
			this.onSTARTSPEEDRUN,
			$.Localize('Cancel'),
			() => {},
			"blur"
		);
	}

	static showDisconnectPrompt() {

		// make sure game is paused so we can see the popup if hit from a keybind in-game
		$.DispatchEvent('ChaosMainMenuPauseGame');

		const message =
`${$.Localize('#Revolution_ToMainMenu_Confirm')}
<span class="popup-muted-text">${$.Localize('#Revolution_UnsavedChangesWillBeLost')}</span>`;

		UiToolkitAPI.ShowGenericPopupThreeOptionsBgStyle(
			$.Localize('#Revolution_ToMainMenu'),
			message,
			'warning-popup',
			$.Localize('#Revolution_ToMainMenu_Option1'),
			this.disconnectGame,
			$.Localize('#Revolution_ToMainMenu_Option2'),
			this.quitGame,
			$.Localize('#Revolution_ToMainMenu_Option3'),
			() => {},
			"blur"
		);
	}
	
	static onClickGitHubBtn() {
		SteamOverlayAPI.OpenURL("https://github.com/Vidzhet/p2ce-autosplitter");
	}

	static onClickSupportBtn() {
		SteamOverlayAPI.OpenGameOverlayStore(APPID_PLUSH_DLC, OverlayToStoreFlag.AddToCartAndShow);
	}

	/** @return {boolean} Whether or not the dlc is installed */
	static isPlushDlcInstalled() {
		return GameInterfaceAPI.GetSettingBool("revo_plush_dlc_installed");
	}

	static quitGame() {
		GameInterfaceAPI.ConsoleCommand("quit");
	}
	static disconnectGame() {
		GameInterfaceAPI.ConsoleCommand("disconnect");
	}


	static setBackgroundExposureLowered(isLowered) {
		GameInterfaceAPI.ConsoleCommand("");
	}

	/** @param {Page} page */
	static openPage(page) {
		if(page.content === null) {
			const newPanel = $.CreatePanel("Panel", page.parent, page.name);
			newPanel.LoadLayout(page.layoutFile, false, false);
			newPanel.RegisterForReadyEvents(true);

			page.content = newPanel;
		}

		this.currentPage = page;
		this.updatePageAnimations();
		
		this.panels.homeList.AddClass("hidden");
		this.panels.restartspeedrunbtn.AddClass("hidden");
		this.panels.restartlevelbtn.AddClass("hidden");
		this.panels.github.AddClass("hidden");
		this.panels.dlc.AddClass("hidden");
		
		this.panels.pagesFooter.RemoveClass("hidden");
		this.currentPage.parent.RemoveClass("hidden");
		this.currentPage.parent.SetFocus();

		this.clearActionBar();

		$.DispatchEvent("OpenPage", page.name);

		this.panels.logo.AddClass("retract");
		this.panels.mainMenu.AddClass("mainmenu__parent-pageopen");
	}

	/** @param {boolean} noAnimation */
	static closePage(noAnimation = false) {
		this.updatePageAnimations();

		this.panels.logo.RemoveClass("retract");
		this.panels.mainMenu.RemoveClass("mainmenu__parent-pageopen");

		this.currentPage.parent.AddClass("hidden");
		this.panels.pagesFooter.AddClass("hidden");
		this.panels.homeList.RemoveClass("hidden");
		this.panels.restartspeedrunbtn.RemoveClass("hidden");
		this.panels.restartlevelbtn.RemoveClass("hidden");
		this.panels.github.RemoveClass("hidden");
		this.panels.dlc.RemoveClass("hidden");

		this.currentPage.button.SetFocus();
		this.currentPage = null;
	}

	static openAnimationsSuppressed = true;

	static updatePageAnimations() {
		let hasAnim = this.shouldHavePageAnimations();

		this.panels.homeList.SetHasClass("anim", hasAnim);
		this.panels.pagesFooter.SetHasClass("anim", hasAnim);
		this.currentPage.parent.SetHasClass("anim", hasAnim);
	}

	static shouldHavePageAnimations() {
		if(!GameInterfaceAPI.GetGameUIState() === GameUIState.PAUSEMENU) return true;

		if(this.openAnimationsSuppressed) {
			return true;
		}

		this.openAnimationsSuppressed = true;
		return false;
	}
	
	static onRestartSpeedrun() {
        this.showRestartSpeedrunPrompt();
    }

}
