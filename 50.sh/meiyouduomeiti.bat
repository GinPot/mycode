adb root
adb remount

:label
	adb shell am start com.hryt.lamplight/.ui.MainActivity
	@pause
	adb shell am start com.hryt.energymanager/.ui.EnergyManagerActivity
	@pause
	adb shell am start com.hryt.energymonitor/.MainActivity
	@pause
	adb shell am start -n com.hryt.bodycontrol/.ui.VehicleControlActivity
	@pause
goto label